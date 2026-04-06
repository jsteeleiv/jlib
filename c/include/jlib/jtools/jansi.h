#pragma once
#ifndef JANSI_H
#define JANSI_H

//#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <jlib/jstd/jerr.h>
#include <jlib/jparse/jargs.h>

#define ANSI_NONE 0

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
    attr attr;
} color_t;

typedef struct Style {
    color_t fg;
    color_t bg;
    uint32_t fgval;
    uint32_t bgval;
    uint8_t fgmod;
    uint8_t bgmod;
    unsigned int attrs;
} style_t;

typedef struct Jansi {
    color_t color;
    style_t style;
} ansi_t;

typedef struct {
    Jerror errs;
    ansi_t ansi;
} Jansi;

/* ASCII */

typedef struct AsciiMirror {
    char was;
    char is;
} mirror_t;

static const mirror_t ascii_map[] = {
    {'(', ')'},
    {')', '('},
    {'[', ']'},
    {']', '['},
    {'{', '}'},
    {'}', '{'},
    {'<', '>'},
    {'>', '<'},
    {'/', '\\'},
    {'\\', '/'},
};

typedef struct Ascii {
    char *buffer;
    char **lines;
    size_t length;
    size_t width;
    size_t height;
    bool owns;
} ascii_t;

typedef struct {
    Jerror errs;
    ansi_t ansi;
    ascii_t ascii;
} Jascii;



static inline Jerror ansi_print(style_t fmt, const char *msg, Jarg args);
static inline void ansi_reset(void);
static inline void ansi_fg(color_t color, int bold);

/* ASCII */
/* ascii lifecycle */
ascii_t *ascii_from_text(const char *text);
ascii_t *ascii_clone(const ascii_t *src);
void ascii_destroy(ascii_t *a);

/* ascii transforms */
static ascii_t *ascii_flip_horz(const ascii_t *src);
static ascii_t *ascii_flip_vert(const ascii_t *src);

/* ascii output */
void ascii_print(const ascii_t *a);
char *ascii_to_string(const ascii_t *a);

/* jascii lifecycle */
Jascii *jascii_new(const char *text);
void jascii_die(Jascii *j);

/* jascii transforms */
Jascii *jascii_flip_horz(const Jascii *j);
Jascii *jascii_flip_vert(const Jascii *j);

/* jascii output */
void jascii_print(const Jascii *j);

#endif /* JANSI_H */
#ifdef JANSI_IMPL

/* 
functions that change text color
(arg == 0 (normal) ; arg == 1 (bold)
*/

static inline Jerror ansi_print(style_t fmt, const char *msg, args_t args){

}

static inline void jansi_reset(void){
    printf("\033[0;0m");
}

static inline void jansi_fg(color color, int bold) {
    if (bold)
        printf("\033[1;%dm", color);
    else
        printf("\033[0;%dm", color);
}



static char ansi_map_char(char c){
    c = ascii_map[c]
}


/* ASCII */
static void ascii_die(ascii_t *a){
    if (!a) return;
    if (a->buffer) {
        free(a-<buffer);
    } else if (a->lines) {
        // free manually allocated lines
        for (size_t i = 0; i < a->length; i++){
            free(a->lines[i]);
        }
    }
    free(a->lines);
    free(a);
}


static ascii_t *ascii_from_text(const char *text){
    if (!text) return NULL;
    ascii_t *a = (ascii_t *)malloc(sizeof(ascii_t));
    if (!a) return NULL;

    a->buffer = strdup(text);
    if (!a->buffer) {
        free(a);
        return NULL;
    }

    // Count lines
    size_t lines = 1;
    for (const char *p = text; *p; p++) {
        if (*p == '\n') lines++;
    }

    a->lines = (char **)malloc(sizeof(char *) * lines);
    if (!a->lines) {
        free(a->buffer);
        free(a);
        return NULL;
    }

    // Split lines
    size_t idx = 0;
    char *start = a->buffer;

    for (char *p = a->buffer; ; p++) {
        if (*p == '\n' || *p == '\0') {
            *p = '\0';
            a->lines[idx++] = start;
            start = p + 1;
        }
        if (*p == '\0') break;
    }

    a->line_count = idx;
    a->height     = idx;

    // Compute max width
    size_t max_width = 0;
    for (size_t i = 0; i < idx; i++) {
        size_t len = strlen(a->lines[i]);
        if (len > max_width) max_width = len;
    }

    a->width = max_width;

    return a;

}




#endif /* JANSI_IMPL */