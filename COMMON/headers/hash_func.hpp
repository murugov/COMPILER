#ifndef HASH_FUNC_HPP
#define HASH_FUNC_HPP

#include "CONFIG.hpp"

#include <stdio.h>
#include <ctype.h>

typedef size_t hash_t;

__attribute__ ((always_inline, pure))
inline hash_t hash_func_str(char * __restrict ptr)
{
    hash_t new_hash = 0;

    size_t len = 0;
    while (!isspace(*ptr) && *ptr != '\0' && len < MAX_LEN_STR_FOR_HASH)
    {
        new_hash = (new_hash << 5) - new_hash + (hash_t)(*ptr++);
        len++;
    }

    return new_hash;
}


#endif