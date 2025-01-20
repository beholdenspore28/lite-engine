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

SRC	            :=  src/*.c
OBJ	            :=  build/*.o
INC	            := -Isrc -Idep -Idep/glad/include
OUT	            := -o build/lite_engine

C	              :=  gcc

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
LIBS_X11 := -lm -lrt -lX11

X11: build_directory glx 
	${C} ${SRC} ${OBJ} ${INC} ${LIBS_X11} ${CFLAGS} ${OUT}
	./build/lite_engine

# FREE_BSD BUILD
# TODO free bsd build may not require linking to -lGL
FREE_BSD_LIBS := -L/usr/local/lib -I/usr/local/include -lGL -lm -lrt

free_bsd: build_directory glx 
	${C} ${SRC} ${OBJ} ${INC} ${FREE_BSD_LIBS} ${CFLAGS} ${OUT}
	./build/lite_engine

glx:
	${C} -c dep/glad/src/gl.c  -o build/gl.o  ${INC} ${CFLAGS}
	${C} -c dep/glad/src/glx.c -o build/glx.o ${INC} ${CFLAGS}

# WINDOWS MINGW BUILD
WINDOWS_MINGW_LIBS := -Lbuild -lopengl32

windows_mingw: build_directory wgl
	${C} ${SRC} ${OBJ} ${INC} ${WINDOWS_MINGW_LIBS} ${CFLAGS} ${OUT}
	./build/lite_engine

wgl:
	${C} -c dep/glad/src/gl.c  -o build/gl.o  ${INC} ${CFLAGS}
	${C} -c dep/glad/src/wgl.c -o build/wgl.o ${INC} ${CFLAGS}


build_directory:
	mkdir -p build
