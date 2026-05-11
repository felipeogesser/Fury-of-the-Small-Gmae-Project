CC = gcc

CFLAGS = -g -O0 -Wall -Wextra

SDLFLAGS = $(shell sdl2-config --cflags --libs)

SRC = \
	baking.c \
	calculateEntityQuadrant.c \
	engine.c \
	entities.c \
	gameLoop.c \
	gameState.c \
	grids.c \
	loadArmies.c \
	mainCopy.c \
	mapMaker.c \
	maps.c \
	peripherals.c \
	player.c \
	playerMapEdgeCollisionFunc.c \
	quadrant.c \
	renderer.c \
	setArmiesPosition.c \
	update_data.c \
	window.c \

OBJ = $(SRC:.c=.o)

OUT = game

all: $(OUT)

$(OUT): $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) -lm $(SDLFLAGS) -o $(OUT)

%.o: %.c
	$(CC) -c $< $(CFLAGS)

clean:
	rm -f $(OBJ) $(OUT)