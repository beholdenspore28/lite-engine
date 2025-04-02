SRC             :=  src/*.c
OBJ             :=  build/*.o
INC             := -Isrc -Idep -Idep/glad/include
OUT             := -o build/lite_engine

GCC_ANALYZER    :=  gcc   -fanalyzer
CLANG_ANALYZER  :=  clang -fsanitize=address,undefined

CC              := ${CLANG_ANALYZER}

CFLAGS_DEBUG    := -g3 
CFLAGS_RELEASE  := -O3 -flto

CFLAGS          := -Wall           \
                   -Wextra         \
                   -Wpedantic      \
                   -std=c11        \
                   ${CFLAGS_DEBUG} \

LIBS := -lglfw -lm -lopenal -lalut

build: build_directory glad
	${CC} ${SRC} ${OBJ} ${INC} ${LIBS} ${CFLAGS} ${OUT}
	./build/lite_engine

LIBS_WINDOWS := -Ldep -lglfw3 -lgdi32 -lopengl32

build_windows: 
	${CC} ${SRC} ${OBJ} ${INC} ${LIBS_WINDOWS} ${CFLAGS} ${OUT}
	./build/lite_engine.exe

glad:
	${CC} -c -Idep/glad/include dep/glad/src/gl.c -o build/glad.o

build_directory:
	mkdir -p build
