SRCFILES := \
	src/*.c \

OBJFILES := \
	build/obj/*.o

INCDIR :=	-Isrc 					\
		-Idep					\
		-I/usr/include/freetype2 		\
		-I/usr/include/libpng16 		\
		-I/usr/include/harfbuzz 		\
		-I/usr/include/glib-2.0 		\
		-I/usr/lib/glib-2.0/include 		\
		-I/usr/include/sysprof-6 -pthread

C = clang
OUT := build/bin/lite-engine

OPT_DEBUG :=	-g3 -fsanitize=address
OPT_SMALL :=	-Oz -flto
OPT_RELEASE :=	-flto -O3 
OPT_ :=		${OPT_DEBUG} 
OPT :=		${OPT_${MODE}}
CFLAGS +=	${OPT} -Wall -Wextra -Wpedantic -std=c99 -ferror-limit=15


LIBS := -lglfw -lGL -lm -lfreetype

default: build_lite_engine

build_lite_engine:	build_dir \
			build_glad
	${C} ${OBJFILES} ${SRCFILES} ${INCDIR} ${LIBS} ${CFLAGS} -o ${OUT} 
	./${OUT}

build_glad: build_dir
	${C} -c dep/glad.c ${CFLAGS} -Idep -o build/obj/glad.o

build_dir:
	mkdir -p build/bin
	mkdir -p build/obj
