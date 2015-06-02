/*
 * sirius is a unix command like sirius client.
 * Copyright (C) 2015 https://github.com/d4ndo/sirius-client
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <malloc.h>
#include <stdlib.h>
#include <sys/stat.h>

int file2memory(char **binaryptr, size_t *size, FILE *file)
{
    char *newbuf = NULL;
    char *buffer = NULL;
    size_t alloc = 512;
    size_t nused = 0;
    size_t nread = 0;

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
