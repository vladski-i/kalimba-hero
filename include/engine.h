#ifndef __ENGINE_H
#define __ENGINE_H
#include "types.h"

typedef enum engine_status_e {
    ENGINE_OK,
    ENGINE_STARTED,
    ENGINE_ERROR
} engine_status;

engine_status engine_init();
engine_status engine_start();
pitch *engine_poll_notes(uint32_t *notes_read);

#endif
