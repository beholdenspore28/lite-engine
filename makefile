SRCFILES != find src -name '*.c'
BLIBSRCFILES != find blib/src -name '*.c'
SRCFILES += ${BLIBSRCFILES}

INCDIR := -Isrc -Iblib/src
LIBS := -lglfw -ldl -lm 
OBJFILES := ${SRCFILES:%.c=build/obj/%.o} 

CC ?= clang

SAN := -fsanitize=address -fno-optimize-sibling-calls

CFLAGS := -Wall -Wno-missing-braces -std=c11 -g3 -O0 ${SAN}
LDFLAGS := -Wall -Wno-missing-braces -std=c11 -g3 -O0 ${SAN}

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
	rm -r build/*
