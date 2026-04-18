#pragma once
#ifndef JPROC_H
#define JPROC_H

#ifndef _WIN32
#    define _POSIX_C_SOURCE 200809L
#    include <sys/types.h>
#    include <sys/wait.h>
#    include <sys/stat.h>
#    include <unistd.h>
typedef pid_t Pid;
typedef int Fd;
#else
#    define WIN32_MEAN_AND_LEAN
#    include "windows.h"
#    include <process.h>
#    define PATH_SEP "\\"
typedef HANDLE Pid;
typedef HANDLE Fd;
#endif

#define JSTR_IMPL
#include "../jtype/jstr.h"
#include "../jstd/jarch.h"

typedef struct Command {
    cstrarr_t line;
    pipe_t pipe;
} cmd_t;

cstr_t cmd_show(cmd_t cmd);
Pid cmd_run_async(cmd_t cmd, Fd *fdin, Fd *fdout);
void cmd_run_sync(cmd_t cmd);

typedef struct CommandArray {
    cmd_t *items;
    size_t count;
} cmdarr_t;

#define CMD(...) \
    do { \
        cmd_t cmd = { \
            .line = cstr_array_make(__VA_ARGS__, NULL) \
        }; \
        INFO("CMD: %s", cmd_show(cmd)); \
        cmd_sync(cmd); \
    } while (0)

void pid_wait(Pid pid);


typedef enum ChainTokenKind {
    CHAIN_TOK_END = 0,
    CHAIN_TOK_IN,
    CHAIN_TOK_OUT,
    CHAIN_TOK_CMD
} tokkind_e;

typedef struct ChainToken {
    tokkind_e kind;
    cstrarr_t args;
} chaintok_t;

#define TOKIN(path) \
    (chaintok_t) {  \
        .kind = CHAIN_TOK_IN, \
        .args = cstr_array_make(path, NULL) \
}
#define OUT(path) \
    (chaintok_t) { \
        .type = CHAIN_TOK_OUT, \
        .args = cstr_array_make(path, NULL) \
}
#define CHAIN_CMD(...) \
    (chaintok_t) { \
        .type = CHAIN_TOK_CMD, \
        .args = cstr_array_make(__VA_ARGS__, NULL) \
}

typedef struct Chain {
    cstr_t file_in;
    cstr_t file_out;
    cmdarr_t cmds;
} chain_t;

chain_t chain_from_tokens(chaintok_t first, ...);
void chain_sync(chain_t chain);
void chain_echo(chain_t chain);

#define CHAIN(...) \
    do { \
        chain_t chain = chain_from_tokens(__VA_ARGS__, (Chain_Token) {0}); \
        chain_echo(chain); \
        chain_sync(chain); \
    } while(0)

typedef struct Pipe {
    Pid pid;
    int writ;
    int exec;
} pipe_t;
/* int	 pipe(int [2]); */
pipe_t pipe_make(void);

typedef struct Process {

} proc_t;


#endif /* JPROC_H */
#define JPROC_IMPL // #debug-mode
#ifdef JPROC_IMPL



#endif /* JPROC_IMPL */
