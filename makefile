SRC	            :=  src/*.c
OBJ	            :=  build/*.o
INC	            := -Isrc -Idep -Idep/glad/include
OUT	            := -o build/lite_engine

C	              :=  gcc

CFLAGS_DEBUG	  := -g3 
CFLAGS_RELEASE	:= -O3 -flto

CFLAGS		      := -Wall           \
								   -Wextra         \
								   -Wpedantic      \
								   -Werror         \
								   -std=c99        \
								   ${CFLAGS_DEBUG} \

LIBS := -lglfw3

build: build_directory glad
	${C} ${SRC} ${OBJ} ${INC} ${LIBS} ${CFLAGS} ${OUT}
	./build/lite_engine

LIBS_WINDOWS := -Ldep -lglfw3 -lgdi32 -lopengl32

build_windows: 
	${C} ${SRC} ${OBJ} ${INC} ${LIBS_WINDOWS} ${CFLAGS} ${OUT}
	./build/lite_engine.exe

glad:
	${C} -c -Idep/glad/include dep/glad/src/gl.c -o build/glad.o

build_directory:
	mkdir -p build
