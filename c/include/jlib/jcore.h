#ifndef JCORE_H
#define JCORE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <jlib/jlib.h>

// xor_swap, echo (or split later)
void echo(const char* format, ...);
void _args(const char *init_arg, ...);




typedef struct ByteArray {
    unsigned char *data;
    size_t length;
} bArray;

Jerror readfile_bArray(const char *path) {
    Jerror jerr;
    FILE *f;
    bArray arr;
    long file_sz;
    size_t bytes_read;
    unsigned char *tmpbuf;

    if (!path) {
        JERR("No filepath provided", ERROR, LOG_ARGS, NULL_ARG, -1, false);
        return jerr;
    }

    f = fopen(path, "rb");
    if (!f) {
        JERR("could not read file", ERROR, LOG_FILE, FILE_OPEN, -1, false);
        return jerr;
    }

    arr.data = NULL;
    arr.length = 0;

    // Read the file byte by byte and store it in the array
    unsigned char byte;
    while (fread(&byte, sizeof(byte), 1, f) == 1) {
        // Reallocate memory for the byte array and append the byte
        arr.data = (unsigned char*)realloc(arr.data, arr.length + 1);
        if (!arr.data) {
            JERR("Could not allocate memory", ERROR, LOG_MEM, MEM_NULL, -1, false);
            fclose(f);
            return jerr;
        }
        arr.data[arr.length] = byte;
        arr.length++;
    }

    fclose(f);

    // Print the bytes stored in the struct
    printf("Bytes read from file (%s):\n", path);
    for (size_t i = 0; i < arr.length; i++) {
        printf("%02X ", arr.data[i]);  // Print bytes as hexadecimal
    }
    printf("\n");

    // Free the allocated memory for the byte array
    free(arr.data);
    JERR("NONE", NONE, LOG_NONE, SUCCESS, 0, false);
    return jerr;

}


FILE *jopen(const char *path, const char *rwb, Jerror *jerr){
    FILE *f = fopen(path, rwb);
    if (!f) {
        SET_E(jerr, "file not found", WARN, FILE_ERR, -1, false);
    }
    SET_E(jerr, "set successful", NONE, SUCCESS, 0, false);
    return f;
}

void jclose(FILE *f, Jerror *jerr){
    if (!f) {
        SET_E(jerr, "file not found", WARN, FILE_ERR, -1, false);    
    }
    fclose(f);
    SET_E(jerr, "set successful", NONE, SUCCESS, 0, false);
}



#ifdef JCORE_IMPL


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



#endif /* JCORE_IMPL */

#endif /* JCORE_H */