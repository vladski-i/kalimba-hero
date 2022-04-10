#ifndef __ENGINE_H
#define __ENGINE_H
#include "types.h"
#include "comb.h"

typedef enum engine_status_e {
  ENGINE_OK,
  ENGINE_STARTED,
  ENGINE_ERROR,
  ENGINE_STOPPED
} engine_status;

engine_status engine_init(comb **comb_arr, uint32_t comb_arr_len);
engine_status engine_start();
pitch *engine_poll_notes(uint32_t *notes_read);
float *engine_poll_data(uint32_t *samples_no);
bool *engine_poll_triggers(uint32_t *triggers_read);
engine_status engine_stop();

#endif
