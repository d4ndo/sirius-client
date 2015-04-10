#ifndef GLOBALDEFS_H
#define GLOBALDEFS_H

#define MAX_DATA_SIZE   (2048)
#define WAV_HEADER      "Content-Type: audio/vnd.wave; rate=16000"
#define IMG_HEADER      "Content-Type: image/jpeg"
#define QUERY           "?query=\0"
#define VERSION         "0.0.3"

struct MemoryStruct {
  char *memory;
  size_t size;
};

struct Config {
    const char* url;
    const char* qport;
    const char* aport;
    const char* iport;
};

#endif
