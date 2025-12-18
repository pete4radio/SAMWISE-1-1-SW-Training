/**
 * @author  Niklas Vainio
 * @date    2024-08-23
 *
 * This file contains the main entry point for the SAMWISE flight code.
 */

#include "flash.h"
#include "init.h"
#include "logger.h"
#include "macros.h"
#include "neopixel.h"
#include "rfm9x.h"
#include "safe_sleep.h"
#include "scheduler.h"
#include "slate.h"

// Slate is initialized in slate.c
extern slate_t slate;

#ifndef PICO
// Ensure that PICO_RP2350A is defined to 0 for PICUBED builds.
// This is to enable full 48pin GPIO support on the RP2350B chip.
// boards/samwise_picubed.h should define it to 0.
// The CMakeLists.txt file points to this file for the board definition.
static_assert(PICO_RP2350A == 0,
              "PICO_RP2350A must be defined to 0 for PICUBED builds.");
#endif

/**
 * Main code entry point.
 *
 * SAMWISE is a 2U Cubesat with four processors:  This is the flight
 * processor, a GNC processor, a reaction wheel controller, and a 
 * RPi payload processor with a camera.  The processors communicate 
 * via UART.
 * 
 * Key features are listed below:
 * - RFM9x LoRa Radio for ground communication
 * - Neopixel RGB LED for status indication
 * - Persistent flash storage for reboot counter and other data
 * - Watchdog timer to ensure system reliability
 * - State machine scheduler for managing operational states
 * - Safe sleep function to handle delays without triggering the watchdog
 * - Power telemetry monitoring for battery and solar panel status
 * - over-the-air flight firmware updates
 * 
 */
int main()
{
    // Initialize watchdog before any sleep is called.
    // The external Watchdog timer is reset regularly by the firmware; if
    // the flight software fails due to latchup or other issues, the watchdog
    // powers off the flight processor and powers it on again to reboot.
    slate.watchdog = watchdog_mk();
    watchdog_init(&slate.watchdog);

/*
 * Brief delay after reboot/powering up due to power spikes to prevent
 * deployment when satellite is still within the launch mechanism.
 */
#ifdef FLIGHT
    // 15 minutes delay is required for dispersion after release from
    // exolaunch deployment chute.
    safe_sleep_ms(15 * 60 * 1000); // 15 minutes
#else
    safe_sleep_ms(5000); // 5 second for debugging
#endif

    /*
     * Initialize persistent data or load existing data if already in flash.
     * The reboot counter is incremented each time this code runs.
     */
    persistent_data_t *data = init_persistent_data();
    increment_reboot_counter();
    LOG_INFO("Current reboot count: %d", data->reboot_counter);

    /*
     * Initialize everything.
     */
    LOG_DEBUG("main: Slate uses %d bytes", sizeof(slate));
    LOG_INFO("main: Initializing...");
    ASSERT(init(&slate));
    slate.reboot_counter = data->reboot_counter;
    LOG_INFO("main: Initialized successfully!\n\n\n");

    /*
     * Print commit hash
     */
#ifdef COMMIT_HASH
    LOG_INFO("main: Running samwise-flight-software %s", COMMIT_HASH);
#endif

    neopixel_set_color_rgb(0, 0xff, 0xff);

    /*
     * Go state machine!
     */
    LOG_INFO("main: Dispatching the state machine...");
    while (true)
    {
        sched_dispatch(&slate);
    }

    /*
     * We should NEVER be here so something bad has happened.
     * @todo reboot!
     */

    ERROR("Wait for WDT reboot");
}
