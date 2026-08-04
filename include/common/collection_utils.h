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
    if (!validate_not_null(array) ||
        !validate_not_null(count) ||
        !validate_not_null(new_item) ||
        !validate_not_null(source_name) ||
        item_size == 0) {
        return false;
    }

    if (!validate_array_index(*count, max_count)) {
        return false;
    }

    if (name_offset > item_size) {
        return false;
    }

    if (name_buffer_size > (item_size - name_offset)) {
        return false;
    }

    if (*count > (SIZE_MAX / item_size)) {
        return false;
    }

    uint8_t *item = (uint8_t *)array + ((size_t)(*count) * item_size);
    if (!safe_memcpy(item, item_size, new_item, item_size)) {
        return false;
    }

    if (!safe_strncpy((char *)(item + name_offset), name_buffer_size, source_name, source_name_limit)) {
        return false;
    }

    (*count)++;
    return true;
}

#endif /* COLLECTION_UTILS_H */
