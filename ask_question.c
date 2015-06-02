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

#include <curl/curl.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "ask_question.h"
#include "./pcrs/pcrs.h"
#include "globaldefs.h"

int ask_question(const char *url, const char *question, char **answer) {

    CURL *curl_handle;
    CURLcode res;
   
    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();
 
    /* Prepare GET request
     * Must be in this form:
     * mySiriusHost.com?query=What%20is%20the%20speed%20of%20light
     */ 
    char *modified_url = NULL;
    modified_url = prepare_url(url, question);
    curl_easy_setopt(curl_handle, CURLOPT_URL, modified_url);
    free(modified_url);

    struct MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;

    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    res = curl_easy_perform(curl_handle);

    if(res == CURLE_GOT_NOTHING) {
        chunk.memory = realloc(chunk.memory, 2 * sizeof(char));
        chunk.memory = strndup("\0", 2);
        chunk.size = 0;
    }

    if(res != CURLE_OK && res != CURLE_GOT_NOTHING) {
        fprintf(stderr, "curl error num %d\n", res);
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                        curl_easy_strerror(res));
        return -1;
    }

    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();

    *answer = chunk.memory;
    return chunk.size;
}

static char *prepare_url(const char *url, const char *question)
{
    int err = 0;
    size_t length;
    char *request_question = NULL;
    char *myquestion = strndup(question, MAX_DATA_SIZE);    

    /* remove redundant whitespace */
    pcrs_job *job0 = NULL;
    char pattern0[] = "s/\\s+/ /g";

    if (NULL == (job0 = pcrs_compile_command(pattern0, &err)))
    {
        fprintf(stderr, "%s Compile error:  %s (%d).\n", pattern0, pcrs_strerror(err), err);
        exit(EXIT_FAILURE);
    }

    if (0 > (err = pcrs_execute(job0, myquestion, strlen(myquestion), &request_question, &length)))
    {
        fprintf(stderr, "Exec error:  %s (%d)\n", pcrs_strerror(err), err);
        exit(EXIT_FAILURE);
    }
    free(job0);

    /* replace whitespace by %20 */
    pcrs_job *job = NULL;
    char pattern[] = "s/\\s/%20/g";

    if (NULL == (job = pcrs_compile_command(pattern, &err)))
    {
        fprintf(stderr, "%s Compile error:  %s (%d).\n", pattern, pcrs_strerror(err), err);
        exit(EXIT_FAILURE);
    }

    if (0 > (err = pcrs_execute(job, myquestion, strlen(myquestion), &request_question, &length)))
    {
        fprintf(stderr, "Exec error:  %s (%d)\n", pcrs_strerror(err), err);
        exit(EXIT_FAILURE);
    }
    free(job);

    /* build new GET Request string and return it */
    char *buffer = NULL;
    if (0 > asprintf(&buffer, "%s%s%s", url, QUERY, request_question))
    {
        free(request_question);
        fprintf(stderr, "Allocation error @ prepar_url.\n");
        exit(EXIT_FAILURE);
    }
    free(request_question);

    return buffer;
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
