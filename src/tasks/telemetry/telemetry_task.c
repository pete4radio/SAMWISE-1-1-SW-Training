#include "telemetry_task.h"

/*
 * telemetry_task.c - telemetry collection and reporting
 *
 * Functions:
 * - telemetry_task_init(slate_t *slate):
 *     Initialize power monitor (ADM1176) and both MPPT (LT8491). On non-PICO
 *     builds the code scans the I2C bus and configures devices; on PICO
 *     (TEST) builds it initializes mocked instances.
 * - telemetry_task_dispatch(slate_t *slate):
 *     Read ADM1176 voltage/current and LT8491 telemetry, update `slate`
 *     fields (battery/solar voltage & current), log values, and update
 *     RBF (remove-before-flight) detection status.
 *
 * Export:
 * - `telemetry_task`: scheduler descriptor used to register the task.
 */
/*
 * Review notes:
 * - I2C scan cost: scanning all 7-bit addresses with blocking timed reads is
 *   slow at init; limit addresses, probe known addresses first, or remove
 *   full scan in flight.
 * - I2C error handling: check `found_device` before initializing drivers or
 *   set a slate flag / return status so dispatch can skip reads when absent.
 * - Blocking on init: the scan and per-address timeouts block startup; move
 *   to background or shorten per-address timeout to avoid long delays.
 * - Float logging: `%f` may not be available unless float printf is linked;
 *   prefer integer mV/mA logs or enable float support in the toolchain.
 * - 64-bit GPIO print: use `PRIu64`/`PRIx64` or cast to `unsigned long long`
 *   and use `%016llX` for portable formatting of 64-bit values.
 * - Type/overflow checks: converting floats to `uint16_t` for mV/mA may
 *   overflow silently for large inputs—validate or clamp before casting.
 * - Logging style: avoid embedding `\n` in `LOG_*` messages; let the logging
 *   layer handle line endings and severity.
 * - Mock vs real init: ensure mocked instances mirror real error returns so
 *   dispatch logic behaves consistently.
 * - Suggestion: consider returning a status from `telemetry_task_init` or
 *   set a `slate` flag indicating sensor availability.
 */

// Add power monitor instance
static adm1176_t power_monitor;
// Add MPPT instance
static mppt_t solar_charger_monitor;

void telemetry_task_init(slate_t *slate)
{
#ifndef PICO
    LOG_INFO("Scanning the I2C bus...\n");
    bool found_device = false;
    for (uint8_t addr = 0x08; addr < 0x78; ++addr)
    { // Valid 7-bit I2C addresses
        uint8_t rxdata;
        // try_lock equivalent: i2c_read_blocking checks for ACK
        // A read of 1 byte is a common way to check for a device
        // For some devices, a write is better. LT8491 should respond to a read
        // attempt to a valid address. However, a more robust check might
        // involve trying to read a known register. For a simple scan, we just
        // see if we get an ACK. The SDK functions return PICO_ERROR_GENERIC if
        // no device responds.
        LOG_INFO("Scanning MPPT_I2C address 0x%02X\n", addr);
        int ret =
            i2c_read_blocking_until(SAMWISE_MPPT_I2C, addr, &rxdata, 1, false,
                                    make_timeout_time_ms(I2C_TIMEOUT_MS));
        if (ret >= 0)
        { // If ret is not an error code (i.e., ACK received)
            LOG_INFO("MPPT Device at 0x%02X\n", addr);
            found_device = true;
        }
        LOG_INFO("Scanning POWER_I2C address 0x%02X\n", addr);
        ret = i2c_read_blocking_until(SAMWISE_POWER_MONITOR_I2C, addr, &rxdata,
                                      1, false,
                                      make_timeout_time_ms(I2C_TIMEOUT_MS));
        if (ret >= 0)
        { // If ret is not an error code (i.e., ACK received)
            LOG_INFO("Power Monitor Device at 0x%02X\n", addr);
            found_device = true;
        }
    }
    if (!found_device)
    {
        LOG_ERROR("No I2C devices found.\n");
    }

    // Initialize power monitor
    power_monitor = adm1176_mk(SAMWISE_POWER_MONITOR_I2C, ADM1176_I2C_ADDR,
                               ADM1176_DEFAULT_SENSE_RESISTOR);

    // Initialize MPPT
    solar_charger_monitor = mppt_mk(SAMWISE_MPPT_I2C, LT8491_I2C_ADDR);
    mppt_init(&solar_charger_monitor);
#else
    // Initialize mocked PICO power monitor
    power_monitor = adm1176_mk_mock();

    // Initialize mocked PICO MPPT
    solar_charger_monitor = mppt_mk_mock();
#endif
}

void telemetry_task_dispatch(slate_t *slate)
{
    // Read power monitor data from ADM1176
    float voltage = adm1176_get_voltage(&power_monitor);
    float current = adm1176_get_current(&power_monitor);
    LOG_INFO("Power Monitor - Voltage: %.3fV, Current: %.3fA", voltage,
             current);

    // Convert float into mV and mA and write to slate
    slate->battery_voltage = (uint16_t)(voltage * 1000); // Convert to mV
    slate->battery_current = (uint16_t)(current * 1000); // Convert to mA

    // Read telemetry data from the LT8491
    uint16_t solar_vin_voltage = mppt_get_vin_voltage(&solar_charger_monitor);
    uint16_t solar_voltage = mppt_get_voltage(&solar_charger_monitor);
    uint16_t solar_current = mppt_get_current(&solar_charger_monitor);
    uint16_t solar_battery_voltage =
        mppt_get_battery_voltage(&solar_charger_monitor);
    uint16_t solar_battery_current =
        mppt_get_battery_current(&solar_charger_monitor);
    LOG_INFO("Solar Charger - Voltage: %umV, Current: %umA", solar_voltage,
             solar_current);
    LOG_INFO("Solar Charger - VBAT: %umV, Current: %umA", solar_battery_voltage,
             solar_battery_current);
    LOG_INFO("Solar Charger - VIN: %umV", solar_vin_voltage);

    // Write to slate
    slate->solar_voltage = solar_voltage;
    slate->solar_current = solar_current;

    LOG_INFO("GPIO bits: %16lX", (uint64_t)gpio_get_all64());

    slate->is_rbf_detected = !gpio_get(SAMWISE_RBF_DETECT_PIN);
    LOG_INFO("RBF_PIN status: %s",
             slate->is_rbf_detected ? "STILL ATTACHED!" : "REMOVED!");
}

sched_task_t telemetry_task = {.name = "telemetry",
                               .dispatch_period_ms = 1000,
                               .task_init = &telemetry_task_init,
                               .task_dispatch = &telemetry_task_dispatch,
                               /* Set to an actual value on init */
                               .next_dispatch = 0};
