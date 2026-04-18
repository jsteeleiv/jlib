#pragma once
#ifndef JTERM_H
#define JTERM_H

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <termios.h>

#include "jproc.h"
#include "jkeys.h"
#include "../jstd/jerr.h"
#include "../jtools/jansi.h"

#define TTY struct termios
struct termios term;
typedef void (*func)(void);

typedef enum TerminalSignals {
    TERM_SIGINT = 0,
    TERM_SIGTERM,
    TERM_SIGSEGV,

    TERM_CUSTOM,

    TERM_SIGS_COUNT = 16
} tsigs_t;

typedef uint16_t tsigflags;

/* KEYS & such 
--------------
    handle escape sequences (\x1b [ A)
    buffer reads, multi-byte input

*/
keyevent_t term_read_key(void);


typedef struct TerminalDimensions {
    const int height, width, depth;
} tdim_t;

typedef struct TerminalCoordinates {
    const int x, y, z;
} tcoords_t;

typedef struct TerminalStatus {
    tdim_t dims;
    tsigs_t sigs;
    tsigflags flags;
    tcoords_t coords;
    keyevent_t *kevents;
} tstatus_t;

typedef struct TerminalSize {
     int rows, cols;
} tsize_t;
/* Size detection
-----------------
    my custom tuplewoudl work well here as return value
    (TODO?)

    use: ioctl(TIOCGWINSZ)
*/
static inline tsize_t term_get_size(void);

typedef struct TerminalState {
    struct termios raw;
    char *msg;
    bool active;
    int pid;
    size_t pipes;
    size_t shells;
} tstate_t;

typedef struct TerminalBuffer {
    unsigned char data[64];
    size_t size;
} tbuf_t;

#define TERM_QSZ 128
typedef struct TerminalEventQueue {
    keyevent_t q[TERM_QSZ];
    int head, tail;
} teventq_t;

typedef struct Terminal {
    error_t *errors;
    tsize_t lifetime; /* time alive; likely in secs, even mins or hrs ? */
    tcoords_t coords;
    tstate_t state;
    chain_t chain;
    proc_t proc;
    size_t procs;
    pipe_t pipe;
} term_t;

static inline void term_init(tstate_t *s);

static inline void term_restore(void);
static inline void term_refresh(term_t *t);
/* Non-blocking/Timeout
-----------------------
    use: select() or poll()
    if (term_kbhit(100)){ keyevent_t ke = term_read_key()}
*/
static inline int term_kbhit(int ms);
/* guarantee full write (handle EINTR, partial writes) */
ssize_t term_write_all(int fd, const void *buf, size_t len);
/* cursor control */
static inline void term_curs_move(int row, int col);
static inline void term_curs_hide(void);
static inline void term_curs_show(void);
/* screen management */
static inline void term_clear(void);
static inline void term_clear_line(void);

/* colored output -> later --> 256 colors, truecolor ? */
static inline void term_set_color(ansi_t ansi);

/* Alternate Screen Buffer
--------------------------
    like vim, or top ...
    escape codes = {
        \x1b[?1049h
        \x1b[?1049l
    };
*/
static inline void term_screen_enter(void);
static inline void term_screen_exit(void);

/* Input mode stack
-------------------
    instead of just raw on/off
    lets you:
        temporarily change modes
        safely revert nested stated
*/
typedef struct ModeStack {
    struct termios stack[8];
    static int top;
    void *tmp;
} modestack_t;

/* store functino pointers, call in reverse; 
    like atexit() but SCOPED. and we like scope ... yesssss
*/
static inline void term_register_cleanup(void (*fn)(void));

/* Debug & such
---------------
    flags, modes, etc ...
*/
static inline void term_dbg_dump(void);

/* Escape sequence builder
--------------------------
    instead of hardcoding:
    term_write(term_escape("\x1b[%d;%dH", row, col));
*/
static inline char *term_escape(const char *fmt, ...);

/* Mini event-loop
------------------
    loop, poll, call handlers ...
    tiny terminal engine ???
*/
typedef void (*term_tick)(void);
typedef void (*term_key)(keyevent_t);
static inline void term_run(term_tick tick, term_key onkey);

// Function to enable raw mode for keyboard input
static inline void rawmode_on();
// Function to disable canonical mode and echoing
static inline void rawmode_off();

#endif /* JTERM_H */
#define JTERM_IMPL // #debug-mode
#ifdef JTERM_IMPL

static inline int term_getpid();

static inline void term_init(tstate_t *s){
    tcgetattr(STDIN_FILENO, &s->raw);
    s->active = 1;
    s->msg = "Hello, world!\n";
    s->pid = 0; /* need to term_getpid() for real value*/
    s->pipes = 0;
    if (NULL)/* try to init shell here */
        s->shells = 1;

    atexit(term_restore);
}

static inline void rawmode_on(bool run, func fn){
    tcgetattr(STDIN_FILENO, &term);
    if (run && fn){
        // atexit(disableRawMode); ncurses; used in jtui.h
        fn();
    }

    struct termios raw = term;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

static inline void rawmode_off(){
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
}

#endif /* JTERM_IMPL */
