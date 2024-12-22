/*----------------------------------LEGAL--------------------------------------

  MIT License

  Copyright (c) 2023 Benjamin Joseph Brooks

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

  -----------------------------------------------------------------------------*/

#ifndef BLIB_FILE_H
#define BLIB_FILE_H

#include <stdio.h>
#include <stdlib.h>
#include "blib.h"

#define BLIB_FILE_BUFFER_CHUNK_SIZE (64 /* chars */)
#define BLIB_FILE_BUFFER_GROWTH (4 /* times */)

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct {
	size_t length;
	char *text;
	bool error : 1;
} file_buffer;

file_buffer file_buffer_alloc(const char *filename);
void file_buffer_free(const file_buffer file);


#ifdef __cplusplus
} //extern "C" {
#endif // __cplusplus

#endif // BLIB_FILE_H

#ifdef BLIB_IMPLEMENTATION

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

inline file_buffer file_buffer_alloc(const char *filename) {
	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		file_buffer ret;
		ret.error = true;
		return ret;
	}
	size_t alloc = BLIB_FILE_BUFFER_CHUNK_SIZE * BLIB_FILE_BUFFER_GROWTH;
	char *buf = (char *)malloc(alloc);
	size_t length = 0;
	while (!feof(file)) {
		if (alloc - length <= BLIB_FILE_BUFFER_CHUNK_SIZE + 1) {
			alloc += BLIB_FILE_BUFFER_CHUNK_SIZE;
			alloc *= BLIB_FILE_BUFFER_GROWTH;
			buf = (char *)realloc((void *)buf, alloc);
		}
		int got = fread((void *)&buf[length], 1, BLIB_FILE_BUFFER_CHUNK_SIZE, file);
		length += got;
		if (got != BLIB_FILE_BUFFER_CHUNK_SIZE) {
			break;
		}
	}
	buf[length] = '\0';
	fclose(file);
	file_buffer ret;
	ret.text = buf;
	ret.length = length;
	ret.error = false;
	return ret;
}

inline void file_buffer_free(const file_buffer file) { free(file.text); }

#ifdef __cplusplus
} //extern "C" {
#endif // __cplusplus

#endif // BLIB_IMPLEMENTATION
