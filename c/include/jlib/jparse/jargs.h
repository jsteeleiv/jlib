#pragma once
#ifndef JARGS_H
#define JARGS_H

/* Jargs
--------
    my personal project dealing with arguments
        -> parsing, capturing, passing, etc...
    
    

    some of the manual arg parsing was learned from `ed` the editor
*/

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

#include "../jdata/jvec.h"
#include "../jstd/jerr.h"
#include "../jstd/jlog.h"
#include "../jtype/jkind.h"

typedef enum HasArgument {
    ARGP_NO= 0, ARGP_OK, ARGP_MYB
} hasarg_e;

typedef struct ArgumentOption {
    int code;         // short option; letter or code (code != 0)
    const char *name; // long option; name (may be NULL)
    hasarg_e hasarg;
} argopt_t;

typedef struct ArgumentRecord {
    int code;
    char *arg;
} argrec_t;

typedef struct ArgumentParser {
    argrec_t *data;
    char *error;
    int data_sz;
    int error_sz;
} argparse_t;

static inline void free_data(argparse_t *const ap);
static inline char add_error(argparse_t *const ap, const char *const msg);

static inline char argp_init(
    argparse_t *const ap, const int argc, const char *const argv[],
    const argopt_t opts[], const char inorder
);

static inline void argp_free(argparse_t *const ap);
static inline const char *argp_err(const argparse_t *const ap);
/* number of args may differ from argc's count */ 
static inline int argp_args(const argparse_t *const ap);
/* code(i) == 0 ? arg(i) = non-option : arg(i) is the argument (or empty) */
static inline int argp_code(const argparse_t *const ap, const int i);
static inline const char *argp_arg(const argparse_t *const ap, const int i);
static inline void *argp_rszbuf(void *buf, const int min);

static inline char rec_append(
    argparse_t *const ap, const int code, const char *const arg
);

static inline char parse_longopt(
    argparse_t *const ap, const char *const opt, const char *const arg,
    const argopt_t opts[], int *const argidxp
);

static inline char parse_shortopt(
    argparse_t *const ap, const char *const opt, const char *const arg,
    const argopt_t opts[], int *const argidxp
);



typedef struct VariadicArguments{
    any_t *items;
    va_list list;
    size_t size;
    size_t cap;
    bool die;
    bool init;
} vargs_t;

typedef struct {
    error_t err;
    logger_t log;
    vargs_t *args;
    char *msg;
} Jarg;

static inline vargs_t vargs_init(va_list list);
static inline vargs_t vargs_dup(vargs_t *src);
static inline any_v vargs_pop(vargs_t *vargs, kind_t kind);

// void _args(const char *init_arg, ...);

#endif /* JARGS_H */
// #define JARGS_IMPL // #debug-mode
#ifdef JARGS_IMPL

/* normal args & such */
static inline void *argp_rszbuf(void *buf, const int min){
    if (buf) buf = realloc(buf, min);
    else buf = malloc(min);
    return buf;
}

static inline void free_data(argparse_t *const ap){
    for (int i = 0; i < ap->data_sz; ++i) free(ap->data[i].arg);
    if (ap->data) { free(ap->data); ap->data = 0; }
    ap->data_sz = 0;
}

static inline char add_error(argparse_t *const ap, const char *const msg){
    const int len = strlen(msg);
    void *tmp = argp_rszbuf(ap->error, ap->error_sz + len + 1);
    if (!tmp) return 0;
    ap->error = (char*)tmp;
    strncpy(ap->error + ap->error_sz, msg, len + 1);
    ap->error_sz += len;
    return 1;
}

static inline char rec_append(
    argparse_t *const ap, const int code, const char *const arg){
//  -----       
    const int len = strlen(arg);
    /* grow by 1 */
    void *tmp = argp_rszbuf(ap->data, (ap->data_sz + 1) * sizeof(argrec_t));
    if (!tmp) return 0;
    ap->data = (argrec_t*)tmp;
    argrec_t *rec = &(ap->data[ap->data_sz]);
    rec->code = code;
    rec->arg = 0;
    // tmp = argp_rszbuf(rec->arg, len + 1);
    if (!(tmp = argp_rszbuf(rec->arg, len + 1))) return 0;
    rec->arg = (char*)tmp;
    strncpy(rec->arg, arg, len + 1);
    ++ap->data_sz;
    return 1;
}

