#ifndef JDBG_H
#define JDBG_H

#include <jdata/jtype.h>


typedef enum EventKind {
    EVENT_LOG, EVENT_WARN, EVENT_ERROR, EVENT_ASSERT,
    EVENT_STATE_CHANGE

} ekind_t;

typedef enum TraceKind {
    TRACE_ENTER, TRACE_LEAVE, TRACE_POINT, TRACE_ERROR
} tkind_t;

typedef enum TraceFlag {
    SHOW_FILE   = 1 << 0,
    SHOW_LINE   = 1 << 1,
    SHOW_FUNC   = 1 << 2,
    SHOW_TIME   = 1 << 3,
    SHOW_ERROR  = 1 << 4,
    SHOW_DEPTH  = 1 << 5
} tflag_t;

typedef struct Event {
    tkind_t trace_kind;
    const char *name;
    const char *func;
    const char *file;
    const char *msg;
    struct tm time;
    uint64_t time_ns;
    ekind_t kind;
    int line;
    int level;
} event_t;

typedef struct Tracer {
    const char *func;
    const char *file;
    const char *name;    // module name
    uint64_t self_id;
    uint64_t parent_id;
    uint32_t flags;
    int line;
} trace_t;

typedef struct Frame {
    trace_t trace;
    struct tm time;
    size_t depth;
    const char *label;
    const char *args;
    uint32_t state;
} frame_t;    // frames get pushed onto the stack

typedef struct Configuration {
    //Jansi color;
    int level;
    uint16_t flags;
    const char *module;
    ringbuf_t *ringbuf;
    size_t ringbuf_cap;
    bool trace_enabled;
    bool timer_enabled;
} config_t;

typedef struct {
    const Jerror *lasterr;
    Jstack *ctxstk;
    Jstack frames;
    Jlist events;
    Jtime time;
    ringbuf_t *ringbuf;
    config_t conf;
    size_t trace_depth;
    size_t max_depth;
    void *sinks;
    void *stats;
    uint16_t trace_flags;

} Jdbg;


static void err_attach(Jerror *e);
static void err_log(Jlog *log);



#endif /* JDBG_H */
#ifdef JDBG_IMPL



#endif /* JDBG_IMPL */