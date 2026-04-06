#pragma once
#ifndef JLOG_H
#define JLOG_H

#include <jlib/jstd/jtime.h>
#include <jlib/jdata/jlist.h>
#include <jlib/jdata/jvec.h>

typedef enum LogLevel {
    LOGLVL_NONE = 0,
    LOGLVL_TRACE,
    LOGLVL_DEBUG,
    LOGLVL_INFO,
    LOGLVL_WARN,
    LOGLVL_ERROR,
    LOGLVL_PANIC,
    LOGLVL_FATAL
} loglvl_t;

typedef enum LogCode {
    LOG_NONE = 0,
    LOG_MISC,
    LOG_INIT,
    LOG_TIME,
    LOG_FILE,
    LOG_IO,
    LOG_ARGS,
    LOG_MEM,
    LOG_TYPE,
    LOG_LIST,
    LOG_VEC,
    LOG_ALGO,
    LOG_STATE,
    LOG_API,
    LOG_TRACE
} logcode_t;

typedef struct LogFile {
    FILE *handle;
    fileinfo_t info;
    size_t size;
    uint8_t flags;
    bool isopen;
    bool flush;
    char *name;
} logfile_t;

#define LOGFILE_FLAG_APPEND   0x01
#define LOGFILE_FLAG_AUMAKE   0x02
#define LOGFILE_FLAG_AUFLUSH  0x04
#define LOGFILE_FLAG_OWNPATH  0x08
#define LOGFILE_FLAG_OWNFILE  0x10
#define LOGFILE_FLAG_ROTATE   0x20

typedef struct Logger {
    loglvl_t lvl;     // priority level
    logcode_t code;   // LOG_CODE
    time_t rawtime;   // raw epoch timestamp
    vec_t buffer;     // tmp dynamic buffer
    logfile_t file;   // logfile type
    const char* msg;  // message to write to file (will `\n`'d)
    size_t size;      // size of message (length of characters)
    uint32_t source;  // last line of the file written to
} log_t;

typedef struct {
    Jerror errs;   // jlib's error struct (internal) 
    Jtime time;    // jlib's timestamp struct
    Jansi ansi;    // theme coloring etc...
    log_t log;    // custom logs, each with own time, file, msg, etc
    Jlist logs;
    const char *tmpbuf;        // tmp buffer to build strings
} Jlog;


void print_log(const Jlog *log, bool err);
void panic(Jlog *log);

#endif /* JLOG_H */
#ifdef JLOG_IMPL
#include <stdio.h>
#include <stdbool.h>


void panic(Jlog *log){
    if (!log) return;
    log->errs.die = true;
    log->errs.loglvl = PANIC;
    log->errs.errtime = init_t();
}



void print_log(const Jlog *log, bool err){
    printf("");
}

#endif /* JLOG_IMPL */

