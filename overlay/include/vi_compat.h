#pragma once

/*
 * Compatibility shim for Switch Assistant's notification overlay.
 *
 * main.c contains a helper that sends raw IManagerDisplayService command 6000
 * (AddToLayerStack) to several VI layer stacks. That sequence is not required
 * by the normal libnx/libtesla-style managed-layer setup and appears to be
 * unsafe on newer HOS versions, where it can crash am after the overlay jumps.
 *
 * switch.h is included here first so libnx's serviceDispatchIn macro is fully
 * defined before we override it for this one translation unit. In main.c the
 * only direct use of serviceDispatchIn is vi_add_to_layer_stack(), so turning
 * it into a successful no-op disables those raw layer-stack IPC calls while
 * leaving the rest of the VI initialization path unchanged for diagnosis.
 */
#include <switch.h>

#ifdef serviceDispatchIn
#undef serviceDispatchIn
#endif
#define serviceDispatchIn(...) ((Result)0)
