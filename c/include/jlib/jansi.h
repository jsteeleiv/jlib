#ifndef JANSI_H
#define JANSI_H

#include <stdio.h>
#include <jlib/jlib.h>

#define JANSI_NONE 0

typedef enum Mode {
    M_NULL = 0,
    M_8BIT,
    M_256,
    M_RGB
} mode;

typedef enum Code {
    BLACK  = 30,
    RED    = 31,
    GREEN  = 32,
    YELLOW = 33,
    BLUE   = 34,
    PURPLE = 35,
    CYAN   = 36,
    WHITE  = 37,
    /* bright versions */
    B_BLACK  = 90,
    B_RED    = 91,
    B_GREEN  = 92,
    B_YELLOW = 93,
    B_BLUE   = 94,
    B_PURPLE = 95,
    B_CYAN   = 96,
    B_WHITE  = 97
} code;

typedef enum Attr {
    BOLD    = 1 << 0,
    DIM     = 1 << 1,
    ITALIC  = 1 << 2,
    UNDERLN = 1 << 3,
    INVERSE = 1 << 4
} attr;

typedef struct Color {
    mode mode;
    code code;
    // attr attr;
} color;

typedef struct Style {
    color fg;
    color bg;
    // attr attr;
    uint32_t fgval;
    uint32_t bgval;
    uint8_t fgmod;
    uint8_t bgmod;
    unsigned int attrs;
} style;

typedef struct Object {

} ansi;

Error printj(style fmt, const char *msg, ...);

static inline void jansi_reset(void);
static inline void jansi_fg(color color, int bold);

#ifdef JANSI_IMPL

/* 
functions that change text color
(arg == 0 (normal) ; arg == 1 (bold)
*/

static inline void jansi_reset(void){
    printf("\033[0;0m");
}

static inline void jansi_fg(color color, int bold) {
    if (bold)
        printf("\033[1;%dm", color);
    else
        printf("\033[0;%dm", color);
}

#endif /* JANSI_IMPL */
#endif /* JANSI_H */