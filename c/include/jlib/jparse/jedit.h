#pragma once
#ifndef JEDIT_H
#define JEDIT_H


/* Text editor
------------------------------
    inspired by the 'ed' editor
    
*/


typedef enum GlobalFlags {
    GLOBAL = 0x01,
} g_flag;

#include <stdio.h>
#include <stdbool.h>

typedef struct EditorBuffer {
    static int curr_addr;  // current address in editor buffer
    static int last_addr;  // last address in editor buffer
    static int next_addr;  // next address to care about ...
    static bool isbinary;  // if set -> buffer contains ASCII NULLs
    static bool modified;  // if set -> buffer changed since last write
    static bool newline;   // if set -> newline will be appended to file
    static bool seekwrite; // seek before writing
    static FILE *sfptr;    // scratch file pointer
    static long sfpos; 
} editbuf_t;


#endif /* JEDIT_H */

#ifdef JEDIT_IMPL



#endif /* JEDIT_IMPL */