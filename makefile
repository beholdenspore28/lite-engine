SRCFILES != find src -name '*.c'
BLIBSRCFILES != find blib/src -name '*.c'
SRCFILES += ${BLIBSRCFILES}

INCDIR := -Isrc -Iblib/src
LIBS := -lSDL2 -ldl -lm
OBJFILES := ${SRCFILES:%.c=build/obj/%.o} 

MODE ?= DEBUG
CC := gcc

LDFLAGS := -Wall -Wno-missing-braces -std=c11
CFLAGS := -Wall -Wno-missing-braces -std=c11
LDFLAGS_DEBUG := -g3
CFLAGS_DEBUG := -g3
LDFLAGS_RELEASE := -flto
CFLAGS_RELEASE := -O2 -flto
LDFLAGS += ${LDFLAGS_${MODE}}
CFLAGS += ${CFLAGS_${MODE}}

default: run

clearscreen:
	clear

run: build/bin/test
	./build/bin/test

build/bin/test: ${OBJFILES};
	@mkdir -p build/bin
	${CC} ${OBJFILES} ${INCDIR} ${LIBS} ${LDFLAGS} -o build/bin/test

${OBJFILES}: ${@:build/obj/%.o=%.c}
	@mkdir -p ${dir ${@}}
	${CC} -c ${@:build/obj/%.o=%.c} ${CFLAGS} ${INCDIR} -o ${@}

clean: 
	rm build/bin/test

