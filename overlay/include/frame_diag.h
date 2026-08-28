#pragma once

#include <switch.h>
#include <switch/display/framebuffer.h>
#include <stdio.h>
#include <string.h>

#define FRAME_DIAG_LOG_PATH "sdmc:/switch/switch-ha/overlay.log"
#define FRAME_DIAG_NOTIFICATION_PATH "sdmc:/switch/switch-ha/notification-current.ini"
#define FRAME_DIAG_DUMMY_SIZE (1024 * 1024)

static inline void frame_diag_log(const char *message, long long value) {
    FILE *file = fopen(FRAME_DIAG_LOG_PATH, "a");
    if (!file) return;
    fprintf(file, "%llu DIAG %s value=%lld\n",
            (unsigned long long)(armTicksToNs(armGetSystemTick()) / 1000000ULL),
            message,
            value);
    fclose(file);
}

static inline FILE *frame_diag_fopen(const char *path, const char *mode) {
    const bool is_notification = path && strcmp(path, FRAME_DIAG_NOTIFICATION_PATH) == 0;
    if (is_notification) frame_diag_log("notification fopen enter", 0);
    FILE *file = fopen(path, mode);
    if (is_notification) frame_diag_log("notification fopen return", (long long)(uintptr_t)file);
    return file;
}

/*
 * Diagnostic ONEBEGIN mode.
 * Exactly one real framebufferBegin() is executed. The real pointer is never
 * exposed to the renderer, framebufferEnd() is always bypassed, and all later
 * framebufferBegin() calls are bypassed too. eventWait() remains bypassed.
 */
static inline void *frame_diag_framebufferBegin(Framebuffer *fb, u32 *out_stride) {
    static unsigned char dummy[FRAME_DIAG_DUMMY_SIZE] __attribute__((aligned(0x1000)));
    static unsigned int count = 0;

    if (count == 0) {
        frame_diag_log("ONEBEGIN framebufferBegin real enter", 0);
        void *real_pixels = framebufferBegin(fb, out_stride);
        frame_diag_log("ONEBEGIN framebufferBegin real return", (long long)(uintptr_t)real_pixels);
        frame_diag_log("ONEBEGIN stride", out_stride ? (long long)*out_stride : -1);
        frame_diag_log("ONEBEGIN fb_size", fb ? (long long)fb->fb_size : -1);
    } else {
        if (out_stride) *out_stride = 1280;
        if (count < 8) frame_diag_log("ONEBEGIN framebufferBegin bypass", count);
    }

    count++;
    return dummy;
}

static inline Result frame_diag_eventWait(Event *event, u64 timeout) {
    (void)event;
    static unsigned int count = 0;
    if (count < 8) frame_diag_log("ONEBEGIN eventWait bypass", (long long)timeout);
    count++;
    return 0;
}

static inline void frame_diag_framebufferEnd(Framebuffer *fb) {
    (void)fb;
    static unsigned int count = 0;
    if (count < 8) frame_diag_log("ONEBEGIN framebufferEnd bypass", count);
    count++;
}

/* Define macros only after wrappers so wrappers call the real libc/libnx functions. */
#define fopen frame_diag_fopen
#define framebufferBegin frame_diag_framebufferBegin
#define eventWait frame_diag_eventWait
#define framebufferEnd frame_diag_framebufferEnd
