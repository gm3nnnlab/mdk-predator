#ifndef SUBGHZ_SIGNAL_VALIDATION_UTILS_H
#define SUBGHZ_SIGNAL_VALIDATION_UTILS_H

#include "input_validation.h"
#include "wireless/subghz_analyzer.h"

static inline bool validate_subghz_iq_signal(const rf_signal_t *signal) {
    if (!validate_not_null(signal) ||
        !validate_not_null(signal->i_samples) ||
        !validate_not_null(signal->q_samples)) {
        return false;
    }

    return validate_buffer_length(signal->sample_count, MAX_SIGNAL_BUFFER_SIZE) == VALIDATION_OK;
}

#endif /* SUBGHZ_SIGNAL_VALIDATION_UTILS_H */
