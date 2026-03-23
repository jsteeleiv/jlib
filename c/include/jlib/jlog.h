#ifndef JLOG_H
#define JLOG_H

// error/log structs
//#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <jlib/jerr.h>
#include <jlib/jtime.h>

typedef enum LogLevel {
    NONE = 0,
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    PANIC,
    FATAL
} Jloglvl;

typedef enum LogCode {
    LOG_NONE = 0,
    LOG_CORE,
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
} Jlogcode;

typedef struct Logger {
    Jerror jerr;              // jlib's error struct (internal) 
    Jtime time;               // jlib's timestamp struct
    Jloglvl loglvl;           // priority level
    Jlogcode logcode;         // LOG_CODE
    const char tmpbuf[1024];  // tmp buffer
    const char* file;         // path to logfile
    const char* text;         // rendered line of text (will `\n`'d)
} Jlog;


void print_log(const Jlog *log, bool err);
void panic(Jlog *log);

void panic(Jlog *log){
    if (!log) return;
    log->jerr.die = true;
    log->jerr.loglvl = PANIC;
    log->jerr.errtime = init_t();
}



void print_log(const Jlog *log, bool err){
    printf("");
}



#ifdef JLOG_IMPL


#endif /* JLOG_IMPL */


#endif /* JLOG_H */
