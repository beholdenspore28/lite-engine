SRCFILES != find . -name '*.c'
INCDIR := -Isrc -Idep
CFLAGS := -Wall -Wextra -Werror -std=c11 -g3 -O0 
LIBS := -lglfw -lGL -lm 

build: build/bin
	gcc ${SRCFILES} ${INCDIR} ${LIBS} ${CFLAGS} -o build/bin/game
	./build/bin/game
