###############################################################################
#| Lite-Engine build system                                                  |#
#|                                                                           |#
#| To build a linux binary:                                                  |#
#|    run: make -B linux                                                    |#
#|                                                                           |#
#| To build a FreeBSD binary:                                                |#
#|    run: make -B free_bsd                                                 |#
#|                                                                           |#
#| To build an Apple MacOS binary                                            |#
#|    run: make -B macos                                                    |#
#|                                                                           |#
#| If the engine is built successfully, executables/binaries are stored in   |# 
#| the build directory                                                       |#
#|                                                                           |#
#| If you are unable to build the project, please feel free to submit an     |#
#| issue or pull request to this repository:                                 |#
#| https://www.github.com/beholdenspore28/lite-engine                        |#
#|                                                                           |#
###############################################################################

SOURCE := src/*.c
OBJECT := build/*.o
INCLUDE := -Isrc -Idep
C = gcc

CFLAGS_DEBUG := -g3 -fsanitize=address -Wall -Wextra -Wpedantic -std=gnu99 -ferror-limit=15
CFLAGS_RELEASE := -03 -flto
CFLAGS_:= ${CFLAGS_DEBUG}

# LINUX BUILD
LIBS_LINUX := -lGL -lm -lrt

linux: build_directory glad 
	${C} ${SOURCE} ${OBJECT} ${INCLUDE} ${LIBS_LINUX} ${CFLAGS} -o build/lite_engine_linux
	./build/lite_engine_linux

# WINDOWS MINGW BUILD
MINGW_LIBS := -Lbuild -lopengl32

mingw: build_directory glad
	${C} ${SOURCE} ${OBJECT} ${INCLUDE} ${MINGW_LIBS} ${CFLAGS} -o build/lite_engine_mingw
	./build/lite_engine_mingw

# FREE_BSD BUILD
LIBS_FREE_BSD := -L/usr/local/lib -I/usr/local/include -lGL -lm -lrt

free_bsd: build_directory glad 
	${C} ${SOURCE} ${OBJECT} ${INCLUDE} ${LIBS_FREE_BSD} ${CFLAGS} -o build/lite_engine_free_bsd
	./build/lite_engine_free_bsd

#LIBS_MACOS := -lm -framework Cocoa -framework IOKit -framework OpenGL
#
#macos: build_directory glad
#	${C} ${SOURCE} ${INCLUDE} ${LIBS_MACOS} ${CFLAGS} -o build/lite_engine_macos
#	./build/lite_engine_macos
#
#macos_glad:
#	${C} -c dep/glad.c -o build/glad.o -Idep

glad:
	${C} -c dep/glad.c -o build/glad.o -Idep

build_directory:
	mkdir -p build
