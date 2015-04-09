#ifndef IMAGE_MATCH_H
#define IMAGE_MATCH_H

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
int image_match(char *url, unsigned char *imgfile, char **answer);

#endif
