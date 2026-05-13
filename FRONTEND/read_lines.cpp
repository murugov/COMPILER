#include "lexer.hpp"
#include "find_file_size.hpp"

front_err_t read_lines (vec_t<char*> *lines, FILE *stream)
{
    ssize_t file_size = find_file_size(stream);
    if (file_size <= 0) { return FRONT_ERROR; }

    char *buffer = (char*)calloc((size_t)(file_size + 1), sizeof(char));
    if (buffer == NULL) { return FRONT_ERROR; }
    
    size_t actual_size = fread(buffer, sizeof(char), (size_t)file_size, stream);
    buffer[actual_size] = '\0';

    char *line_start = buffer;

    for (size_t i = 0; i < actual_size; i++)
    {
        if (buffer[i] == '\n') 
        {
            buffer[i] = '\0';
            
            char *line_copy = strdup(line_start);
            vec_push_back(lines, line_copy);
            
            line_start = &buffer[i + 1];
        }
    }

    if (*line_start != '\0')
    {
        char *line_copy = strdup(line_start);
        vec_push_back(lines, line_copy);
    }

    free(buffer);

    return FRONT_SUCCESS;
}


front_err_t free_lines (vec_t<char*> *lines)
{
    if (lines)
    {
        const size_t count_lines = lines->size;
        for (size_t i = 0; i < count_lines; i++) { free(lines->data[i]); }
    }

    return FRONT_SUCCESS;
}