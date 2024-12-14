###############################################################################
#| Lite-Engine build system                                                  |#
#|                                                                           |#
#| To build a linux binary:                                                  |#
#|    run: make -Bj linux                                                    |#
#|                                                                           |#
#| To build an Apple MacOS binary                                            |#
#|    run: make -Bj macos                                                    |#
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
C = clang

CLANG_CFLAGS_DEBUG := -g3 -fsanitize=address -Wall -Wextra -Wpedantic -std=c99 -ferror-limit=15
CLANG_CFLAGS_RELEASE := -03 -flto
CLANG_CFLAGS := ${CLANG_CFLAGS_DEBUG}

LIBS_LINUX := -lglfw -lGL -lm
LIBS_MACOS := -lglfw -lm -framework Cocoa -framework IOKit -framework OpenGL

linux: build_directory linux_glad 
	${C} ${SOURCE} ${OBJECT} ${INCLUDE} ${LIBS_LINUX} ${CLANG_CFLAGS} -o build/lite_engine_linux
	./build/lite_engine_linux

linux_glad:
	${C} -c dep/glad.c -o build/glad.o -Idep

macos: build_directory macos_glad
	${C} ${SOURCE} ${INCLUDE} ${LIBS_MACOS} ${CLANG_CFLAGS} -o build/lite_engine_macos
	./build/lite_engine_macos

macos_glad:
	${C} -c dep/glad.c -o build/glad.o -Idep

build_directory:
	mkdir -p build
