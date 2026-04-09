#ifndef JDBG_H
#define JDBG_H

#include "../jtype/jval.h"
#include "../jio/jfile.h"
#include "../jdata/jstk.h"
#include "../jio/jbuf.h"
#include "../jstd/jtime.h"

typedef enum TraceKind {
    TRACE_ENTER, TRACE_LEAVE, TRACE_POINT, TRACE_ERROR
} tracekind_t;

typedef enum TraceFlags {
    SHOW_FILE   = 1 << 0, SHOW_LINE   = 1 << 1,
    SHOW_FUNC   = 1 << 2, SHOW_TIME   = 1 << 3,
    SHOW_ERROR  = 1 << 4, SHOW_DEPTH  = 1 << 5
} traceflags_t;

typedef struct Trace {
    chrono_t time;
    //fileloc_t fileloc;
    traceflags_t flags;
    tracekind_t kind;
    uint64_t selfid;
    uint64_t parentid;
    size_t maxdepth;
    size_t line;
    size_t depth;
} trace_t;

typedef enum EventKind {
    EVENT_LOG, EVENT_WARN, EVENT_ERROR, EVENT_ASSERT,
    EVENT_UNKNOWN, EVENT_PANIC, EVENT_STATE_CHANGED
} eventkind_t;

typedef struct Event {
    eventkind_t kind;
    error_t error;
    trace_t trace;
    const char *msg;
} event_t;

typedef struct Frame {
    const error_t lasterr;
    ringbuf_t *bufarray;  // will be making this the buffer of events ???
    size_t ringbuf_cap;
    event_t event;
    const char *label;
    const char *msg;
    uint32_t flags;
} frame_t;               // frames get pushed onto the stack ?

typedef struct DebugConfig {
    int level;
    const char *args;
    const char *module;
    bool trace_enabled;
    bool timer_enabled;
} dbgconf_t;

typedef struct DebugContext {
    dbgconf_t config;
    frame_t *frames;
    logger_t logger;
} dbgctx_t;


typedef struct {
    const dbgctx_t context;
    void *sinks;
    void *stats;
} Jdbg;


// static void dbg_err_attach(Jerror *e);
// static void dbg_err_log(Jlog *log);



#endif /* JDBG_H */
// #define JDBG_IMPL // #debug-mode, will remove ...
#ifdef JDBG_IMPL



#endif /* JDBG_IMPL */