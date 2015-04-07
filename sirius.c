#include <stdio.h>
#include <curl/curl.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "ask_question.h"
#include "globaldefs.h"

static int verbose = 0;
static const struct option options[] = {
	{"help"         , no_argument      , NULL, 'h'},
        {"url"          , required_argument, NULL, 'u'},
        {"version"      , no_argument      , NULL, 'v'},
	{"verbose"      , no_argument      , &verbose, 1},
	{NULL, 0, NULL, 0}
};

static char *optstring = "hu:v";
static void usage(int status);
static unsigned char *readStdin(int *length);

int main(int argc, char **argv)
{
    int opt, lindex = -1;
    char *url = NULL;
    char *answer = NULL;
    unsigned char *question = NULL;
    int length = 0;

    while((opt = getopt_long(argc, argv, optstring, options, &lindex)) != -1) {
        switch(opt) {
            case 'h':
	        if(lindex == 0) {
                    usage(EXIT_SUCCESS);
	        } else {
	            usage(EXIT_SUCCESS);
		}
		exit(EXIT_SUCCESS);
                break;
	    case 'u':
                url = optarg;
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
        question = (unsigned char *)argv[optind];
        length = strlen((char *)question);
    }
    
    if (url == NULL)
    {
        char localhost[] = SIRIUS_URL;
        url = (char *)malloc(MAX_DATA_SIZE + 1);
        strcpy(url, localhost);
    }
   
    if (question == NULL)
    { 
        question = readStdin(&length);
    }

    int ret = 0;
    if (0 > (ret = ask_question(url, question, &answer))) {
        fprintf(stderr, "Could not connect to host: %s\n", url);
        exit(EXIT_FAILURE); 
    }

    if (verbose)
    {
        fprintf(stderr, "%s\n", question);
        fprintf(stderr, "%d bytes retrieved\n", ret);
    }

    fprintf(stdout, "%s", answer);
}

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
    "               WAV_FILE some voice audio file for sirius to recognice. NOT IMPLEMENTED\n\n"
    "  -i, --imagematch [IMAGE_FILE]\n"
    "               IMAGE_FILE some image file for sirius to match. NOT IMPLEMENTED\n\n"
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

static unsigned char *readStdin(int *length)
{
    unsigned char *buffer;
    int ret;

    buffer = (unsigned char *)malloc(MAX_DATA_SIZE + 1);
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
