CC = gcc

CFLAGS_RELEASE = -O0

CFLAGS_DEBUG = -g3 -O0 \
-Wall -Wextra -Wpedantic \
-Wshadow \
-Wconversion \
-Wsign-conversion \
-Wcast-align \
-Wstrict-prototypes \
-Wmissing-prototypes \
-Wdouble-promotion \
-Wformat=2 \
-Wundef \
-Wnull-dereference \
-Wuninitialized \
-Warray-bounds \
-Wimplicit-fallthrough \
-Werror=return-type \
-fsanitize=address,undefined \
-fno-omit-frame-pointer \
-fno-common \
-fstack-protector-strong


SDLFLAGS = $(shell pkg-config --cflags --libs sdl2 SDL2_ttf)

SRC = \
	baking.c \
	calculateEntityQuadrant.c \
	engine.c \
	entities.c \
	gameLoop.c \
	gameState.c \
	grids.c \
	init.c \
	loadArmies.c \
	main.c \
	main_menu.c \
	mapMaker.c \
	maps.c \
	memory_arena.c \
	peripherals.c \
	player.c \
	playerMapEdgeCollisionFunc.c \
	quadrant.c \
	renderer.c \
	scene_handler.c \
	scene_registry.c \
	setArmiesPosition.c \
	shutdown.c \
	update_data.c \
	window.c \

OBJ = $(SRC:.c=.o)

OUT = game
OUT_DEBUG = game_debug

release: CFLAGS = $(CFLAGS_RELEASE)
release: $(OUT)

debug: CFLAGS = $(CFLAGS_DEBUG)
debug: $(OUT_DEBUG)

$(OUT): $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) $(SDLFLAGS) -lm -o $(OUT)

$(OUT_DEBUG): $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) $(SDLFLAGS) -lm -o $(OUT_DEBUG)

%.o: %.c
	$(CC) -c $< $(CFLAGS) $(SDLFLAGS)

clean:
	rm -f $(OBJ) $(OUT) $(OUT_DEBUG)