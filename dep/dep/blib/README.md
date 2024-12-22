#blib
What is this and why have you created it?

  This project is a general purpose tool library for tasks I believe to be 
common in software development. blib is written in C99. I realise that no 
one asked for this and there is probably someone out there who has already done 
what I am trying to do here. The main goal for me is to learn the 
inner-workings of the code most people take for granted. This will allow me to 
completely understand all of the code that I use in my projects.

#How do I use it ?
Simply add the header files to one of your include directories and place the c files in your src directory.
You have to link to the standard math library for many of the libraries functions contained in b_math to work.

here is an example makefile that i use for most of my projects. (your mileage may vary)

```makefile
SRCFILES != find . -name '*.c'
INCDIR := -Isrc -Idep
CFLAGS := -Wall -Wextra -Werror -O2 -std=c99 -pedantic
LIBS := -lm #this links to the standard c math library.

build: build/bin
	clang ${SRCFILES} ${INCDIR} ${LIBS} ${CFLAGS} -o build/bin/game
	./build/bin/game
```

#Why is it called blib ? 
The name blib is a combination of my first initial and 'lib' which is short
for library. Also it sounds silly and squishy.
