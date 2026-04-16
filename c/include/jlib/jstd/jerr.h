#pragma once
#ifndef JERR_H
#define JERR_H

#include "../jstd.h"
#include "../jtools.h"
#include "jtime.h"




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
    ERR_TYPE_UNKNOWN,

    ERRCODE_COUNT
} errcode_t;

// CREATE the bitmask like I did for the logcodes... with macros and all.
// simplify all of the error related functions, use error_t over Jerror, I will define Jerror stuff later , way later?
// clean up everything in the IMPL section to ensure no compiler errors ...


typedef struct Error {
    errcode_t errcode;   // error code
    tmstamp_t time;
    file_t path;         // location of error {__FILE__, __LINE__, __FUNC__}
    time_t rawtime;      // error struct initialization time (raw tm)
    const char *msg;     // msg to pass along (at least to the logger())
    int lives;           // if life == 0; die = true; (-1 = ♾️ )
    int line;            // number of line in question, in *file
    int exitcode;        // actual exit code -> propogated
    bool passerr;        // if ERROR or higher write to stderr
    bool die;            // should we die after some point?
} error_t;

#define err_ok(e) ((e)->errcode == ERR_SUCCESS)

typedef struct Fault {

} fault_t;

typedef struct Panic {

} panic_t;

typedef struct Exception {

} except_t;

typedef struct Signal {

} signal_t;

typedef struct Try {

} try_t;

typedef struct Catch {

} catch_t;

typedef struct Result {

} result_t;

/* try, catch, throw, raise, panic, finally 
    stack unwinding, 
    fault is lowlevel exception caused by current instruction
    trap is a synchronous transfer of control caused intentionally by code
    abort is a severe exception that usually cannot be meaningfully resumed.

*/

// typedef struct {
//     Jtime time;       // time object
//     Jansi ansi;          // ansi object (ascii, coloring, etc)
//     Jlist list;          // list of errors
//     Jlog log;
//     error_t err;
// } Jerror;


/* TODO: edit macros to fit new structures error_t vs. Jerror */
#define ERR_NEW( msg, err, exit) \
    err_init(__FILE__, __LINE__, __func__, (msg), (err), (exit))

#define ERR_SET(e, msg, err, exit, die) \
    err_set(__FILE__, __LINE__, __func__, (e), (msg), (err), (exit), (die))

static inline error_t err_init(const char *file, const char *func, int line, 
    const char *msg, int err, int exit
);

static inline void err_set(error_t *e, const char *msg, int err,
    int exit, bool die
);

static inline void err_reset(error_t *e, const char *opt, void *val, bool kill);
static inline void err_clear(error_t *e);
static inline void err_kill(error_t *e);


#endif /* JERR_H */
#define JERR_IMPL //#debug-mode
#ifdef JERR_IMPL
#include <stdio.h>
#include <errno.h>

static inline void err_clear(error_t *e){
    if (!e) return;
    e = NULL;
    e->path.loc.file = NULL;
    e->path.loc.line = NULL;
    e->path.loc.func = NULL;
    timestamp_refresh(&e->time);
}

static inline void err_reset(error_t *e, const char *opt, void *val, bool kill){
    if (!e) return;
    if (kill) err_kill(&e);

    err_clear(&e);
    err_set(&e, opt, ERR_SUCCESS, 0, false);


}

static inline void err_kill(error_t *e){
    e->lives = 0;
    e->die = true;
}

static inline error_t err_init(const char *file, const char *func, int line,
    const char *msg, int err, int exit){
    
    // initialize life
    error_t e = {0};
    e.lives = 9;
    e.die = false;
    //initialize time
    tmstamp_t t = timestamp_init();
    e.time = t;
    e.rawtime = t.rawtime;
    // initialize file location
    e.path.loc.file = file;
    e.path.loc.line = line;
    e.path.loc.func = func;
    e.errcode = err;
    e.exitcode = exit;
    // prepend prefix
    snprintf(e.msg, sizeof(e.msg), "[ERROR]: %s", msg ? msg : "message is null");
    return e;
}

static inline void err_set( error_t *e, const char *msg,
    int err, int exit, bool die){

    if(die && --e->lives == 0) err_kill(e);
    //refresh time struct
    timestamp_init(e->time);
    e->errcode = err;
    e->exitcode = exit;
    // prepend prefix to message
    snprintf(e->msg, sizeof(e->msg), "[ERROR]: %s", msg ? msg : "message is null");

    // TODO: Do I even need these? should the error stay with the file forever?
    // --> or should an error be able to "swtich context" on the fly?
    // e->path.loc.file = file;
    // e->path.loc.line = line;
    // e->path.loc.func = func;
}

#endif /* JERR_IMPL */
