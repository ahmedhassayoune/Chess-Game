CC = gcc
CPPFLAGS = 
CFLAGS = -g -Wall -Wextra -std=c99 -O1 `pkg-config --cflags sdl2`
LDFLAGS =
LDLIBS = -lm `pkg-config --libs sdl2` -lSDL2_image
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
EXEC = main

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@ $(LDLIBS)

$(EXEC) : $(OBJ)
	$(CC) $(CFLAGS) $^ -o $(EXEC) $(LDLIBS)

all: $(EXEC)

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} $(EXEC)
