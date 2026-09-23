# Homework 4 - OpenGL and GLFW (no GLU, GLUT, or CSCIx229)
EXE=hw4
CC=gcc
CFLAGS=-O3 -Wall -Wextra -std=c99
GLFW_CFLAGS=$(shell pkg-config --cflags glfw3 2>/dev/null)
GLFW_LIBS=$(shell pkg-config --libs glfw3 2>/dev/null || echo -lglfw)

ifeq "$(OS)" "Windows_NT"
LDLIBS=-lglfw3 -lopengl32 -lgdi32 -lm
else ifeq "$(shell uname)" "Darwin"
LDLIBS=$(GLFW_LIBS) -framework OpenGL -lm
else
LDLIBS=$(GLFW_LIBS) -lGL -lm
endif

.PHONY: all clean
all: $(EXE)

$(EXE): hw4.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

hw4.o: hw4.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(GLFW_CFLAGS) -c $< -o $@

clean:
	rm -f $(EXE) $(EXE).exe hw4.o
