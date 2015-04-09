#ifndef GLOBALDEFS_H
#define GLOBALDEFS_H

#define MAX_DATA_SIZE   (2048)
#define SIRIUS_URL      "localhost\0"
#define QUESTION_PORT   ":8080\0"
#define SPEECH_PORT     ":8081\0"
#define IMAGE_PORT      ":8082\0"
#define WAV_HEADER      "Content-Type: audio/vnd.wave; rate=16000"
#define IMG_HEADER      "Content-Type: image/jpeg"
#define QUERY           "?query=\0"
#define VERSION         "0.0.2"

struct MemoryStruct {
  char *memory;
  size_t size;
};

#endif
