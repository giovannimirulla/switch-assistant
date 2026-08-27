#pragma once

#include <switch.h>

/*
 * HOS 22.5.0 has a tighter system memory pool and Atmosphere reports
 * 0x10801 / 2001-0132 when that pool is exhausted. The notification
 * overlay loader currently requests a 6 MiB process heap even though the
 * Switch Assistant overlay is small. Clamp that request to 2 MiB so the
 * loader leaves more headroom for am/VI and other system services.
 *
 * This is intentionally local to the overlay-loader translation unit and
 * does not change the main Switch Assistant sysmodule heap.
 */
static inline Result switch_ha_svcSetHeapSize(void **out_addr, u64 size) {
    const u64 max_size = 0x200000ULL;
    return svcSetHeapSize(out_addr, size > max_size ? max_size : size);
}

#define svcSetHeapSize switch_ha_svcSetHeapSize
