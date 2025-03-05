SRC	            :=  src/*.c
OBJ	            :=  build/*.o
INC	            := -Isrc -Idep -Idep/glad/include
OUT	            := -o build/lite_engine

C	              :=  clang

CFLAGS_DEBUG	  := -g3 -fsanitize=address
CFLAGS_RELEASE	:= -O3 -flto

CFLAGS		      ?= -Wall        \
								-Wextra         \
								-Wpedantic      \
								-Werror         \
								-std=gnu99      \
								${CFLAGS_DEBUG} \

VALGRIND		    := valgrind	--leak-check=full \
						    --show-leak-kinds=all         \
						    --track-origins=yes           \
						    --verbose                     \

# LINUX X11 BUILD
LIBS_X11 := -lm -lrt -lglfw -lGL

build: build_directory glad
	${C} ${SRC} ${OBJ} ${INC} ${LIBS_X11} ${CFLAGS} ${OUT}
	./build/lite_engine

glad:
	${C} -c -Idep/glad/include dep/glad/src/gl.c -o build/glad.o

build_directory:
	mkdir -p build