static inline char parse_longopt( argparse_t *const ap, const char *const opt,
  const char *const arg, const argopt_t opts[], int *const argidxp){

    unsigned len;
    int i, idx = -1;
    char exact = 0, opaque = 0;

    for (len = 0; opt[len+2] && opt[len+2] != '='; ++len);
    for (i = 0; opts[i].code != 0; ++i){
        if (opts[i].name && !strncmp(opts[i].name, &opt[2], len)){
            if (strlen(opts[i].name) == len){ // exact match found
                idx = i; exact = 1; break;
            }
            else if (idx < 0) idx = i; // first non-exact match found!
            else if (
                opts[idx].code != opts[i].code ||
                opts[idx].hasarg != opts[i].hasarg
            ) opaque = 1;
        }
    }
    if (opaque && !exact){
        add_error(ap, "option '");
        add_error(ap, opt);
        add_error(ap, "' is opaque");
        return 1;
    }
    if (idx < 0){
        add_error( ap, "unrecognized option '" );
        add_error( ap, opt ); add_error( ap, "'" );
        return 1;
    }
    ++*argidxp;

    if (opt[len+2]){
        if (opts[idx].hasarg == ARGP_NO){
            add_error(ap, "option '--");
            add_error(ap, opts[idx].name);
            add_error(ap, "' doesn't allow an argument");
            return 1;
        }
        if (opts[idx].hasarg == ARGP_OK && !opt[len+3]){
            add_error(ap, "option '--");
            add_error(ap, opts[idx].name);
            add_error(ap, "' requires an argument");
            return 1;
        }
        return rec_append(ap, opts[idx].code, &opt[len+3]);
    }
    if (opts[idx].hasarg == ARGP_OK){
        if (!arg || !arg[0]){
            add_error(ap, "option '--");
            add_error(ap, opts[idx].name);
            add_error(ap, "' requires an argument");
            return 1;
        }
        ++*argidxp;
        return rec_append(ap, opts[idx].code, arg);
    }
    return rec_append(ap, opts[idx].code, "");    
}

static inline char parse_shortopt(argparse_t *const ap, const char *const opt,
  const char *const arg, const argopt_t opts[], int *const argidxp){

    int cidx = 1; // char index in opt
    while (cidx > 0){
        int i, idx = -1;
        const unsigned char code = opt[cidx];
        char codestr[2];
        codestr[0] = code; codestr[1] = 0;
        if (code != 0)
            for (i = 0; opts[i].code; ++i)
                if (code == opts[i].code){ idx = i; break; }

        if (idx < 0){
            add_error(ap, "invalid option -- "); add_err(ap, codestr);
            return 1;
        }
        if (opt[++cidx] == 0){ ++*argidxp; cidx = 0; }
        if (opts[idx].hasarg != ARGP_NO && cidx > 0 && opt[cidx]){
            if (!rec_append(ap, code, &opt[cidx])) return 0;
            ++*argidxp; cidx = 0;
        }
        else if (opts[idx].hasarg == ARGP_OK){
            if (!arg || !arg[0]){
                add_error( ap, "option requires an argument -- " );
                add_error( ap, codestr );
                return 1;
            }
            ++*argidxp; cidx = 0;
            if(!rec_append(ap, code, arg)) return 0;
        }
        else if (!rec_append(ap, code, "")) return 0;
    }
    return 1;
}

