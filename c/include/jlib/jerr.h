#ifndef JERR_H
#define JERR_H

#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <jlib/jtime.h>

typedef enum ErrorCode {
    SUCCESS = 0,

    /* general: 1-99 */
    UNKNOWN,
    BAD_ARG,
    NULL_ARG,
    BAD_STATE,
    UNSUPPORTED,

    /* memory: 100-199 */
    MEM_ALLOC = 100,
    MEM_FREE,
    MEM_NULL,
    MEM_BOUNDS,

    /* file: 200-299 */
    FILE_ERR = 200,
    FILE_OPEN,
    FILE_CLOSE,
    FILE_READ,
    FILE_WRITE,
    FILE_SEEK,
    FILE_TELL,
    FILE_FLUSH,
    FILE_EOF,
    FILE_NOT_FOUND,
    FILE_PERM,
    FILE_EXISTS,
    FILE_INVALID,
    FILE_FORMAT,

    /* time: 300-399 */
    TIME_ERR = 300,
    TIME_INIT,
    TIME_CONV,

    /* type / generic value: 400-499 */
    TYPE_ERR = 400,
    TYPE_MISMATCH,
    TYPE_INVALID,
    TYPE_UNKNOWN
} Jerrcode;



typedef struct Error {
    Jerrcode errcode;       // error code
    Jtime errtime;     // time of most recent error/exception
    struct tm init;      // time object is initialized
    const char *file;   // filepath, if applicable
    const char *func;   // name of func on stack (may be null)
    const char *msg;    // jlib's error message (string)
    char *tmpbuf;    // tmp buffer to build strings
    int life;           // if life == 0; die = true; (-1 = ♾️ )
    int line;           // number of line in question, in *file
    int exitcode;      // actual exit code? -> propogated
    int logcode;      // jlib's error code -> stored
    int loglvl;      // jlib's error code -> stored
    bool _stderr;      // if ERROR or higher write to stderr
    bool die;         // should we die after some point?
} Jerror;

#define INIT_E(msg, loglvl, logcode, errcode, exit, die) \
    init_e(__FILE__, __LINE__, __func__, (msg), (loglvl), (logcode), (errcode), (exit), (die))

    #define SET_E(e, msg, loglvl, logcode, errcode, exit, die) \
    set_e(__FILE__, __LINE__, __func__, (e), (msg), (loglvl), (logcode), (errcode), (exit), (die))

Jerror init_e( 
    const char *file,
    const char *func,
    const char *msg,
    int line,
    Jloglvl loglvl,
    Jlogcode logcode,
    Jerrcode errcode,
    int exitcode,
    bool die
);

Jerror set_e(
    Jerror *e,
    //const char *file,
    //const char *func,
    const char *msg,
    //int line,
    Jloglvl loglvl,
    Jlogcode logcode,
    Jerrcode errcode,
    int exitcode,
    bool die
);

// wating to build Jany data type to handle generics; will change then
Jerror reset(Jerror *e, const char *opt, void *val, bool kill);
void clear(Jerror *e);
void kill(Jerror *e);

Jerror reset(Jerror *e, const char *opt, void *val, bool kill){
    // TODO: figure out Jany generic type ...;
}

void clear(Jerror *e){
    if (!e) return;
    *e = (Jerror){0};
}

void kill(Jerror *e){
    e->life = 0;
    e->die = true;
}

Jerror init_e( const char *file, const char *func, const char *msg,
    int line, Jloglvl loglvl, Jlogcode logcode, Jerrcode errcode, int exitcode, bool die){
    
    // initialize error struct
    Jerror e = {0};
    e.life = -1;

    //initialize time struct
    Jtime t = init_t();
    e.errtime = t;
    e.init = t.raw;

    // prepend prefix
    snprintf(e.tmpbuf, sizeof(e.tmpbuf), 
            "[JERR]: %s", msg ? msg : "message is null");

    if (e.loglvl >= ERROR){
        e._stderr = true;
    }

    e.file = file;
    e.line = line;
    e.func = func;
    e.msg = e.tmpbuf;
    e.logcode = logcode;
    e.errcode = errcode;
    e.exitcode = exitcode;
    e.die = false;
}


/* Coinvenience MACROS */
#define set_jerr(e, code, msg) \
    _set_jerr((e), (code), (msg), __FILE__, __LINE__, __func__, false)

#define get_errno(e, code, msg) \
    _set_errno((e), (code), (msg), __FILE__, __LINE__, __func__, false)

#define set_fatal(e, code, msg) \
    _set_err((e), (code), (msg), __FILE__, __LINE__, __func__, true)

// void _set_err(
//     Jerror *e,
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
//     Jerror *e,
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



#ifdef JERR_IMPL

#endif /* JERR_IMPL */


#endif /* JERR_H */