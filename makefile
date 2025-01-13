###############################################################################
#| Lite-Engine build system                                                  |#
#|                                                                           |#
#| To build a linux binary:                                                  |#
#|    run: make -B linux                                                     |#
#|                                                                           |#
#| To build a FreeBSD binary:                                                |#
#|    run: make -B free_bsd                                                  |#
#|                                                                           |#
#| If the engine is built successfully, executables/binaries are stored in   |# 
#| the build directory                                                       |#
#|                                                                           |#
#| If you are unable to build the project, please feel free to submit an     |#
#| issue or pull request to this repository:                                 |#
#| https://www.github.com/beholdenspore28/lite-engine                        |#
#|                                                                           |#
###############################################################################

OUT	:= -o lite_engine
SRC	:= src/*.c
OBJ	:= build/*.o
INC	:= -Isrc -Idep -Idep/glad/include
C	:= gcc

CFLAGS_DEBUG	:= -g3 -fsanitize=address -Wall -Wextra -Wpedantic -std=gnu99
CFLAGS_RELEASE	:= -03 -flto
CFLAGS		?= ${CFLAGS_DEBUG}

# LINUX BUILD
LIBS_LINUX := -lm -lrt

linux: build_directory glx 
	${C} ${SRC} ${OBJ} ${INC} ${LIBS_LINUX} ${CFLAGS} ${OUT}_linux
	./build/lite_engine_linux

# WINDOWS MINGW BUILD
WINDOWS_MINGW_LIBS := -Lbuild -lopengl32

windows_mingw: build_directory wgl
	${C} ${SRC} ${OBJ} ${INC} ${WINDOWS_MINGW_LIBS} ${CFLAGS} ${OUT}_windows_mingw
	./build/lite_engine_windows_mingw.exe

# FREE_BSD BUILD
# TODO free bsd build may not require linking to -lGL
FREE_BSD_LIBS := -L/usr/local/lib -I/usr/local/include -lGL -lm -lrt

free_bsd: build_directory glx 
	${C} ${SRC} ${OBJ} ${INC} ${FREE_BSD_LIBS} ${CFLAGS} ${OUT}_free_bsd
	./build/lite_engine_free_bsd

glx:
	${C} -c dep/glad/src/gl.c  -o build/gl.o  ${INC}
	${C} -c dep/glad/src/glx.c -o build/glx.o ${INC}

wgl:
	${C} -c dep/glad/src/gl.c  -o build/gl.o  ${INC}
	${C} -c dep/glad/src/wgl.c -o build/wgl.o ${INC}

build_directory:
	mkdir -p build
