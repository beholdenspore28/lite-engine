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

#ifndef BLIB_H
#define BLIB_H

#ifdef __cplusplus
extern "C" {
#endif //ifdef __cplusplus

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef uint8_t bool;
enum { false, true };

typedef void* void_ptr;
typedef const void* const_void_ptr;

#define DECLARE_LIST(type)\
typedef struct {\
	size_t capacity;\
	size_t length;\
	type *array;\
} list_##type;\
list_##type list_##type##_alloc(void);\
void list_##type##_free(list_##type *list);\
void list_##type##_add(list_##type *l, type value);\
void list_##type##_remove(list_##type *l);

#define DEFINE_LIST(type)\
	list_##type list_##type##_alloc(void) {\
		list_##type list;\
		memset(&list, 0, sizeof(list_##type));\
		list.array = malloc(sizeof(type));\
		return list;\
	}\
	\
	void list_##type##_free(list_##type *list) {\
		free(list->array);\
		memset(list, 0, sizeof(list_##type));\
	}\
	\
	void list_##type##_add(list_##type *l, type value) {\
		if (l->length >= l->capacity) {\
			l->capacity = l->length * 2 + 1;\
			l->array = realloc(l->array, sizeof(type) * l->capacity);\
		}\
		l->array[l->length] = value;\
		l->length++;\
	}\
	\
	int list_##type##_contains(list_##type *l, type* item) {\
		for (size_t i = 0; i < l->length; i++) {\
			if (memcmp(&l->array[i],item,sizeof(type)))\
				return 1;\
		}\
		return 0;\
	}\
	\
	void list_##type##_remove(list_##type *l) { l->length--; }

#define DECLARE_LIST_ALIAS(baseType, targetType)\
	typedef list_##baseType list_##targetType;\
	list_##targetType list_##targetType##_alloc(void);\
	void list_##targetType##_free(list_##targetType *list);\
	void list_##targetType##_add(list_##targetType *list, targetType value);\
	void list_##targetType##_remove(list_##targetType *list);

#define DEFINE_LIST_ALIAS(baseType, targetType)\
	list_##targetType list_##targetType##_alloc(void) {\
		return list_##baseType##_alloc();\
	}\
	void list_##targetType##_free(list_##targetType *list) {\
		list_##baseType##_free(list);\
	}\
	void list_##targetType##_add(list_##targetType *list, targetType value) {\
		list_##baseType##_add(list, *(targetType *)&value);\
	}\
	void list_##targetType##_remove(list_##targetType *list) {\
		list_##baseType##_remove(list);\
	}

DECLARE_LIST(void_ptr)
DECLARE_LIST(const_void_ptr)
DECLARE_LIST(char)
DECLARE_LIST(float)
DECLARE_LIST(double)

DECLARE_LIST(uint8_t)
DECLARE_LIST(uint16_t)
DECLARE_LIST(uint32_t)
DECLARE_LIST(uint64_t)
DECLARE_LIST(size_t)

DECLARE_LIST_ALIAS(uint8_t, int8_t)
DECLARE_LIST_ALIAS(uint16_t, int16_t)
DECLARE_LIST_ALIAS(uint32_t, int32_t)
DECLARE_LIST_ALIAS(uint64_t, int64_t)

#ifdef __cplusplus
} // extern "C" {
#endif //ifdef __cplusplus

#endif // BLIB_H

#ifdef BLIB_IMPLEMENTATION

#ifdef __cplusplus
extern "C" {
#endif //ifdef __cplusplus

	DEFINE_LIST(void_ptr)
	DEFINE_LIST(const_void_ptr)
	DEFINE_LIST(char)
	DEFINE_LIST(float)
	DEFINE_LIST(double)
	
	DEFINE_LIST(uint8_t)
	DEFINE_LIST(uint16_t)
	DEFINE_LIST(uint32_t)
	DEFINE_LIST(uint64_t)
	DEFINE_LIST(size_t)
	
	DEFINE_LIST_ALIAS(uint8_t, int8_t)
	DEFINE_LIST_ALIAS(uint16_t, int16_t)
	DEFINE_LIST_ALIAS(uint32_t, int32_t)
	DEFINE_LIST_ALIAS(uint64_t, int64_t)

#ifdef __cplusplus
} // extern "C" {
#endif //ifdef __cplusplus

#endif // BLIB_IMPLEMENTATION

