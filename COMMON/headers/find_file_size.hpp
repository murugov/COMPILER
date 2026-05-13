#ifndef FIND_FILE_SIZE_HPP
#define FIND_FILE_SIZE_HPP

#include <stdio.h>
#include <sys/stat.h>

__attribute__ ((always_inline, pure))
inline ssize_t find_file_size(FILE* stream)
{
    struct stat file_info = {};

    if (fstat(fileno(stream), &file_info) != 0) { return -1; }

    return (ssize_t)file_info.st_size;
}

#endif