#if(!defined(BLIB_FILE_H))
#define BLIB_FILE_H

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

struct blib_fileBuffer_t;
typedef struct blib_fileBuffer_t blib_fileBuffer_t;

struct blib_fileBuffer_t{
  size_t len;
  char *text;
  bool error: 1;
};

#if !defined(BLIB_FILE_BUFFER_CHUNK_SIZE)
/*Number of chars before buffer must be expanded*/
#define BLIB_FILE_BUFFER_CHUNK_SIZE (64 /* chars */) 
#endif

#if !defined(BLIB_FILE_BUFFER_GROWTH)
#define BLIB_FILE_BUFFER_GROWTH (4 /* times */) /*Scalar to multiply chunk size when expanding*/
#endif

void blib_fileBuffer_close(blib_fileBuffer_t file);

blib_fileBuffer_t blib_fileBuffer_read(const char *filename);

#endif /*BLIB_FILE_H*/
