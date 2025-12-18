#include "logger.h"

#include <stdarg.h>
#include <stdio.h>
// Track enabled sinks using bitwise OR
static uint8_t enabled_sinks =
    LOG_SINK_TEST | LOG_SINK_FLASH | LOG_SINK_DISK | LOG_SINK_USB;

/* File-scope buffer to avoid large stack usage in `log_message`.
 * Single buffer is not reentrant; protect with a mutex if used from
 * multiple tasks/ISRs. Size chosen conservatively for embedded use.
 */
#define LOG_BUFFER_SIZE 1024
static char log_buffer[LOG_BUFFER_SIZE];

// Initialize logger system
void logger_init(void)
{
// Initialize hardware/drivers for FLASH/DISK if needed
#ifdef TEST
    // idk do something?
#else
    stdio_usb_init();
    // TODO: Initialize flash logging when ready
    // TODO: Initialize disk logging when ready
#endif
}

// Enable/disable specific sinks
void logger_set_sink_enabled(uint8_t sink_mask, bool enabled)
{
    if (enabled)
    {
        enabled_sinks |= sink_mask; // Set bits using OR
    }
    else
    {
        enabled_sinks &= ~sink_mask; // Clear bits using AND with inverted mask
    }
}

// Main logging function
void log_message(LOG_LEVEL level, uint8_t sink_mask, const char *fmt, ...)
{
    // Only log to enabled sinks
    sink_mask &= enabled_sinks;

    if (!sink_mask)
    {
        return;
    }

    // Format the message into the file-scope buffer
    va_list args;
    va_start(args, fmt);
    int n = vsnprintf(log_buffer, sizeof(log_buffer), fmt, args);
    va_end(args);

    // Output to each enabled sink

    // Both USB & testing use printf
    if (sink_mask & LOG_SINK_TEST || sink_mask & LOG_SINK_USB)
    {
        /* Prefix with seconds since boot for quick timestamps */
        uint64_t secs = time_us_64() / 1000000ULL;
        printf("[%llu] %s ", (unsigned long long)secs, log_buffer);
    }

    if (sink_mask & LOG_SINK_FLASH)
    {
        // TODO: Implement flash logging
    }

    if (sink_mask & LOG_SINK_DISK)
    {
        // TODO: Implement disk logging
    }
}
