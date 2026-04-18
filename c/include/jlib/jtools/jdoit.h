#pragma once
#ifndef JDOIT_H
#define JDOIT_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef enum DoItTag {
    DOIT_TAG_NONE = 0,
    DOIT_TAG_DOIT,
    DOIT_TAG_NOTE,
    DOIT_TAG_JLIB,
    DOIT_TAG_WIKI,
    DOIT_TAG_TODO,
    DOIT_TAG_WARN,
    DOIT_TAG_HACK,
    DOIT_TAG_CHECK,
    DOIT_TAG_BUGGY,
    DOIT_TAG_FIXME,
    DOIT_TAG_TODONT,
    DOIT_TAG_BREAKME,
    DOIT_TAG_WATCHME,
    
    DOIT_TAG_CUSTOM,

    DOIT_TAG_COUNT
} doittag_t;



/* <?> 
------
    If you ever want filtering like:
        show only TODO + FIXME

    You could switch to bit flags:

    typedef enum {
        JDOIT_TAG_NONE      = 0,
        JDOIT_TAG_TODO      = 1 << 0,
        JDOIT_TAG_FIXME     = 1 << 1,
        JDOIT_TAG_BUG       = 1 << 2,
        JDOIT_TAG_HACK      = 1 << 3,
        JDOIT_TAG_NOTE      = 1 << 4,
        JDOIT_TAG_OPTIMIZE  = 1 << 5,
        JDOIT_TAG_REFACTOR  = 1 << 6,
        JDOIT_TAG_JTODO     = 1 << 7
    } doittag_flags;
*/

typedef struct DoItItem{
    doittag_t tag;
    char *who;
    char *path;
    char *text;
    char *rawtag;
    char *priority;
    char *linestr;
    size_t line;
    size_t col;
} doititem_t;

typedef struct DoItList {
    doititem_t *items;
    size_t len;
    size_t cap;
} doitlist_t;

typedef struct DoItConfig {
    const char **ignoredirs;
    const char **tags;
    const char **exts;
    size_t tagcount;
    size_t extcount;
    size_t ignoredir_count;
    bool sensitive;
    bool recursive;
    bool scanexts;
    bool hidden;
} doitconf_t;

typedef struct DoItContext {
    doitconf_t *cfg;
    doitlist_t  *results;
    size_t files_seen;
    size_t files_read;
    size_t dirs_seen;
    size_t dirs_read;
    size_t matches_seen;
    size_t matches_read;
    size_t matches_captured;
    size_t matches_captured;
} doitctx_t;

/* Context
----------
    jdoit_context ctx;
    jdoit_context_init(&ctx);
    jdoit_context_default(&ctx);
    jdoit_scan_path(path, &ctx);

    void jdoit_context_init(jdoit_context *ctx);
    void jdoit_context_default(jdoit_context *ctx);
    void jdoit_context_reset_stats(jdoit_context *ctx);
    void jdoit_context_free(jdoit_context *ctx);

*/

static inline int doit_scan_file(const char *path, doitlist_t *val);
static inline int doit_scan_dir(const char *path, doitlist_t *val);
static inline void doitlist_t_free(doitlist_t *list);
static inline void doit_print(const doitlist_t *list);
static inline void doit_print_grouped(const doitlist_t *list);

int doit_scanpath(const char *path, const doitconf_t *cfg, doitlist_t *val);

#endif /* JDOIT_H */
#define JDOIT_IMPL // #debug-mode
#ifdef JDOIT_IMPL

static inline const char *doit_tag2str(doittag_t tag){
    switch (tag){
        case DOIT_TAG_DOIT:      return "DOIT";
        case DOIT_TAG_NOTE:      return "NOTE";
        case DOIT_TAG_JLIB:      return "JLIB";
        case DOIT_TAG_WIKI:      return "WIKI";
        case DOIT_TAG_TODO:      return "TODO";
        case DOIT_TAG_WARN:      return "WARN";
        case DOIT_TAG_HACK:      return "HACK";
        case DOIT_TAG_CHECK:     return "CHECK";
        case DOIT_TAG_BUGGY:     return "BUGGY";
        case DOIT_TAG_FIXME:     return "FIXME";
        case DOIT_TAG_TODONT:    return "TODONT";
        case DOIT_TAG_BREAKME:   return "BREAKME";
        case DOIT_TAG_WATCHME:   return "WATCHME";
        /* custom TAGS for future use ... */
        // case DOIT_TAG_NOTE:   return "NOTE";
        // case DOIT_TAG_NOTE:   return "NOTE";
        default: return "NONE";
    }
}

static inline doittag_t doit_str2tag(const char *s){
    if (!s) return DOIT_TAG_NONE;

    if (strcmp(s, "DOIT") == 0) return DOIT_TAG_DOIT;
    if (strcmp(s, "NOTE") == 0) return DOIT_TAG_NOTE;
    if (strcmp(s, "JLIB") == 0) return DOIT_TAG_JLIB;
    if (strcmp(s, "WIKI") == 0) return DOIT_TAG_WIKI;
    if (strcmp(s, "TODO") == 0) return DOIT_TAG_TODO;
    if (strcmp(s, "WARN") == 0) return DOIT_TAG_WARN;
    if (strcmp(s, "HACK") == 0) return DOIT_TAG_HACK;
    if (strcmp(s, "CHECK") == 0) return DOIT_TAG_CHECK;
    if (strcmp(s, "BUGGY") == 0) return DOIT_TAG_BUGGY;
    if (strcmp(s, "FIXME") == 0) return DOIT_TAG_FIXME;
    if (strcmp(s, "TODONT") == 0) return DOIT_TAG_TODONT;
    if (strcmp(s, "BREAKME") == 0) return DOIT_TAG_BREAKME;
    if (strcmp(s, "WATCHME") == 0) return DOIT_TAG_WATCHME;
    /* ... */

    return DOIT_TAG_NONE;
}

