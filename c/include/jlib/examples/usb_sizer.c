#define JUSB_IMPL
#include <include/jlib/jusb.h>
#include <stdio.h>
#include <sys/stat.h>

#ifndef ISO_SZ
#define ISO_SZ BUFFER
#endif

static long long file_size(const char *p) {
    long long bytes;
    /* do some stuff */
    return bytes;
}

int main(void) {
   uint64_t need = (uint64_t)ISO_SZ;
   uint64_t usb = j_usb_min_size_with_buffer(need, BUFFER);
   return 0;
}