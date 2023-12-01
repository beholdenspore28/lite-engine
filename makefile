SRCFILES != find src -name '*.c'
BLIBSRCFILES != find blib/src -name '*.c'
SRCFILES += ${BLIBSRCFILES}

INCDIR := -Isrc -Iblib/src
LIBS := -lglfw -lGL -lm 
OBJFILES := ${SRCFILES:%.c=build/obj/%.o} 

CC ?= gcc

CFLAGS := -Wall -Wno-missing-braces -std=c11 -g3 -O0 -fsanitize=address -fno-optimize-sibling-calls
LDFLAGS := -Wall -Wno-missing-braces -std=c11 -g3 -O0 -fsanitize=address -fno-optimize-sibling-calls

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
