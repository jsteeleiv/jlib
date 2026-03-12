#ifndef J_HUE_H
#define J_HUE_H

#include <stdio.h>

typedef enum {
    BLACK  = 30,
    RED    = 31,
    GREEN  = 32,
    YELLOW = 33,
    BLUE   = 34,
    PURPLE = 35,
    CYAN   = 36,
    WHITE  = 37
} color;

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