static inline char argp_init( argparse_t *const ap, const int argc,
const char *const argv[], const argopt_t opts[], const char inorder){

    if (argc < 2 || !argv || !opts) return 1;

    const char **nopts = 0;  // skipped non-options
    static int nopts_sz = 0; //number of skipped non-options
    int argidx = 1;
    int i; // global iterator

    ap->data = ap->data_sz = ap->error = ap->error_sz = 0;
    while (argidx < argc){
        const unsigned char c1 = argv[argidx][0];
        const unsigned char c2 = (c1 ? argv[argidx][1] : 0);

        if (c1 == '-' && c2){
            /* option found */
            const char *const opt = argv[argidx];
            const char *const arg = (argidx + 1 < argc) ? argv[argidx+1] : 0;
            
            if (c2 == '-'){
                if (!argv[argidx][2]) { ++argidx; break; } /* found "--" */
                else if (!parse_longopt(ap, opt, arg, opts, &argidx)) return 0;
            }
            else if (!parse_shortopt(ap, opt, arg, opts, &argidx)) return 0;
            if (ap->error) break;
        }
        else {
            if (!inorder){
                void *tmp = argp_rszbuf(nopts, (nopts_sz + 1) * sizeof *nopts);
                if (!tmp) return 0;
                nopts = (const char**)tmp;
                nopts[nopts_sz++] = argv[argidx++];
            }
            else if (!rec_append(ap, 0, argv[argidx++])) return 0;
        }
    }
    if (ap->error) free_data(ap);
    else{
        for(i = 0; i < nopts_sz; ++i){
            if (!rec_append(ap, 0, nopts[i])) return 0;
        }
        while (argidx < argc){
            if (!rec_append(ap, 0, argv[argidx++])) return 0;

        }
    }
    if (nopts) free(nopts);
    return 1;
}

static inline void argp_free(argparse_t *const ap){
    free_data(ap);
    if (ap->error){free(ap->error); ap->error = 0; }
    ap->error_sz = 0;
}

static inline const char *argp_err(const argparse_t *const ap){ return ap->error; }

static inline int argp_args(const argparse_t *const ap){ return ap->data_sz; }

static inline int argp_code(const argparse_t *const ap, const int i){
    if (i >= 0 && i < argp_args(ap)) return ap->data[i].code;
    else return 0;
}

static inline const char *argp_arg(const argparse_t *const ap, const int i){
    if (i >= 0 && i < argp_args(ap)) return ap->data[i].arg;
    else return "";
}

/* varargs n such */

static inline vargs_t vargs_init(va_list list){
    vargs_t vargs = {0};
    va_copy(vargs.list, list);
    return vargs;
}

static inline vargs_t vargs_dup(vargs_t *src){
    vargs_t dest;
    va_copy(dest.list, src->list);
    return dest;
}

static inline any_v vargs_pop(vargs_t *vargs, kind_t kind){
    any_v out = {0};
    out.kind = kind;
    switch (kind) {
        case BOOL:
            out.as.boolean = (bool)va_arg(vargs->list, int);
            break;
        case I8:
            out.as.i8 = (int8_t)va_arg(vargs->list, int);
            break;
        case I16:
            out.as.i16 = (int16_t)va_arg(vargs->list, int);
            break;
        case I32:
            out.as.i32 = (int32_t)va_arg(vargs->list, int);
            break;
        case I64:
            out.as.i64 = (int64_t)va_arg(vargs->list, long long);
            break;
        case U8:
            out.as.u8 = (uint8_t)va_arg(vargs->list, unsigned int);
            break;
        case U16:
            out.as.u16 = (uint16_t)va_arg(vargs->list, unsigned int);
            break;
        case U32:
            out.as.u32 = (uint32_t)va_arg(vargs->list, unsigned int);
            break;
        case U64:
            out.as.u64 = (uint64_t)va_arg(vargs->list, unsigned long long);
            break;
        case F32:
            out.as.f32 = (float)va_arg(vargs->list, double);
            break;
        case F64:
            out.as.f64 = va_arg(vargs->list, double);
            break;
        case CSTR:
            out.as.cstr = va_arg(vargs->list, const char *);
            break;
        case STR:
            out.as.str = va_arg(vargs->list, char *);
            break;
        case CONST_PTR:
            out.as.cptr = va_arg(vargs->list, const void *);
            break;
        case PTR:
            out.as.ptr = va_arg(vargs->list, void *);
            break;
        case ISNULL:
            out.as.null = NULL;
            break;
        default:
            out.kind = CUSTOM;
            out.as.ptr = va_arg(vargs->list, void *);
            break;
    }
    return out;
}



#endif /* JARGS_IMPL */