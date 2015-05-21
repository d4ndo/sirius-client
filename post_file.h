#ifndef POST_FILE_H
#define POST_FILE_H

/* defined redundant because of static */
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
int post_file(const char *url, const char *file, char **answer);

#endif
