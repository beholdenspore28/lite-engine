SRC	            :=  src/*.c
OBJ	            :=  build/*.o
INC	            := -Isrc -Idep -Idep/glad/include
OUT	            := -o build/lite_engine

C	              :=  clang

CFLAGS_DEBUG	  := -g3 -fsanitize=address
CFLAGS_RELEASE	:= -O3 -flto

CFLAGS		      := -Wall           \
								   -Wextra         \
								   -Wpedantic      \
								   -Werror         \
								   -std=c99        \
								   ${CFLAGS_DEBUG} \

LIBS := -lm -lrt -lglfw -lGL

build: build_directory glad
	${C} ${SRC} ${OBJ} ${INC} ${LIBS} ${CFLAGS} ${OUT}
	./build/lite_engine

glad:
	${C} -c -Idep/glad/include dep/glad/src/gl.c -o build/glad.o

build_directory:
	mkdir -p build
