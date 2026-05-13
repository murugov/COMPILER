#include "logger.hpp"


FILE *log_file = NULL;
const char* verdict_strings[] = {"INFO", "DEBUG", "WARN", "ERROR"};

int open_log_file(const char* path)     // make it possible to create multiple logfiles
{
    log_file = fopen(path, "w");
    if ((void*)log_file == NULL)
    {
        perror("Failed to open log file");
        return 1;
    }
    return 0;
}

int close_log_file()
{
    if ((void*)log_file == NULL)
    {
        fclose(log_file);
        return 0;
    }
    return 1;
}