#ifndef JDBG_H
#define JDBG_H

#include <jlib/jlib.h>

typedef enum EventKind {
    TRACE_ENTER, TRACE_LEAVE, TRACE_POINT,
    EVENT_LOG, EVENT_WARN, EVENT_ERROR, EVENT_ASSERT,
    EVENT_STATE_CHANGE

} ekind_t;

typedef struct Event {
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
    int level;
    uint16_t flags;
    const char *module;
    const char *ring_buf;
    size_t ring_cap;

} config_t;

typedef struct {
    Jerror errs;
    Jansi ansi;
    Jtime time;
    Jlist events;
    Jstack frames;
    config_t conf;

} Jdbg;


static void err_attach(Jerror *e);
static void err_log(Jlog *log);



#endif /* JDBG_H */
#ifdef JDBG_IMPL



#endif /* JDBG_IMPL */