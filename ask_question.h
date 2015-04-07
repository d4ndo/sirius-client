#ifndef ASK_QUESTION_H
#define ASK_QUESTION_H

static char *getRequest(char *url, unsigned char *question);
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
int ask_question(char *url, unsigned char *question, char **answer);

#endif
