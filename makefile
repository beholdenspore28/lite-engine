SRCFILES != \
	find src -name '*.c' & \
	find dep/blib -name '*.c'

OBJFILES != \
	find build/obj -name '*.o'

INCDIR := -Isrc -Idep

C = clang
OUT := build/bin/lite-engine

OPT_DEBUG := -g3
OPT_SMALL := -Oz -flto
OPT_RELEASE := -flto -O3 
OPT_ := ${OPT_DEBUG} 
OPT := ${OPT_${MODE}}
CFLAGS += ${OPT} -Wall -Wextra -Wpedantic -std=c11

LIBS := -lglfw -lGL -lm 

lite-engine-run: lite-engine-build
	./${OUT}

lite-engine-gdb: lite-engine-build
	gdb ./${OUT}

lite-engine-build:
	mkdir -p build/bin
	${C} ${OBJFILES} ${SRCFILES} ${INCDIR} ${LIBS} ${CFLAGS} -o ${OUT} 

glad:
	mkdir -p build/obj
	${C} -c dep/glad.c ${CFLAGS} -Idep -o build/obj/glad.o

