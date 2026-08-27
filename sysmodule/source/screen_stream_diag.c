#include "screen_stream.h"

#include <stdio.h>

/*
 * Diagnostic replacement for source/screen_stream.c.
 *
 * HOS 22.5.0 is crashing am shortly after the notification overlay creates
 * its VI layer while the sysmodule is also holding an active grcd capture.
 * This build intentionally removes every grcd/capture operation so we can
 * determine whether the two subsystems are interacting.
 */

#define RESULT_NOT_INITIALIZED MAKERESULT(Module_Libnx, LibnxError_NotInitialized)

bool screen_stream_start(AppState *state) {
    (void)state;
    return false;
}

void screen_stream_stop(void) {
}

void screen_stream_set_paused(bool paused) {
    (void)paused;
}

u16 screen_stream_port(void) {
    return 0;
}

void screen_stream_get_status(char *out, size_t out_size) {
    if (!out || out_size == 0) {
        return;
    }
    snprintf(out, out_size, "disabled-diagnostic");
}

Result screen_stream_get_grcd_open_result(void) {
    return RESULT_NOT_INITIALIZED;
}

Result screen_stream_get_grcd_begin_result(void) {
    return RESULT_NOT_INITIALIZED;
}
