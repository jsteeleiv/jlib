#ifndef JERR_H
#define JERR_H


#include <jtools/jlog.h>
#include <jtools/jansi.h>
#include <jdata/jvec.h>



typedef enum ErrorCode {
    ERR_SUCCESS = 0,

    /* general: 1-99 */
    ERR_UNKNOWN,
    ERR_NULL_ARG,
    ERR_BAD_STATE,
    ERR_UNSUPPORTED,

    /* memory: 100-199 */
    ERR_MEM_ALLOC = 100,
    ERR_MEM_FREE,
    ERR_MEM_NULL,
    ERR_MEM_BOUNDS,

    /* file: 200-299 */
    ERR_FILE_NULL = 200,
    ERR_FILE_OPEN,
    ERR_FILE_CLOSE,
    ERR_FILE_READ,
    ERR_FILE_WRITE,
    ERR_FILE_SEEK,
    ERR_FILE_TELL,
    ERR_FILE_FLUSH,
    ERR_FILE_EOF,
    //ERR_FILE_NOT_FOUND,
    ERR_FILE_PERM,
    ERR_FILE_EXISTS,
    ERR_FILE_INVALID,
    ERR_FILE_FORMAT,

    /* time: 300-399 */
    ERR_TIME_GENERIC = 300,
    ERR_TIME_INIT,
    ERR_TIME_CONV,

    /* type / generic value: 400-499 */
    ERR_TYPE_GENERIC = 400,
    ERR_TYPE_MISMATCH,
    ERR_TYPE_INVALID,
    ERR_TYPE_UNKNOWN
} errcode_t;

typedef struct Error {
    errcode_t errcode;   // error code
    time_t rawtime;      // error struct initialization time (raw tm)
    const char *file;    // filepath, if applicable
    const char *func;    // name of func on stack (caution: may be null)
    const char *msg;     // jlib's error message (string)
    int lives;           // if life == 0; die = true; (-1 = ♾️ )
    int line;            // number of line in question, in *file
    int exitcode;        // actual exit code -> propogated
    bool passerr;        // if ERROR or higher write to stderr
    bool die;            // should we die after some point?
} error_t;

#define err_ok(e) ((e)->errcode == ERR_SUCCESS)

typedef struct {
    Jtime time;       // time object
    Jansi ansi;          // ansi object (ascii, coloring, etc)
    Jlist list;          // list of errors
    Jlog log;
} Jerror;


/* TODO: edit macros to fit new structures error_t vs. Jerror */
#define INIT_E(lives, msg, loglvl, logcode, errcode, exit) \
    init_e(__FILE__, __LINE__, __func__, (lives), (msg), (loglvl), (logcode), (errcode), (exit))

#define SET_E(e, msg, loglvl, logcode, errcode, exit, die) \
    set_e(__FILE__, __LINE__, __func__, (e), (msg), (loglvl), (logcode), (errcode), (exit), (die))

Jerror init_e( 
    const char *file,
    char *func,
    const char *msg,
    int line,
    loglvl_t loglvl,
    logcode_t logcode,
    errcode_t errcode,
    int exitcode
);

Jerror set_e(
    Jerror *e,
    char *msg,
    int line,
    loglvl_t loglvl,
    logcode_t logcode,
    errcode_t errcode,
    int exitcode,
    bool die
);

Jerror reset(Jerror *e, const char *opt, void *val, bool kill);
void clear(Jerror *e);
void kill(Jerror *e);


#endif /* JERR_H */
#define JERR_IMPL
#ifdef JERR_IMPL
/* TODO; finish implementing the implementations ... */
// waiting to build Jany data type to handle generics; will change then
#include <stdio.h>
#include <errno.h>

// typedef struct Error {
//     errcode_t errcode;   // error code
//     time_t rawtime;      // error struct initialization time (raw tm)
//     const char *file;    // filepath, if applicable
//     const char *func;    // name of func on stack (caution: may be null)
//     const char *msg;     // jlib's error message (string)
//     int lives;           // if life == 0; die = true; (-1 = ♾️ )
//     int line;            // number of line in question, in *file
//     int exitcode;        // actual exit code -> propogated
//     logcode_t logcode;   // 
//     loglvl_t loglvl;     // 
//     bool passerr;        // if ERROR or higher write to stderr
//     bool die;            // should we die after some point?
// } error_t;




Jerror reset(Jerror *e, const char *opt, void *val, bool kill){
    // TODO: figure out Jany generic type ...;
}

void clear(Jerror *e){
    if (!e) return;
    e->errtime.tstr = {NULL};
    e->log.logs.list = {NULL};
    e->time. = getnow();
}

void kill(Jerror *e){
    e->life = 0;
    e->die = true;
}

Jerror init_e(int lives, const char *file, const char *func, const char *msg, int line,
    loglvl_t loglvl, logcode_t logcode, errcode_t errcode, int exitcode){
    
    // initialize life
    Jerror e = {0};
    e.life = lives;
    e.die = false;

    //initialize time
    Jtime t = init_t();
    e.errtime = t;
    e.init = t.raw;

    e.file = file;
    e.line = line;
    e.func = func;
    e.logcode = logcode;
    e.errcode = errcode;
    e.exitcode = exitcode;

    // prepend prefix
    snprintf(e.tmpbuf, sizeof(e.tmpbuf), 
        "[Jerror]: %s", msg ? msg : "message is null");

    if (e.loglvl >= ERROR){
        e.passerr = true;
    }

    e.msg = e.tmpbuf;
    return e;
}

/* TODO; make set_e functionality....*/
void set_e( Jerror *e, const char *file, const char *func, const char *msg,
    int line, Jloglvl loglvl, Jlogcode logcode, err_code errcode, int exitcode, bool die){
    
    // initialize error struct
    // if(die) e->life--;
    // if(e->life==0) kill(e);
    if(die && --e->life == 0) kill(e);
    //die ? e->--life==0 ? kill(e) :;

    //initialize time struct
    Jtime t = init_t();
    e.errtime = t;
    e.init = t.raw;

    // prepend prefix
    snprintf(e.tmpbuf, sizeof(e.tmpbuf), 
            "[Jerror]: %s", msg ? msg : "message is null");

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

#endif /* JERR_IMPL */


