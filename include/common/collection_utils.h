#ifndef COLLECTION_UTILS_H
#define COLLECTION_UTILS_H

#include <stddef.h>
#include <stdint.h>
#include "input_validation.h"

static inline bool append_named_item(void *array,
                                     size_t item_size,
                                     uint32_t *count,
                                     uint32_t max_count,
                                     const void *new_item,
                                     size_t name_offset,
                                     size_t name_buffer_size,
                                     const char *source_name,
                                     size_t source_name_limit) {
    if (!validate_array_index(*count, max_count)) {
        return false;
    }

    uint8_t *item = (uint8_t *)array + ((*count) * item_size);
    safe_memcpy(item, item_size, new_item, item_size);
    safe_strncpy((char *)(item + name_offset), name_buffer_size, source_name, source_name_limit);

    (*count)++;
    return true;
}

#endif /* COLLECTION_UTILS_H */
