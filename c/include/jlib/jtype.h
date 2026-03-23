#ifndef JTYPE_H
#define JTYPE_H

/* =========================
   PUBLIC API
   ========================= */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* types */
typedef enum Type {
    NONE = 0,
    I32,
    I64,
    F32,
    F64,
    BOOL
} Jtype;

typedef union Value {
    /* data */

} Jval;



typedef struct Any {
    
} Jany;



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