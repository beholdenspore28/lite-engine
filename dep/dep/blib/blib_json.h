#ifndef BLIB_JSON_H
#define BLIB_JSON_H

#ifdef __cplusplus
extern "C" {
#endif //ifdef __cplusplus

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>

#include "blib_file.h"

enum {
	JSON_VALUE_STRING,
	JSON_VALUE_NUMBER,
	JSON_VALUE_OBJECT,
	JSON_VALUE_ARRAY,
	JSON_VALUE_BOOLEAN,
	JSON_VALUE_NULL,
}; typedef uint8_t json_value_type;

typedef struct json_value {
	json_value_type type;
	list_char string;
	double number;
	uint8_t boolean;
	uint8_t is_null;
	list_void_ptr children;
} json_value;

#ifdef __cplusplus
} // extern "C" {
#endif //ifdef __cplusplus

#endif // BLIB_JSON_H

#ifdef BLIB_IMPLEMENTATION

#ifdef __cplusplus
extern "C" {
#endif //ifdef __cplusplus

void json_free(json_value *json) {
	if (json->type == JSON_VALUE_STRING)
		list_char_free(&json->string);
	if (json->type == JSON_VALUE_OBJECT) {
		for(size_t i = 0; i < json->children.length; i++) {
			json_value *child = json->children.array[i];
			json_free(child);
		}
		list_void_ptr_free(&json->children);
	}
	free(json);
}

void indent(int depth) {
	for(int i = 0; i < depth; i++)
		printf("    ");
}

void json_print(json_value *json) {
	static int depth = -1;
	depth++;
	putchar('\n');
	for(size_t i = 0; i < json->children.length; i++) {
		json_value *child = json->children.array[i];
		indent(depth);
		printf("child of type %d at %p ", child->type, (void*)child);
		switch(child->type) {
			case JSON_VALUE_OBJECT: {
				json_print(child);
			} break;
			case JSON_VALUE_STRING: {
				printf("string \"%s\"", child->string.array);
			} break;
			case JSON_VALUE_NUMBER: {
				printf("number %lf", child->number);
			} break;
			case JSON_VALUE_BOOLEAN: {
				printf("boolean %d", child->boolean);
			} break;
			case JSON_VALUE_NULL: {
				printf("is_null %d", child->is_null);
			} break;
		};
		putchar('\n');
	}
	indent(depth); puts("END");
	depth=0;
}

#define is_number(character) (character <= '9' && character >= '0')

json_value *json_parse(char* c, const size_t string_length) {
	json_value *json = malloc(sizeof(json_value));
	json->children = list_void_ptr_alloc();
	json->type = JSON_VALUE_OBJECT;
	while(c < c+string_length) {
		if (*c == '\0')
			break;
		if (is_number(*c)) {
			double n;
			sscanf(c, "%lf", &n);
			while(is_number(*(++c)) || *(++c)=='.'){}
			json_value *child = malloc(sizeof(json_value));
			child->type = JSON_VALUE_NUMBER;
			child->number = n;
			list_void_ptr_add(&json->children, child);
		}
		switch (*c) {
			case '{': {
				++c;
				json_value* child = json_parse(c, string_length);
				list_void_ptr_add(&json->children, (void*)child);
			} break;
			case '}': {
				++c;
				return json;
			} break;
			case '\"': {
				list_char string = list_char_alloc();
				while(*(++c)!='\"'){
					list_char_add(&string, *c);
				}
				list_char_add(&string, '\0');
				json_value *child = malloc(sizeof(json_value));
				child->type = JSON_VALUE_STRING;
				child->string = string;
				list_void_ptr_add(&json->children, child);
			} break;
			case 't': {
				if( *(++c)=='r' &&
					*(++c)=='u' &&
					*(++c)=='e') {
					json_value *child = malloc(sizeof(json_value));
					child->type = JSON_VALUE_BOOLEAN;
					child->boolean = true;
					list_void_ptr_add(&json->children, child);
				}
			} break;
			case 'f': {
				if( *(++c)=='a' &&
					*(++c)=='l' &&
					*(++c)=='s' &&
					*(++c)=='e') {
					json_value *child = malloc(sizeof(json_value));
					child->type = JSON_VALUE_BOOLEAN;
					child->boolean = false;
					list_void_ptr_add(&json->children, child);
				}
			} break;
			case 'n': {
				if( *(++c)=='u' &&
					*(++c)=='l' &&
					*(++c)=='l') {
					json_value *child = malloc(sizeof(json_value));
					child->type = JSON_VALUE_NULL;
					child->is_null = true;
					list_void_ptr_add(&json->children, child);
				}
			} break;
		}
		c++;
	}
	return json;
}

json_value *json_read(const char *path_to_file) {
	file_buffer fb = file_buffer_alloc(path_to_file);
	if (fb.error)
		fprintf(stderr, "failed to load file %s\n", path_to_file);
	json_value *json = json_parse(fb.text, fb.length);
	file_buffer_free(fb);
	return json;
}

#ifdef __cplusplus
} // extern "C" {
#endif //ifdef __cplusplus

#endif //#ifdef BLIB_IMPLEMENTATION
