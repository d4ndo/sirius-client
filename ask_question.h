#ifndef ASK_QUESTION_H
#define ASK_QUESTION_H

static char *prepare_url(char *url, char *question);
/* defined redundant because of static */
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
int ask_question(char *url, char *question, char **answer);

#endif
