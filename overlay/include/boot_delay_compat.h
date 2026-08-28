#pragma once

#include <switch.h>

/*
 * Diagnostic build: skip only the overlay's hard-coded 25 second graphics
 * startup delay. Keep all other svcSleepThread calls unchanged so the runtime
 * behavior after graphics initialization stays representative.
 */
static inline Result switch_ha_diag_svcSleepThread(s64 nanoseconds) {
    if (nanoseconds == 25LL * 1000LL * 1000LL * 1000LL) {
        return 0;
    }
    return svcSleepThread(nanoseconds);
}

#define svcSleepThread switch_ha_diag_svcSleepThread
