#include <curl/curl.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "ask_question.h"
#include "./pcrs/pcrs.h"
#include "globaldefs.h"

int ask_question(char *url, char *question, char **answer) {

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

static char *prepare_url(char *url, char *question)
{
    int err = 0;

    /* remove redundant whitespace */
    pcrs_job *job0 = NULL;
    char pattern0[] = "s/\\s+/ /g";

    if (NULL == (job0 = pcrs_compile_command(pattern0, &err)))
    {
        fprintf(stderr, "%s Compile error:  %s (%d).\n", pattern0, pcrs_strerror(err), err);
        exit(EXIT_FAILURE);
    }

    /* replace whitespace by %20 */
    pcrs_job *job = NULL;
    char pattern[] = "s/\\s/%20/g";

    if (NULL == (job = pcrs_compile_command(pattern, &err)))
    {
        fprintf(stderr, "%s Compile error:  %s (%d).\n", pattern, pcrs_strerror(err), err);
        exit(EXIT_FAILURE);
    }

    size_t length;
    char *request_question = NULL;

    if (0 > (err = pcrs_execute(job0, question, strlen(question), &request_question, &length)))
    {
        fprintf(stderr, "Exec error:  %s (%d)\n", pcrs_strerror(err), err);
        exit(EXIT_FAILURE);
    }

    if (0 > (err = pcrs_execute(job, question, strlen(question), &request_question, &length)))
    {
        fprintf(stderr, "Exec error:  %s (%d)\n", pcrs_strerror(err), err);
        exit(EXIT_FAILURE);
    }

    char *buffer = NULL;
    asprintf(&buffer, "%s%s%s", url, QUERY, request_question);
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
