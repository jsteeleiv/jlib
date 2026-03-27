#ifndef JTYPE_H
#define JTYPE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <jlib/jlib.h>

typedef struct Oracle {
    Jerror errs;
    Jtime time;
    Jmap constellation;
    bool omen;
} bool_t;


typedef struct Random{
   Jerror jerr;
   Jtime time;
   Jstack stack;
   Jansi ansi;
} rand_t;

typedef struct Value {
    /* data */

} val_t;

typedef struct Coordinate {

} coord_t;

/* types */
typedef enum Kind {
    NONE = 0,
    /* basic */
    BOOL,
    /* signed integers */
    I8, I16, I32, I64,
    /* unsigned integers */
    U8, U16, U32, U64,
    /* floating point */
    F32, F64,
    /* string types */
    CSTR, STR,
    /* generic pointers */
    PTR, CONST_PTR,
    /* advanced */
    STRUCT, ARRAY, FUNC, 
    /* custom types */
    ANY, VALUE, XPRESSN,
    /* fallback */
    OPAQUE_T, NULL_T
} kind_t;

typedef struct Any {
    kind_t kind;
    union {
        bool boolean;
        int8_t i8;
        int16_t i16;
        int32_t i32;
        int64_t i64;
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
        uint64_t u64;
        float f32;
        double f64;
        const char *cstr;
        const void *cptr;
        char *str;
        void *ptr;
    } as; // <-- https://www.youtube.com/@TsodingDaily
} any; 

typedef struct RingBuffer {
    Jtype *data;
    size_t head;
    size_t tail;
    size_t cap;
    size_t len;
} ringbuf_t;

typedef struct Information {
    const char *name;
    uintptr_t addr;
    size_t size;
    kind_t kind;
} info_t;

typedef struct Anything {
    kind_t kind;
    info_t info;
    size_t size;
    void *data;
    uint32_t flags;
    any type;
} any_t;

typedef struct {
    Jerror errs;
    Jlist list;     // doubly linked list for casting ...
    info_t info;    // type info about the `value`
    any_t cast;
    any_t val;
} Jtype;

/* function declarations */
void JTYPE_example(void);




#endif /* JTYPE_H */


/* =========================
   IMPLEMENTATION
   ========================= */

#ifdef JTYPE_IMPL

#include <stdlib.h>

/* function definitions */
void JTYPE_example(void)
{
    /* implementation */
}

#endif /* JTYPE_IMPL */