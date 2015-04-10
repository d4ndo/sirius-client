#ifndef SPEECH_RECOG_H
#define SPEECH_RECOG_H

/* defined redundant because of static */
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
int speech_recog(char *url, unsigned char *wavfile, char **answer);

#endif
