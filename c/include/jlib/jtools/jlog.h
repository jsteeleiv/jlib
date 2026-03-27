#pragma once
#ifndef JLOG_H
#define JLOG_H

#include <jlib/jcore/jtime.h>
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

typedef struct Logger {
    loglvl_t lvl;             // priority level
    logcode_t code;           // LOG_CODE
    time_t rawtime;           // raw epoch timestamp
    vec_t buffer;             // tmp buffer
    const char* logfile;      // path to logfile
    const char* text;         // rendered line of text (will `\n`'d)
    size_t size;              // size of message (length of characters)
    uint32_t source;          // last line of the file written to
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

