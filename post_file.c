#include <curl/curl.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include "post_file.h"
#include "globaldefs.h"
#include "file2memory.h"

/*
 * This function is similar to this curl call:
 *  curl -H "Content-Type: audio/vnd.wave; rate=16000"
 * --data-binary @what.is.the.capital.of.italy.wav localhost:8081
 * NO multipart/formdata  HTTP  POST
 */
int post_file(char *url, unsigned char *file, char **answer) {

    CURL *curl_handle;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_HEADER, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_POST, 1L);
    if (DEBUG) curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_PROTOCOLS,
                           CURLPROTO_HTTP | CURLPROTO_HTTPS);

    FILE *fd = NULL;
    if (!(fd = fopen(file, "rb"))) return -1;
    char *binaryptr = NULL;
    size_t binarySize = 0;

    if (0 > file2memory(&binaryptr, &binarySize, fd)) 
    {
        fclose(fd);
        return -1;
    }

    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, binaryptr);
    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, (curl_off_t)binarySize);

    struct MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;

    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "sirius-client");

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, WAV_HEADER);

    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);

    res = curl_easy_perform(curl_handle);

    if(res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: url %s\n",
                        curl_easy_strerror(res));
        fclose(fd);
        return -1;
    }

    /* verbose output */
    if (verbose) {
    double speed_upload, total_time;

    curl_easy_getinfo(curl_handle, CURLINFO_SPEED_UPLOAD, &speed_upload);
    curl_easy_getinfo(curl_handle, CURLINFO_TOTAL_TIME, &total_time);
    fprintf(stderr, "Speed: %.3f bytes/sec during %.3f seconds\n",
                    speed_upload, total_time);
    }

    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();
    curl_slist_free_all(headers);
    free(binaryptr);
    fclose(fd);

    *answer = chunk.memory;
    return chunk.size;
}

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    mem->memory = realloc(mem->memory, mem->size + realsize + 1);
    if(mem->memory == NULL) {
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}
