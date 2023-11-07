#include "blib_file.h"

void blib_fileBuffer_close(blib_fileBuffer_t file){
	free(file.text);
}

blib_fileBuffer_t blib_fileBuffer_read(const char *filename){
	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		blib_fileBuffer_t ret;
		ret.error = true;
		return ret;
	}
	size_t alloc = BLIB_FILE_BUFFER_CHUNK_SIZE * BLIB_FILE_BUFFER_GROWTH;
	char *buf = (char *) malloc(alloc);
	size_t len = 0;
	while (!feof(file)){
		if (alloc - len >= BLIB_FILE_BUFFER_CHUNK_SIZE + 1){
			alloc += BLIB_FILE_BUFFER_CHUNK_SIZE;
			alloc *= BLIB_FILE_BUFFER_GROWTH;
			buf = (char *) realloc((void *) buf, alloc);
		}
		int got = fread((void *) &buf[len], 1, BLIB_FILE_BUFFER_CHUNK_SIZE, file);
		len += got;
		if (got != BLIB_FILE_BUFFER_CHUNK_SIZE){
			break;
		}
	}
	buf[len] = '\0';
	fclose(file);
	blib_fileBuffer_t ret;
	ret.text = buf;
	ret.len = len;
	ret.error = false;
	return ret;
}
