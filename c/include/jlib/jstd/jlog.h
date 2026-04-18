#pragma once
#ifndef JLOG_H
#define JLOG_H

#include <stdbool.h>

#include "../jstd/jtime.h"
#include "../jdata/jvec.h"
#include "../jio/jfile.h"

typedef enum LogCode {
    LOG_MISC = 0,
    LOG_NULL,
    LOG_IO_IN,
    LOG_IO_OUT,
    LOG_MEM,
    LOG_API,
    LOG_VEC,
    LOG_ARGS,
    LOG_INIT,
    LOG_TIME,
    LOG_FILE,
    LOG_TYPE,
    LOG_LIST,
    LOG_ALGO,
    LOG_STATE,
    LOG_TRACE,
    /* 16 */
    
    LOG_COUNT = 64 /* max count {sizeof(uint64_t)} */
} logcode;

typedef uint64_t logmask_t;

#define LOGCODE_BIT(code) (1ULL << (code))
#define LOGCODE_CLEAR(mask) ((mask) = 0)
#define LOGCODE_ENABLE(mask, code) ((mask) |= LOGCODE_BIT(code))
#define LOGCODE_DISABLE(mask, code) ((mask) &= ~LOGCODE_BIT(code))
#define LOGCODE_ISSET(mask, code) (((mask) & LOGCODE_BIT(code)) != 0)
// #define LOGCODE_ALL ((1ULL << LOG_TRACE + 1) - 1ULL) // unsafe; no bounds check!
#define LOGCODE_ALL ((1ULL << LOG_COUNT) - 1ULL)

typedef enum LogLevel {
    LOGLVL_NONE = 0,
    LOGLVL_TRACE,
    LOGLVL_DEBUG,
    LOGLVL_INFO,
    LOGLVL_WARN,
    LOGLVL_ERROR,
    LOGLVL_PANIC,
    LOGLVL_FATAL,
    LOGLVL_COUNT
} loglvl;

typedef enum LogFlags {
    LOGFLAG_NONE        = 0,
    LOGFLAG_TIMESTAMP   = 1 << 0,
    LOGFLAG_LEVEL       = 1 << 1,
    LOGFLAG_CODE        = 1 << 2,
    LOGFLAG_SRC_FILE    = 1 << 3,
    LOGFLAG_SRC_LINE    = 1 << 4,
    LOGFLAG_SRC_FUNC    = 1 << 5,
    LOGFLAG_COLOR       = 1 << 6,
    LOGFLAG_STDOUT      = 1 << 7,
    LOGFLAG_STDERR      = 1 << 8,
    LOGFLAG_ABORT_FATAL = 1 << 9,
    LOGFLAG_ABORT_PANIC = 1 << 10
} logflags;

typedef enum LogFileFlags {
    /*
    #define LOGFILE_APPEND    0x01
    #define LOGFILE_AUTOMAKE  0x02
    #define LOGFILE_AUTOFLUSH 0x04
    #define LOGFILE_OWNPATH   0x08
    #define LOGFILE_OWNFILE   0x10
    #define LOGFILE_ROTATE    0x20
    */

    LOGFILE_NONE      = 0,
    LOGFILE_APPEND    = 1 << 0,
    LOGFILE_AUTOMAKE  = 1 << 1,
    LOGFILE_AUTOFLUSH = 1 << 2,
    LOGFILE_OWNPATH   = 1 << 3,
    LOGFILE_OWNFILE   = 1 << 4,
    LOGFILE_ROTATE    = 1 << 5
} logfile_f;

typedef struct LogFile {
    logfile_f flags;
    file_t file;   // has info, location, and handle of its own
    size_t size;
    uint8_t flags;
    bool isopen;
    bool flush;
    char *tmpbuf;  // mutable buffer to read/write for custom messaging
    size_t bufcap;
} logfile_t;


typedef struct LogEvent {
    logflags flags;
    logcode code;   // LOG_CODE
    loglvl lvl;     // LOG_LVL
    logmask_t mask;
    chrono_t time;    // custom jlib time type
    uint32_t line;
} logevent_t;

typedef struct Logger {
    logfile_t logfile;
    logevent_t event; 
    loglvl minlvl;
    error_t error;    // error info to write in log, contains msg
    char* tmpbuf;     // created based on error struct msg (+`\n`)
    size_t bufcap;
    uint32_t lastln;  // capture last line for meta reasons
} logger_t;

// static inline logger_t logger_log(logger_t *lg, LOGLVL_INFO, LOG_FILE, "opened '%s'", path);
// static inline logger_t logger_log(&lg, LOGLVL_INFO, LOG_FILE, "opened '%s'", path);
// logger_log(&lg, LOGLVL_DEBUG, LOG_MEM,  "allocated %zu bytes", n);

static inline logger_t logger_init(logevent_t ev);



// typedef struct {
//     Jerror err;       // jlib's error struct  
//     Jtime time;       // jlib's time struct
//     Jansi ansi;       // theme coloring etc
//     logger_t logger;  // internal logger
//     char *buffer;     // tmp buffer to build strings
// } Jlog;


// void print_log(const Jlog *log, bool err);
// void panic(Jlog *log);

#endif /* JLOG_H */
#define JLOG_IMPL // #debug-mode
#ifdef JLOG_IMPL
#include <stdio.h>

static inline logger_t logger_init(logevent_t ev){
    logger_t l;
    l.minlvl = LOGLVL_NONE;
    l.event.code = LOGCODE_BIT(LOG_INIT) | LOGCODE_BIT(LOG_MISC);
    l.bufcap = 1024;
    l.tmpbuf = "Hello, World!\n";
    l.lastln = 0;

    /*
    logevent_t ev = {
        .lvl  = LOGLVL_INFO,
        .code = LOG_FILE,
        .msg  = "opened config successfully"
    };

    if (ev.lvl < lg.minlvl)
    return;

    if (!LOGCODE_ISSET(lg.enabled_codes, ev.code))
        return;

    writelog();
    
    */
}

// void panic(Jlog *log){
//     if (!log) return;
//     log->err.err.die = true;
//     log->logger.event.lvl = LOGLVL_PANIC;
//     //log->err.err.errtime = init_t();
// }



// void print_log(const Jlog *log, bool err){
//     printf("");
// }

#endif /* JLOG_IMPL */

