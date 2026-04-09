#pragma once
#ifndef JECHO_H
#define JECHO_H

#include <stdarg.h>

// xor_swap, echo (or split later)
void echo(const char* format, ...);

#endif /* JECHO_H */

#ifdef JECHO_IMPL


// function that takes a string with underscores 
// and replaces them with variables using printf in the background.
void echo(const char* format, ...) {
    va_list args;
    va_start(args, format);

    while (*format) {
        if (*format == '_') {
            int value = va_arg(args, int);
            printf("%d", value);
        } else {
            putchar(*format);
        }
        format++;
    }

    va_end(args);
}

// handle my function args, by simply passing them through to avoid stdargs conflict???
/* void _args(const char *init_arg, ...) {
	va_list args;
	va_start(args, init_arg);
	
	
} */



#endif /* JECHO_IMPL */