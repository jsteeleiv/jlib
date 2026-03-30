#ifndef JSTR_H
#define JSTR_H

#include <stdio.h>

typedef struct StringView {
    const char *str;
    //char *buffer;
    size_t size;
    size_t id;
} strview_t;

strview_t sv(const char *cstr);
void sv_chop_left(strview_t *sv,  size_t n);
void sv_trim_left(strview_t *sv);
void sv_chop_right(strview_t *sv, size_t n);
void sv_chop_right(strview_t *sv, size_t n);
void sv_trim(strview_t *sv);

#endif /* JSTR_H */
#define JSTR_IMPL
#ifdef JSTR_IMPL
#include <string.h>
#include <ctype.h>

strview_t sv(const char *cstr){
    return (StringView) {
        .str = cstr,
        .size = strlen(cstr),
    };
}


void sv_chop_left(strview_t *sv, size_t n){
     if (n > sv->size) n = sv->size;
     sv->size -= n;
     sv->str += n;
}

void sv_trim_left(strview_t *sv){
    while (sv->size > 0 && isspace(sv->str[0])){
        sv_chop_left(sv, 1);
    }
}

void sv_chop_right(strview_t *sv, size_t n){
    if (n > sv->size) n = sv->size;
    sv->size -= n;
}

void sv_trim_right(strview_t *sv){
    while (sv->size > 0 && isspace(sv->str[sv->size - 1])){
        sv_chop_right(sv, 1);
    }
}

void sv_trim(strview_t *sv){
    sv_trim_left(sv);
    sv_trim_right(sv);
}




#endif /* JSTR_IMPL */