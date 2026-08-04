#ifndef KEYFOB_SIGNAL_VALIDATION_UTILS_H
#define KEYFOB_SIGNAL_VALIDATION_UTILS_H

#include "input_validation.h"
#include "automotive/key_fob_analyzer.h"

static inline bool validate_keyfob_signal_payload(const signal_data_t *signal) {
    if (!validate_not_null(signal) || !validate_not_null(signal->data)) {
        return false;
    }

    return validate_buffer_length(signal->length, MAX_SIGNAL_BUFFER_SIZE) == VALIDATION_OK;
}

#endif /* KEYFOB_SIGNAL_VALIDATION_UTILS_H */
