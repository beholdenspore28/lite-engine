SRC             =  $(wildcard src/*.c)
OBJ             =  $(patsubst src/%.c, build/%.o, $(SRC))
INC             = -Isrc -Idep -Idep/glad/include
BUILD_DIR       =  build
GCC_ANALYZER    =  gcc   -fanalyzer
CLANG_ANALYZER  =  clang -fsanitize=address,undefined

CC              = $(CLANG_ANALYZER)

CFLAGS_DEBUG    = -g3 
CFLAGS_RELEASE  = -O3 -flto

CFLAGS          = -Wall \
									-Wextra \
									-Wpedantic \
									-std=c11 \
									$(CFLAGS_DEBUG) \

LIBS := -lglfw -lm -lopenal -lalut
LIBS_WINDOWS := -Ldep -lglfw3 -lgdi32 -lopengl32

LIBRARY = $(BUILD_DIR)/lite-engine.a
GLAD = $(BUILD_DIR)/glad.o

all: $(BUILD_DIR) $(OBJ) $(LIBRARY)

$(LIBRARY): $(OBJ) $(GLAD)
	ar rcs $@ $^

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

build/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(INC)

$(GLAD):
	$(CC) $(CFLAGS) -c dep/glad/src/gl.c -o $(BUILD_DIR)/glad.o -Idep/glad/include
