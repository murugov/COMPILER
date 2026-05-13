#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <stdio.h>
#include <time.h>

extern FILE *log_file;
extern const char* verdict_strings[];

enum log_verdict_t
{
    INFO  = 0x00,
    DEBUG = 0x01,
    WARN  = 0x02, 
    ERROR = 0x03
};

int open_log_file(const char* path);
int close_log_file();

#define LOG(verdict, ...) \
    do { \
        if ((void*)log_file != NULL) \
        { \
            time_t now = time(NULL); \
            struct tm *timeinfo = localtime(&now); \
            char time_str[20] = {0}; \
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo); \
            if (verdict == ERROR) \
                fprintf(log_file, "[%s] %-5s %s:%d: ", time_str, verdict_strings[verdict], __FILE__, __LINE__); \
            else \
                fprintf(log_file, "[%s] %-5s ", time_str, verdict_strings[verdict]); \
            fprintf(log_file, __VA_ARGS__); \
            fprintf(log_file, "\n"); \
            fflush(log_file); \
        } \
    } while(0)

#define ERR_MSG_FORMAT(name) name " in %s:%d: from %s"
#define ERR_MSG_PARAMS __FILE__, __LINE__, __func__

#endif