#pragma once
#ifndef JSTR_H
#define JSTR_H

#include <stdio.h>
#include <stdbool.h>

typedef struct CharacterInfo {
    char ch;
    size_t idx;
    bool alpha;
    bool digit;
    bool space;
    bool unset;
} charinfo_t;

// find info for a certain character by index
static inline charinfo_t charinfo_at(strview_t sv, size_t idx);

typedef struct StringView {
    const char *data;
    size_t size;
    char *tmp; // optional internal buffer
} strview_t;

// used to spread fields out before sending to printf()
#define STR_FMT "%.*s"
// printf(STR_FMT "\n", STR_ARG(s));
#define STR_ARG(s) (s).size, (s).data

static inline strview_t sv(const char *cstr);
static inline void sv_chop_left(strview_t *sv,  size_t n);
static inline void sv_trim_left(strview_t *sv);
static inline void sv_chop_right(strview_t *sv, size_t n);
static inline void sv_trim_right(strview_t *sv);
static inline void sv_trim(strview_t *sv);
static inline strview_t sv_chop_by_delim(strview_t *sv, char delim);

typedef struct CharacterIterator {
    charinfo_t info;
    strview_t *parent;
    size_t index;
} chariter_t;

typedef struct CharacterList {
    chariter_t iter;
    char *data;
    size_t len;
    size_t cap;
} charlist_t;

static inline bool charlist_init(charlist_t *list, size_t max);
static inline bool charlist_push(charlist_t *list, char c);
static inline void charlist_free(charlist_t *list);

typedef struct String {
    charlist_t chars;
    strview_t str;
    void *addr;
} string_t;

static inline void str_toupper_inplace(char *s);

#endif /* JSTR_H */
// #define JSTR_IMPL // #debug-mode
#ifdef JSTR_IMPL

#include <string.h>
#include <ctype.h> // must cast everything as (unsigned char)
#include <stdlib.h>

/* charinfo */
static inline charinfo_t charinfo_at(strview_t sv, size_t idx){
    char c = (idx < sv.size) ? sv.data[idx] : '\0';
    
    return (charinfo_t){
        .ch    = c,
        .idx   = idx,
        .alpha = isalpha((unsigned char)c) ? true : false,
        .digit = isdigit((unsigned char)c) ? true : false,
        .space = isspace((unsigned char)c) ? true : false,
        .unset = (c == '\0')
    };
}

/* charlist */
static inline bool charlist_init(charlist_t *list, size_t max){
    if (max <= list->cap) return true;

    void *tmp = realloc(list->data, max * sizeof(char));
    if (!tmp) return false;

    list->data = (char*)tmp;
    list->cap = max;
    
    return true;
}

static inline bool charlist_push(charlist_t *list, char c){
    if (list->len == list->cap){
        size_t newcap = (list->cap == 0) ? 8 : list->cap * 2;
        if (!charlist_init(list, newcap)) return false;
    }
    list->data[list->len++] = c;
    return true;
}

static inline void charlist_free(charlist_t *list){
    free(list->data);
    list->data = NULL;
    list->len = 0;
    list->cap = 0;
}

/* string view */
static inline strview_t sv(const char *cstr){
    return (strview_t) {
        .data = cstr,
        .size = strlen(cstr),
    };
}

static inline void sv_chop_left(strview_t *sv, size_t n){
     if (n > sv->size) n = sv->size;
     sv->size -= n;
     sv->data += n;
}

static inline void sv_trim_left(strview_t *sv){
    while (sv->size > 0 && isspace((unsigned char)sv->data[0])){
        sv_chop_left(sv, 1);
    }
}

static inline void sv_chop_right(strview_t *sv, size_t n){
    if (n > sv->size) n = sv->size;
    sv->size -= n;
}

static inline void sv_trim_right(strview_t *sv){
    while (sv->size > 0 && isspace((unsigned char)sv->data[sv->size - 1])){
        sv_chop_right(sv, 1);
    }
}

static inline void sv_trim(strview_t *sv){
    sv_trim_left(sv);
    sv_trim_right(sv);
}

static inline strview_t sv_chop_by_delim(strview_t *sv, char delim){
    size_t i = 0;
    while (i < sv->size && sv->data[i] != delim) i++;

    if (i < sv->size){
        strview_t result = {
            .data = sv->data,
            .size = i,
            .buffer = NULL
        };
        sv_chop_left(sv, i + 1);
        return result;
    }

    strview_t result = *sv;
    sv_chop_left(sv, sv->size);
    return result;
}

static inline void str_toupper_inplace(char *s){
    for (; *s; ++s) { *s = (char)toupper((unsigned char)*s); }
}

#endif /* JSTR_IMPL */
