#pragma once
#ifndef JMAKE_H
#define JMAKE_H

#include <assert.h>

#define JPROC_IMPL
#include "../jio/jproc.h"

#ifndef REBUILD
#  if _WIN32
#    if defined(__GNUC__)
#      define REBUILD(binpath, srcpath) CMD("gcc", "-o", binpath, srcpath)
#    elif defined(__clang__)
#      define REBUILD(binpath, srcpath) CMD("clang", "-o", binpath, srcpath)
#    elif defined(_MSC_VER)
#      define REBUILD(binpath, srcpath) CMD("cl.exe", source_path)
#    endif
#  else
#    define REBUILD_URSELF(binpath, srcpath) CMD("cc", "-o", binpath, srcpath)
#  endif
#endif

/* Go Rebuild Urself™ Technology {nobuild.h | @tsoding}
-------------------------------------------------------
    How to use it:
        int main(int argc, char** argv) {
            REBUILD_SELF(argc, argv);
            // actual work
            return 0;
        }
*/

#define REBUILD_SELF(argc, argv) \
    do { \
        const char *srcpath = __FILE__; \
        assert(argc >= 1); \
        const char *binpath = argv[0]; \
        if (is_path1_modified_after_path2(srcpath, binpath)) { \
            RENAME(binpath, CONCAT(binary_path, ".old")); \
            REBUILD(binpath, srcpath); \
            cmd_t cmd = { \
                .line = { \
                    .elems = (Cstr*) argv,  \
                    .count = argc, \
                }, \
            }; \
            INFO("CMD: %s", cmd_show(cmd)); \
            cmd_sync(cmd); \
            exit(0); \
        } \
    } while(0)
// The implementation idea is stolen from https://github.com/zhiayang/nabs

static inline void rebuild_self(const char *binpath, const char *srcpath);



#endif /* JMAKE_H */

#ifdef JMAKE_IMPL



#endif /* JMAKE_IMPL */