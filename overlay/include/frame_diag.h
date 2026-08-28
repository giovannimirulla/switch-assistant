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
 * Diagnostic BEGIN+END mode.
 * framebufferBegin() acquires the real buffer, but callers receive a dummy buffer,
 * so memset/drawing cannot touch framebuffer memory. framebufferEnd() is then
 * called for real to release/present the acquired buffer. VSync wait stays bypassed.
 * This isolates the begin/end queue lifecycle from pixel writes and eventWait().
 */
static inline void *frame_diag_framebufferBegin(Framebuffer *fb, u32 *out_stride) {
    static unsigned char dummy[FRAME_DIAG_DUMMY_SIZE] __attribute__((aligned(0x1000)));
    static unsigned int count = 0;

    if (count < 8) frame_diag_log("BEGINEND framebufferBegin enter", count);
    void *real_pixels = framebufferBegin(fb, out_stride);
    if (count < 8) {
        frame_diag_log("BEGINEND framebufferBegin return", (long long)(uintptr_t)real_pixels);
        frame_diag_log("BEGINEND stride", out_stride ? (long long)*out_stride : -1);
        frame_diag_log("BEGINEND fb_size", fb ? (long long)fb->fb_size : -1);
        frame_diag_log("BEGINEND returning dummy", (long long)(uintptr_t)dummy);
    }
    count++;
    return dummy;
}

static inline Result frame_diag_eventWait(Event *event, u64 timeout) {
    (void)event;
    static unsigned int count = 0;
    if (count < 8) frame_diag_log("BEGINEND eventWait bypass", (long long)timeout);
    count++;
    return 0;
}

static inline void frame_diag_framebufferEnd(Framebuffer *fb) {
    static unsigned int count = 0;
    if (count < 8) frame_diag_log("BEGINEND framebufferEnd enter", count);
    framebufferEnd(fb);
    if (count < 8) frame_diag_log("BEGINEND framebufferEnd return", count);
    count++;
}

/* Define macros only after wrappers so wrappers call the real libc/libnx functions. */
#define fopen frame_diag_fopen
#define framebufferBegin frame_diag_framebufferBegin
#define eventWait frame_diag_eventWait
#define framebufferEnd frame_diag_framebufferEnd
