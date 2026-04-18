#pragma once
#ifndef JARCH_H
#define JARCH_H

/* include sub-libraries */
#include "jarch/jmips.h"
#include "jarch/jriscv.h"
#include "jarch/jsys.h"
#include "jarch/jx64.h"
#include "jarch/jx86.h"

#include <stdbool.h>
#include <stdlib.h>

typedef enum Architecture {
    ARCH_UNKNOWN = 0,
    ARCH_WINDOWS,
    ARCH_LINUX,
    ARCH_MACOS,
    ARCH_FREEBSD,
    ARCH_OPENBSD,
    ARCH_DRAGONFLY,

} arch_t;

#if defined(_WIN32)
  #define ARCH_PLATFORM ARCH_WINDOWS
#elif defined(__linux__)
  #define ARCH_PLATFORM ARCH_LINUX
#elif defined(__APPLE__)
  #define ARCH_PLATFORM ARCH_MACOS  
#elif defined(__FreeBSD__)
  #define ARCH_PLATFORM ARCH_FREEBSD
#elif defined(__OpenBSD__)
  #define ARCH_PLATFORM ARCH_OPENBSD
#elif defined(__DragonFly__)
  #define ARCH_PLATFORM ARCH_DRAGONFLY
#else
  #define ARCH_PLATFORM ARCH_UNKNOWN
#endif



typedef enum ArchitectureCapabilities {
    /* capability flags
    -------------------
        (BITMASK)
        instead of scattering '#ifdef"...
        check capabilities

    */
    ARCH_CAP_NONE = 0,
    /* process & system */
    ARCH_CAP_FORK     = 1 << 0,
    ARCH_CAP_EXEC     = 1 << 1,
    ARCH_CAP_THREAD   = 1 << 2,
    /* IO models */  
    ARCH_CAP_EPOLL    = 1 << 3,
    ARCH_CAP_KQUEUE   = 1 << 4,
    ARCH_CAP_SELECT   = 1 << 5,
    /* terminal features */
    ARCH_CAP_IS_TTY   = 1 << 6,
    ARCH_CAP_RAWMOD   = 1 << 7,
    /* filesystem */
    ARCH_CAP_SYMKINKS = 1 << 8,

} archcaps_t;

#define ARCH_HAS(cap) (arch_has(cap))
#define ARCH_WIN (ARCH_PLATFORM == ARCH_WINDOWS)
#define ARCH_LIN (ARCH_PLATFORM == ARCH_LINUX)
#define ARCH_MAC (ARCH_PLATFORM == ARCH_MACOS)


typedef enum ArchitectureError {
    ARCH_OK = 0,
    ARCH_ERR_UNKNOWN,
    ARCH_ERR_UNSUPPORTED,
    ARCH_ERR_INVALID_ARG,
    ARCH_ERR_PERMISSIONS,
    ARCH_ERR_NOT_FOUND,
    ARCH_ERR_IO_ISSUES,
    ARCH_ERR_MEMORY
} archerr_t;

typedef struct ArchitectureTime{
    uint64_t ns; /* nanoseconds */
} archtime_t;

typedef struct ArchitectureInfo {
    archcaps_t caps;
    archerr_t err;
    archtime_t time;
} archinfo_t;

static inline archinfo_t arch_info_get(void);
static inline bool arch_has(archcaps_t cap);
static inline archerr_t arch_init(void);

typedef struct ArchitectureFile {
    
} archfile_t;

typedef struct ArchitectureProcess {
    archinfo_t info;
} archproc_t;

/* terminal info */
typedef struct TTYinfo {
    bool istty;
    bool raw_ok;
    bool ansi_ok;
    int rows, cols;
} ttyinfo_t;

#endif /* JARCH_H */
#define JARCH_IMPL // #debug-mode
#ifdef JARCH_IMPL

typedef struct ArchitectureResult {
    archerr_t err;
    void *val;
} archres_t;


#endif /* JARCH_IMPL */