const char *default_ignoredirs[] = {
    ".git", "build", "dist", "node_modules", ".venv"
};

const size_t default_ignoredir_count =
    sizeof(default_ignoredirs) / sizeof(default_ignoredirs[0]);

const char *default_extensions[] = {
    ".c", ".h", ".cpp", ".hpp",
    ".py", ".sh", ".bash", ".zsh",
    ".js", ".ts", ".java", ".go", ".rs",
    ".htm", ".html", ".yaml", ".yml", ".json",
};

const size_t default_extension_count =
    sizeof(default_extensions) / sizeof(default_extensions[0]);

void doit_configinit(doitconf_t *cfg){
    if (!cfg) return;

    cfg->tags = NULL;
    cfg->tagcount = 0;
    cfg->exts = NULL;
    cfg->extcount = 0;
    cfg->ignoredirs = NULL;
    cfg->ignoredir_count = 0;
    cfg->recursive = 0;
    cfg->sensitive = 0;
    cfg->hidden = 0;
    cfg->scanexts = 0;
}

void jdoit_config_default(doitconf_t*cfg){
    if (!cfg) return;

    doit_configinit(cfg);
    cfg->exts = default_extensions;
    cfg->extcount = default_extension_count;
    cfg->ignoredirs = default_ignoredirs;
    cfg->ignoredir_count = default_ignoredir_count;
    cfg->recursive = 1;
    cfg->sensitive = 0;
    cfg->hidden = 0;
    cfg->scanexts = 0;
}

int doit_is_ignored_dir(const char *name, const doitconf_t *cfg) {
    if (!name || !cfg) return 0;
    /* skip "." and ".." always */
    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) return 1;
    /* hidden directories (".git", ".cache", etc.) */
    if (!cfg->hidden && name[0] == '.') return 1;
    if (!cfg->ignoredirs || cfg->ignoredir_count == 0) return 0;
    for (size_t i = 0; i < cfg->ignoredir_count; i++) {
        const char *ignore = cfg->ignoredirs[i];
        if (!ignore) continue;
        if (strcmp(name, ignore) == 0) return 1;
    }
    return 0;
}

int doit_has_allowed_extension(const char *path, const doitconf_t *cfg){
    if (!path || !cfg) return 0;
    /* allow everything if explicitly enabled */
    if (cfg->scanexts) return 1;
    /* return if emtpy */
    if (!cfg->exts || cfg->extcount == 0) return 1;

    for (size_t i = 0; i < cfg->extcount; i++){
        const char *ext = cfg->exts[i]; if (!ext) continue;
        size_t path_len, ext_len; path_len = strlen(path); ext_len = strlen(ext);

        if (path_len < ext_len) continue;
        if (strcmp(path + (path_len - ext_len), ext) == 0) return 1;

        const char *tail = path + (path_len - ext_len);
        if (cfg->sensitive) { if (strcmp(tail, ext) == 0) return 1; }
        else { if (strcasecmp(tail, ext) == 0) return 1; }
    }
    return 0;
}

const char *doit_basename(const char *path) {
    const char *slash = strrchr(path, '/');
#ifdef _WIN32
    const char *bslash = strrchr(path, '\\');
    if (!slash || (bslash && bslash > slash))
        slash = bslash;
#endif
    return slash ? slash + 1 : path;
}

/* Context
----------
    void jdoit_context_init(jdoit_context *ctx) {
    if (!ctx)
        return;

    jdoit_config_init(&ctx->cfg);
    jdoit_list_init(&ctx->results);

    ctx->files_seen = 0;
    ctx->files_scanned = 0;
    ctx->dirs_seen = 0;
    ctx->matches_found = 0;
    }

    void jdoit_context_default(jdoit_context *ctx) {
        if (!ctx)
            return;

        jdoit_context_init(ctx);
        jdoit_config_default(&ctx->cfg);
    }

    void jdoit_context_reset_stats(jdoit_context *ctx) {
        if (!ctx)
            return;

        ctx->files_seen = 0;
        ctx->files_scanned = 0;
        ctx->dirs_seen = 0;
        ctx->matches_found = 0;
    }

    void jdoit_context_free(jdoit_context *ctx) {
        if (!ctx)
            return;

        jdoit_list_free(&ctx->results);
    }

    Then your filter helpers become naturally tied into context-driven scanning.

    Example shape:

    if (jdoit_is_ignored_dir(name, &ctx->cfg))
        return;

    if (!jdoit_has_allowed_extension(path, &ctx->cfg))
        return;
*/

void walker(const char *path, int isdir, void *data) {
    doitctx_t *ctx = (doitctx_t*)data;

    if (isdir) {
        const char *name = doit_basename(path);
        if (doit_is_ignored_dir(name, ctx->cfg))
            return; // skip traversal

        return; // continue walking
    }

    if (!doit_has_allowed_extension(path, ctx->cfg))
        return;

    // doit_scan_file(path, &ctx->cfg, &ctx->results);
}

#endif /* JDOIT_IMPL */
