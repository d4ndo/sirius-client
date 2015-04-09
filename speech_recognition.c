#include <curl/curl.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "ask_question.h"
#include "pcrs.h"
#include "globaldefs.h"

int speech_recog(char *url, unsigned char *wavfile, char **answer) {

    CURL *curl_handle;
    CURLcode res;

    char *modified_url = NULL;
    struct MemoryStruct chunk;

    chunk.memory = malloc(1);
    chunk.size = 0;

    struct curl_httppost *formpost=NULL;
    struct curl_httppost *lastptr=NULL;
    struct curl_slist *headerlist=NULL;
    static const char buf[] = WAV_HEADER;

    curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME, "sendfile",
                 CURLFORM_FILE, wavfile,
                 CURLFORM_END);

    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();

    headerlist = curl_slist_append(headerlist, buf);
    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headerlist);
    curl_easy_setopt(curl_handle, CURLOPT_HTTPPOST, formpost);
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);

    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    res = curl_easy_perform(curl_handle);

    if(res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: url %s\n",
                        curl_easy_strerror(res));
        return -1;
    }

    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();

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
