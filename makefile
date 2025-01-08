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
C = clang
E = emcc
# CLANG BUILD
CLANG_CFLAGS_DEBUG := -g3 -fsanitize=address -Wall -Wextra -Wpedantic -std=gnu99 -ferror-limit=15
CLANG_CFLAGS_RELEASE := -03 -flto
CLANG_CFLAGS_:= ${CLANG_CFLAGS_DEBUG}
E_FLAGS := -s USE_GLFW=3 -lglfw -s FULL_ES2=1
# LINUX BUILD

LIBS_LINUX := -lglfw -lGL -lm -lrt

linux: build_directory glad 
	${C} ${SOURCE} ${OBJECT} ${INCLUDE} ${LIBS_LINUX} ${CLANG_CFLAGS} -o build/lite_engine_linux
	./build/lite_engine_linux
#https://emscripten.org/docs/compiling/Dynamic-Linking.html#runtime-dynamic-linking-with-dlopen
web: build_directory web-glad
	emcc ${SOURCE} ${OBJECT} ${INCLUDE} ${LIBS_LINUX} ${CLANG_CFLAGS} -o build/web-lite_engine_linux.html ${E_FLAGS}
	python3 -m http.server 3000 
# FREE_BSD BUILD
LIBS_FREE_BSD := -L/usr/local/lib -I/usr/local/include -lglfw -lGL -lm -lrt

free_bsd: build_directory glad 
	${C} ${SOURCE} ${OBJECT} ${INCLUDE} ${LIBS_FREE_BSD} ${CLANG_CFLAGS} -o build/lite_engine_free_bsd
	./build/lite_engine_free_bsd

#LIBS_MACOS := -lglfw -lm -framework Cocoa -framework IOKit -framework OpenGL
#
#macos: build_directory glad
#	${C} ${SOURCE} ${INCLUDE} ${LIBS_MACOS} ${CLANG_CFLAGS} -o build/lite_engine_macos
#	./build/lite_engine_macos
#
#macos_glad:
#	${C} -c dep/glad.c -o build/glad.o -Idep
web-glad: 
#	 this builds:
	${E} -c dep/glad.c -o build/glad.wasm -Idep ${E_FLAGS}
glad:
	${C} -c dep/glad.c -o build/glad.o -Idep

build_directory:
	mkdir -p build
