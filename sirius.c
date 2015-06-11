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

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdio.h>
#include <curl/curl.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "./inih/ini.h"
#include "ask_question.h"
#include "post_file.h"
#include "globaldefs.h"

/* verbose is extern @ globaldefs.h */
int verbose = 0;

static const struct option options[] = {
	{"help"         , no_argument      , NULL, 'h'},
        {"speechrecog"  , required_argument, NULL, 'a'},
        {"question"     , no_argument      , NULL, 'q'},
        {"tableflip"    , no_argument      , NULL, 't'},
        {"imagematch"   , required_argument, NULL, 'i'},
        {"url"          , required_argument, NULL, 'u'},
        {"version"      , no_argument      , NULL, 'v'},
	{"verbose"      , no_argument      , &verbose, 1},
	{NULL, 0, NULL, 0}
};

static char *optstring = "ha:qti:u:v";
static void usage(int status);
static char *readStdin(int *length);
static int handler(void *user, const char *section, 
                   const char *name, const char *value);

int main(int argc, char **argv)
{
    /* variables used and set by getopt */
    int opt, lindex = -1;
    char *wavfile = NULL;
    char *imgfile = NULL;
    int q = 0;
    char *url = NULL;
    char *answer = NULL;
    char *question = NULL;
    int length = 0;

    /* get HOME directory from environment */
    const char *homedir;
    if ((homedir = getenv("HOME")) == NULL) {
        homedir = getpwuid(getuid())->pw_dir;
    }
    /* see 21st Century C from Ben Klemens about strings */
    char *ini = NULL;
    char *home = strndup(homedir, MAX_DATA_SIZE);
    asprintf(&ini, "%s/.sirius.ini", home);
    free(home);

    /* parse config INI file */
    struct Config config;

    if (ini_parse(ini, handler, &config) < 0) {
        fprintf(stderr, "Can't load %s *\n", ini);
        exit(EXIT_FAILURE);
    }
    free(ini);
 
    while((opt = getopt_long(argc, argv, optstring, options, &lindex)) != -1) {
        switch(opt) {
            case 'h':
	        usage(EXIT_SUCCESS);
		exit(EXIT_SUCCESS);
                break;
	    case 'a':
                wavfile = strndup(optarg, MAX_DATA_SIZE);
                break;
            case 'q':
                q = 1;
                break;
            case 't':
                fprintf(stdout, "(╯°□°）╯︵ ┻━┻\n"); 
                exit(EXIT_SUCCESS);
	    case 'i':
                imgfile = strndup(optarg, MAX_DATA_SIZE);
                break;
            case 'u':
                url = strndup(optarg, MAX_DATA_SIZE);
                break;
            case 'v':
                fprintf(stdout, "sirius Version %s\n", VERSION);
                exit(EXIT_SUCCESS);
                break;
            case 0:
	        break;
            default:
	        usage(EXIT_FAILURE);
	        exit(EXIT_FAILURE);
	        break;
        }
    }

    if (optind < argc) {
        question = (char *)argv[optind];
        length = strlen((char *)question);
    }

    if (question == NULL && wavfile == NULL && imgfile == NULL)
    {
        question = readStdin(&length);
    }

    int ret = 0;

    if (wavfile != NULL)
    {
        /* Append SPEECH port to url */
        char *urlport = NULL;

        if (url != NULL) {
            asprintf(&urlport, "%s", url);
        } else {
            asprintf(&urlport, "%s:%s", config.url, config.aport);
        }

        if (0 > (ret = post_file(urlport, wavfile, &answer))) {
            fprintf(stderr, "Could not connect to host: %s\n", urlport);
            exit(EXIT_FAILURE);
        }

        /* 
         * reset. Using free to avoid memory leak 
         * and set back to NULL, because question is still being used 
         */
        if (question != NULL) {
            free(question); 
            question = NULL;
        }

        if (q) question = strndup(answer, MAX_DATA_SIZE);
    } 

    if (imgfile != NULL)
    {
        /* Append IMAGE matching PORT to url */
        char *urlport = NULL;
        if (url != NULL) {
            asprintf(&urlport, "%s", url);
        } else {
            asprintf(&urlport, "%s:%s", config.url, config.iport);
        }
        if (0 > (ret = post_file(urlport, imgfile, &answer))) {
            fprintf(stderr, "Could not connect to host: %s\n", urlport);
            exit(EXIT_FAILURE);
        }
    }

    if (question != NULL) {
        /* Append QUESTION port to url */
        char *urlport = NULL;
        if (url != NULL) {
            asprintf(&urlport, "%s", url);
        } else {
            asprintf(&urlport, "%s:%s", config.url, config.qport);
        }
        if (0 > (ret = ask_question(urlport, question, &answer))) {
            fprintf(stderr, "Could not connect to host: %s\n", urlport);
            exit(EXIT_FAILURE);
        }
    }

    if (verbose)
    {
        fprintf(stderr, "%s\n", question);
        fprintf(stderr, "%d bytes retrieved\n", ret);
    }
    fprintf(stdout, "%s", answer);
    free(url);
}

