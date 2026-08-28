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
 * Diagnostic no-presentation mode.
 * init_graphics() still creates VI/display/layer/window/framebuffer exactly as before,
 * but once the main loop starts we never submit a real framebuffer, wait on VSync,
 * or call framebufferEnd(). This isolates layer/VI lifetime from presentation.
 */
static inline void *frame_diag_framebufferBegin(Framebuffer *fb, u32 *out_stride) {
    static unsigned char dummy[FRAME_DIAG_DUMMY_SIZE] __attribute__((aligned(0x1000)));
    static unsigned int count = 0;

    if (out_stride) *out_stride = 1280;
    if (count < 8) {
        frame_diag_log("NOFRAME framebufferBegin bypass", count);
        frame_diag_log("NOFRAME fb_size", fb ? (long long)fb->fb_size : -1);
    }
    count++;
    return dummy;
}

static inline Result frame_diag_eventWait(Event *event, u64 timeout) {
    (void)event;
    static unsigned int count = 0;
    if (count < 8) frame_diag_log("NOFRAME eventWait bypass", (long long)timeout);
    count++;
    return 0;
}

static inline void frame_diag_framebufferEnd(Framebuffer *fb) {
    (void)fb;
    static unsigned int count = 0;
    if (count < 8) frame_diag_log("NOFRAME framebufferEnd bypass", count);
    count++;
}

/* Define macros only after wrappers so logging still uses the real libc fopen(). */
#define fopen frame_diag_fopen
#define framebufferBegin frame_diag_framebufferBegin
#define eventWait frame_diag_eventWait
#define framebufferEnd frame_diag_framebufferEnd
