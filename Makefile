CC = gcc

CFLAGS_RELEASE = -O0

CFLAGS_DEBUG = -g3 -O0 \
-Wall -Wextra -Wpedantic \
-Wshadow \
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

SDLFLAGS = $(shell pkg-config --cflags --libs sdl2 SDL2_ttf SDL2_image)

SRC = \
	animation.c \
	baking.c \
	battlefield.c \
	battleplan.c \
	camera.c \
	engine.c \
	entities.c \
	entity_collision.c \
	gameLoop.c \
	game_shutdown.c \
	game_state.c \
	general.c \
	grids.c \
	init.c \
	inventory.c \
	json_parser.c \
	load_armies.c \
	main.c \
	main_menu.c \
	mapMaker.c \
	maps.c \
	memory_arena.c \
	peripherals.c \
	player.c \
	player_collision.c \
	quadrant.c \
	scene_handler.c \
	scene_registry.c \
	set_armies_position.c \
	sprites.c \
	type_tables.c \
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