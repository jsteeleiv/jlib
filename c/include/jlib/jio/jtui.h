#pragma once
#ifndef JTUI_H
#define JTUI_H

#include <ncurses/ncurses.h>

#define JERR_IMPL
#include "../jstd/jerr.h"
#include "jterm.h"
#include "jproc.h"

#define DEFAULT_COLOR 1


/* primitive wrapper around ncurses start */
static error_t tui_start();

#endif /* JTUI_H */
#define JTUI_IMPL // #debug-mode
#ifdef JTUI_IMPL

static error_t tui_start(){
    error_t error;
    initscr();
    raw();
    keypad(stdscr, TRUE);
    start_color();
    noecho();
    
    // Define color pairs
    init_pair(DEFAULT_COLOR, COLOR_BLACK, COLOR_WHITE);

    // Function to disable canonical mode and echoing
    rawmode_off();
    // Function to enable raw mode for keyboard input
    rawmode_on(true, (func)atexit(rawmode_off));

    endwin();
    return error;
}


#endif /* JTUI_IMPL */
