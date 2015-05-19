#include <malloc.h>
#include <stdlib.h>
#include <sys/stat.h>

int file2memory(char **binaryptr, size_t *size, FILE *file)
{
    char *newbuf;
    char *buffer = NULL;
    size_t alloc = 512;
    size_t nused = 0;
    size_t nread;

    if(file) {
        do {
            if(!buffer || (alloc == nused)) {
                /* size_t overflow detection for huge files */
                if(alloc + 1 > ((size_t) - 1) / 2) {
                    free(buffer);
                    return -1;
                }
                alloc *= 2;
                /* allocate an extra char, reserved space, for null termination */
                if((newbuf = realloc(buffer, alloc + 1)) == NULL) {
                    free(buffer);
                    return -1;
                }
                buffer = newbuf;
            }
            nread = fread(buffer+nused, 1, alloc-nused, file);
            nused += nread;
        } while(nread);
        /* null terminate the buffer in case it's used as a string later */
        buffer[nused] = '\0';
        /* free trailing slack space, if possible */
        if(alloc != nused) {
            if((newbuf = realloc(buffer, nused + 1)) == NULL) {
                free(buffer);
                return -1;
            }
            buffer = newbuf;
        }
        /* discard buffer if nothing was read */
        if(!nused) {
            free(buffer); /* no string */
            *size = nused;
            *binaryptr = NULL;
            return -1;
        }
    }
    *size = nused;
    *binaryptr = buffer;
    return 0;
}