/* user HELP */
static void usage(int status)
{
    int ok = status ? 0 : 1;
    if (ok)
    fprintf(stdout,
    "Usage: sirius [OPTION]... QUESTION ...\n\n"
    "  [QUESTION]   QUESTION to ask sirius.\n"
    "               If it is not specified, data will be\n"
    "               taken from standard input.\n\n"
    "  -a, --speechrecog [WAV FILE]\n"
    "               Automatic speech recognition.\n"
    "               WAV_FILE some voice audio file for sirius to recognice.\n\n"
    "  -q, --question\n"
    "               WAV_FILE is a question.\n\n"
    "  -i, --imagematch [IMAGE_FILE]\n"
    "               IMAGE_FILE some image file for sirius to match.\n\n"
    "  -u, --url [URL]\n"
    "               URL from sirius host..\n\n"
    "  -c, --cert [CERTIFICATE_FILE]\n"
    "               CERTIFICATE_FILE the certificate for sirius host. NOT IMPLEMENTED\n\n"
    "  -v, --version\n"
    "               Display the version number of sirius.\n\n"
    "      --verbose\n"
    "               Display verbose information to stderr.\n\n"
    "  -h, --help   Display this help message.\n"

    );
    else
    fprintf(stderr,
    "Use sirius --help for more information\n"
    "Sirius intelligent personal assistent.\n"
    );
}

/* read QUESTION from stdin */
static char *readStdin(int *length)
{
    char *buffer;
    int ret;

    buffer = (char *)malloc(MAX_DATA_SIZE + 1);
    if(buffer == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    ret = fread(buffer, 1, MAX_DATA_SIZE, stdin);
    if(ret == 0) {
        fprintf(stderr, "No input data.\n");
        exit(EXIT_FAILURE);
    }
    if(feof(stdin) == 0) {
        fprintf(stderr, "Input data is too large.\n");
        exit(EXIT_FAILURE);
    }

    buffer[ret] = '\0';
    *length = ret;

    return buffer;
}

/* Parse INI file handler */
static int handler(void *user, const char *section, const char *name,
                   const char *value)
{
    struct Config *pconfig = (struct Config*)user;

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("host", "url")) {
        pconfig->url = strndup(value, MAX_DATA_SIZE);
    } else if (MATCH("host", "question_port")) {
        pconfig->qport = strndup(value, MAX_DATA_SIZE);
    } else if (MATCH("host", "speech_recognition_port")) {
        pconfig->aport = strndup(value, MAX_DATA_SIZE);
    } else if (MATCH("host", "image_matching_port")) {
        pconfig->iport = strndup(value, MAX_DATA_SIZE);
    } else {
        return 0;  /* unknown section/name, error */
    }
    return 1;
}
