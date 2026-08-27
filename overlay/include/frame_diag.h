#pragma once

#include <switch.h>
#include <switch/display/framebuffer.h>
#include <stdio.h>

#define FRAME_DIAG_LOG_PATH "sdmc:/switch/switch-ha/overlay-frame.log"

static inline void frame_diag_log(const char *message, long long value) {
    FILE *file = fopen(FRAME_DIAG_LOG_PATH, "a");
    if (!file) return;
    fprintf(file, "%llu %s value=%lld\n",
            (unsigned long long)(armTicksToNs(armGetSystemTick()) / 1000000ULL),
            message,
            value);
    fclose(file);
}

static inline void *frame_diag_framebufferBegin(Framebuffer *fb, u32 *out_stride) {
    static unsigned int count = 0;
    if (count < 8) frame_diag_log("framebufferBegin enter", count);
    void *pixels = framebufferBegin(fb, out_stride);
    if (count < 8) {
        frame_diag_log("framebufferBegin return", (long long)(uintptr_t)pixels);
        frame_diag_log("framebuffer stride", out_stride ? (long long)*out_stride : -1);
        frame_diag_log("framebuffer fb_size", (long long)fb->fb_size);
    }
    count++;
    return pixels;
}

static inline Result frame_diag_eventWait(Event *event, u64 timeout) {
    static unsigned int count = 0;
    if (count < 8) frame_diag_log("eventWait enter", (long long)timeout);
    Result rc = eventWait(event, timeout);
    if (count < 8) frame_diag_log("eventWait return", (long long)rc);
    count++;
    return rc;
}

static inline void frame_diag_framebufferEnd(Framebuffer *fb) {
    static unsigned int count = 0;
    if (count < 8) frame_diag_log("framebufferEnd enter", count);
    framebufferEnd(fb);
    if (count < 8) frame_diag_log("framebufferEnd return", count);
    count++;
}

/*
 * Define these only after the wrappers above so their calls resolve to the
 * original libnx functions rather than recursing through the macros.
 */
#define framebufferBegin frame_diag_framebufferBegin
#define eventWait frame_diag_eventWait
#define framebufferEnd frame_diag_framebufferEnd
