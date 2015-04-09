#include <curl/curl.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "ask_question.h"
#include "pcrs.h"
#include "globaldefs.h"

int ask_question(char *url, unsigned char *question, char **answer) {

    CURL *curl_handle;
    CURLcode res;

    char *modified_url = NULL;
    struct MemoryStruct chunk;

    chunk.memory = malloc(1);
    chunk.size = 0;

    modified_url = getRequest(url, question);

    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, modified_url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    res = curl_easy_perform(curl_handle);

    if(res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                        curl_easy_strerror(res));
        return -1;
    }

    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();

    *answer = chunk.memory;
    return chunk.size;
}

static char *getRequest(char *url, unsigned char *question)
{
    char *buffer = NULL;
    char query[] = QUERY;

    pcrs_job *job;
    char pattern[] = "s/\\s/%20/g";
    char *request_question;
    int err, linenum=0;
    size_t length;

    if (NULL == (job = pcrs_compile_command(pattern, &err)))
    {
        fprintf(stderr, "%s Compile error:  %s (%d).\n", pattern, pcrs_strerror(err), err);
        exit(EXIT_FAILURE);
    }

    if (0 > (err = pcrs_execute(job, question, strlen(question), &request_question, &length)))
    {
        fprintf(stderr, "Exec error:  %s (%d)\n", pcrs_strerror(err), err);
        exit(EXIT_FAILURE);
    }

    buffer = (char *)malloc(8 * MAX_DATA_SIZE);
    if(buffer == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    strncpy(buffer, url, MAX_DATA_SIZE);
    strncat(buffer, query, strlen(query));
    strncat(buffer, request_question, (3 * MAX_DATA_SIZE));

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
