#ifndef JLOGS_H
#define JLOGS_H

// error/log structs
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <jlib/jtime.h>

// Custom Types
typedef struct Error {
    tm systime;         // actual system time
    time_st err_time;   // "string" (char* system_time)
    const char *file;   // filepath, if applicable
    const char *func;   // name of func on stack (may be null)
    const char *msg;    // jlib's error message (string)
    int line;           // line in question, in *file
    int err_code;       // actual error code
    int exit_code;      // actual exit code
    int jlog_code;      // jlib's error code 
    int exit;           // exit code, provided by `errno`
    bool fatal;         // should we panic?
} err;

typedef struct Logger {
    Error error;        // jlib's error struct
    time_st log_time;   // jlib's timestamp struct
    const char* file;
    const char* line;
} log;

void init_err(err *e);
void set_err(err *e);
void set_errno(err *e);

void jlog_print(FILE *out, const err *e);
void jlog_panic(const err *e);

/* Coinvenience MACROS */
#define set_err(e, code, msg) \
    _set_err((e), (code), (msg), __FILE__, __LINE__, __func__, false)

#define set_errno(e, code, msg) \
    _set_errno((e), (code), (msg), __FILE__, __LINE__, __func__, false)

#define set_fatal(e, code, msg) \
    _set_err((e), (code), (msg), __FILE__, __LINE__, __func__, true)


// void _set_err(
//     err *e,
//     const char *file,
//     const char *msg,
//     const char *func,
//     int line,
//     int err_code,       // actual error code (errno?)
//     int exit_code,      // actual exit code (errno?)
//     int jlog_code,      // jlib's error code 
//     bool fatal
// );

// void _set_errno(
//     err *e,
//     const char *msg,
//     const char *file,
//     const char *func,
//     int line,
//     int err_code,       // actual error code
//     int exit_code,      // actual exit code
//     int jlog_code,      // jlib's error code 
//     bool fatal
// );

/* TODO; finish implementing the implementations ... */




#ifdef JLOGS_IMPL


#endif /* JLOGS_IMPL */


#endif /* JLOGS_H */
