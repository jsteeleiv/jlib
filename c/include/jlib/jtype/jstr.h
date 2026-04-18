#pragma once
#ifndef JSTR_H
#define JSTR_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

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

typedef struct CString {
    const char *str;
} cstr_t;

typedef const char * Cstr;

typedef struct CstrArray {
    Cstr *items;
    size_t count;
} CstrArr;

static inline int cstr_ends_with(cstr_t cstr, cstr_t postfix);
#define ENDS_WITH(cstr, postfix) cstr_ends_with(cstr, postfix)

static inline cstr_t cstr_no_ext(cstr_t path);
#define NOEXT(path) cstr_no_ext(path)

typedef struct CstringArray {
    cstr_t *items;
    size_t count;
} cstrarr_t;

static inline CstrArr cstr_array_make(Cstr first, ...);
static inline CstrArr cstr_array_append(CstrArr cstrs, Cstr cstr);
static inline Cstr cstr_array_join(Cstr sep, CstrArr cstrs);

#define JOIN(sep, ...) cstr_array_join(sep, cstr_array_make(__VA_ARGS__, NULL))
#define CONCAT(...) JOIN("", __VA_ARGS__)
#define PATH(...) JOIN(PATH_SEP, __VA_ARGS__)

typedef struct String {
    charlist_t chars;
    strview_t str;
    void *addr;
} string_t;

static inline void str_toupper_inplace(char *s);

static inline int str_casecmp(const char *a, const char *b);
static inline int str_ncasecmp(const char *a, const char *b, size_t n);

#endif /* JSTR_H */
#define JSTR_IMPL // #debug-mode
#ifdef JSTR_IMPL

#include <string.h>
#include <ctype.h> // must cast everything as (unsigned char)
#include <stdlib.h>
#include <stddef.h>

/* cstring stuff */
static inline CstrArr cstr_array_append(CstrArr cstrs, Cstr cstr){
    CstrArr ret = {
        .count = cstrs.count + 1
    };
    ret.items = malloc(size0f(ret.items[0]) * ret.count);
    memcpy(ret.items, cstrs.items, cstrs.count * sizeof(ret.items[0]));
    ret.items[cstrs.count] = cstr;
    return ret;
}

static inline int cstr_ends_with(cstr_t cstr, cstr_t postfix){
    const size_t cstr_len = strlen(cstr.str);
    const size_t post_len = strlen(postfix.str);
    return post_len <= cstr_len &&
        strcmp(cstr.str + cstr_len - post_len, postfix.str) == 0;
}

static inline cstr_t cstr_no_ext(cstr_t path){
    size_t n = strlen(path.str);
    while (n > 0 && path.str[n-1] != '.') n -= 1;
    if (n > 0){
        char *str = malloc(n);
        memcpy(str, path.str, n);
        str[n-1] = '\0';
        cstr_t ret;
        ret.str = str;
        return ret;
    } else { return path; }
}

static inline CstrArr cstr_array_make(Cstr item, ...){
    CstrArr result = {0};
    if (!item) return result;
    result.count++;

    va_list args;
    va_start(args, item);
    for (Cstr next = va_arg(args, Cstr); next != NULL;
        next = va_arg(args, Cstr)
    ){ result.count++; }

    va_end(args);

    result.items = malloc(sizeof(result.items[0]) * result.count);
    if (!result.items) PANIC("could not allocate memory: %s",
        strerror(errno));

    result.count = 0;
    result.items[result.count++] = item;
    va_start(args, item);
    for (Cstr next = va_arg(args, Cstr); next != NULL;
        next = va_arg(args, Cstr)
    ){ result.items[result.count++] = next; }

    va_end(args);
    return result;
}

static inline Cstr cstr_array_join(Cstr sep, CstrArr cstrs){
    if (cstrs.count == 0) return "";

    const size_t sep_len = strlen(sep);
    size_t len = 0;
    for (size_t i = 0; i < cstrs.count; ++i) {
        len += strlen(cstrs.items[i]);
    }

    const size_t result_len = (cstrs.count - 1) * sep_len + len + 1;
    char *result = malloc(sizeof(char) * result_len);
    if (result == NULL) {
        PANIC("could not allocate memory: %s", strerror(errno));
    }

    len = 0;
    for (size_t i = 0; i < cstrs.count; ++i) {
        if (i > 0) {
            memcpy(result + len, sep, sep_len);
            len += sep_len;
        }

        size_t item_len = strlen(cstrs.items[i]);
        memcpy(result + len, cstrs.items[i], item_len);
        len += item_len;
    }
    result[len] = '\0';

    return result;
}

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
            .tmp = NULL
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

static inline int str_casecmp(const char *a, const char *b) {
    unsigned char ca, cb;
    if (!a && !b) return 0;
    if (!a) return -1;
    if (!b) return 1;

    while (*a && *b) {
        ca = (unsigned char)tolower((unsigned char)*a);
        cb = (unsigned char)tolower((unsigned char)*b);
        if (ca != cb) return (int)ca - (int)cb;
        a++; b++;
    }
    ca = (unsigned char)tolower((unsigned char)*a);
    cb = (unsigned char)tolower((unsigned char)*b);
    return (int)ca - (int)cb;
}

static inline int str_ncasecmp(const char *a, const char *b, size_t n){
    unsigned char ca, cb;
    if (n == 0) return 0;
    if (!a && !b) return 0;
    if (!a) return -1;
    if (!b) return 1;

    while (n-- > 0) {
        ca = (unsigned char)tolower((unsigned char)*a);
        cb = (unsigned char)tolower((unsigned char)*b);
        if (ca != cb) return (int)ca - (int)cb;
        if (*a == '\0' || *b == '\0') return 0;
        a++; b++;
    }
    return 0;
}

#endif /* JSTR_IMPL */
