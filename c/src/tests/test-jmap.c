#define JMAP_IMPL
#include "../../include/jlib/jdata/jmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

static int tests_run = 0;
static int tests_failed = 0;

static void check_bool(const char *label, bool expr){
    tests_run++;

    if (expr){
        printf("[PASS] %s\n", label);
    } else {
        printf("[FAIL] %s\n", label);
        tests_failed++;
    }
}

static void check_str(const char *label, const char *got, const char *want){
    tests_run++;

    if (got && want && strcmp(got, want) == 0){
        printf("[PASS] %s -> \"%s\"\n", label, got);
        return;
    }

    if (!got && !want){
        printf("[PASS] %s -> NULL\n", label);
        return;
    }

    printf("[FAIL] %s -> got=%s want=%s\n",
        label,
        got ? got : "NULL",
        want ? want : "NULL"
    );
    tests_failed++;
}

static void check_size(const char *label, size_t got, size_t want){
    tests_run++;

    if (got == want){
        printf("[PASS] %s -> %zu\n", label, got);
    } else {
        printf("[FAIL] %s -> got=%zu want=%zu\n", label, got, want);
        tests_failed++;
    }
}

int main(void){
    map_t map = map_init(16); /* should normalize upward to power-of-two */

    printf("=== jmap smoke test ===\n\n");

    check_bool("map_ready after init", map_ready(&map));
    check_bool("map.cap is power-of-two", map_ispow2(map.cap));
    check_size("map.cap = 13 | next-pow2", map_nextpow2(13), 16);
    check_size("map.cap = 42 | next-pow2", map_nextpow2(42), 64);
    check_size("map.size starts at 0", map.size, 0);

    printf("\n--- insert / get ---\n");
    check_bool("put name=zed", map_put(&map, "name", "zed") == 0);
    check_bool("put lang=c", map_put(&map, "lang", "c") == 0);
    check_bool("put os=linux", map_put(&map, "os", "linux") == 0);

    check_str("get name", map_get(&map, "name"), "zed");
    check_str("cget lang", map_cget(&map, "lang"), "c");
    check_str("get os", map_get(&map, "os"), "linux");

    check_bool("has name", map_has(&map, "name"));
    check_bool("has missing=false", !map_has(&map, "missing"));
    check_size("map.size after 3 inserts", map.size, 3);

    printf("\n--- overwrite ---\n");
    check_bool("overwrite name=zorander", map_put(&map, "name", "zorander") == 0);
    check_str("get overwritten name", map_get(&map, "name"), "zorander");
    check_size("map.size unchanged after overwrite", map.size, 3);

    printf("\n--- delete ---\n");
    check_bool("delete lang", map_del(&map, "lang") == 0);
    check_bool("has lang after delete=false", !map_has(&map, "lang"));
    check_str("get lang after delete", map_get(&map, "lang"), NULL);
    check_size("map.size after delete", map.size, 2);

    check_bool("delete missing returns error", map_del(&map, "missing") == -1);

    printf("\n--- explicit grow ---\n");
    {
        size_t old_cap = map.cap;
        check_bool("grow to 64", map_grow(&map, 64) == 0);
        check_bool("cap grew or stayed valid", map.cap >= old_cap);
        check_bool("cap still power-of-two", map_ispow2(map.cap));

        check_str("name survives grow", map_get(&map, "name"), "zorander");
        check_str("os survives grow", map_get(&map, "os"), "linux");
    }

    printf("\n--- bulk insert / auto grow stress ---\n");
    for (int i = 0; i < 128; i++){
        char key[32];
        char val[32];

        snprintf(key, sizeof(key), "k%d", i);
        snprintf(val, sizeof(val), "v%d", i);

        if (map_put(&map, key, val) != 0){
            printf("[FAIL] bulk put %s=%s\n", key, val);
            tests_failed++;
            break;
        }
    }

    check_str("bulk get k0", map_get(&map, "k0"), "v0");
    check_str("bulk get k42", map_get(&map, "k42"), "v42");
    check_str("bulk get k127", map_get(&map, "k127"), "v127");
    check_bool("cap remains power-of-two after bulk inserts", map_ispow2(map.cap));

    printf("\n--- hash/index demo ---\n");
    {
        const char *sample = "example";
        size_t hash = map_hash(sample);
        size_t idx  = map_index(&map, sample);

        printf("sample key: \"%s\"\n", sample);
        printf("hash      : %zu\n", hash);
        printf("index     : %zu\n", idx);
        printf("cap       : %zu\n", map.cap);

        check_bool("index is within cap", idx < map.cap);
    }

    printf("\n--- cleanup ---\n");
    map_free(&map);
    check_bool("map not ready after free", !map_ready(&map));
    check_size("map.size after free", map.size, 0);
    check_size("map.cap after free", map.cap, 0);

    printf("\n=== summary ===\n");
    printf("tests run   : %d\n", tests_run);
    printf("tests failed: %d\n", tests_failed);

    return (tests_failed == 0) ? 0 : 1;
}
