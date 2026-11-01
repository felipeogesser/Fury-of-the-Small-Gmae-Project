CC = gcc

CFLAGS_RELEASE = -O2 -march=native

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
	ai_order.c \
	animation.c \
	aos_soa_transformer.c \
	baking.c \
	battalion.c \
	battalion_policy.c \
	battalion_table.c \
	battalion_table_span.c \
	battle_outcome.c \
	battle_step.c \
	battlefield.c \
	battlefield_grid.c \
	battleplan.c \
	battleplan_grid.c \
	camera.c \
	collision.c \
	combat.c \
	contract.c \
	deployment_loader.c \
	engine.c \
	engine_cleanup.c \
	fatigue.c \
	fatigue_policy.c \
	field_entry.c \
	file_io.c \
	flank.c \
	formation.c \
	game_clock.c \
	game_loop.c \
	game_shutdown.c \
	game_state.c \
	general.c \
	general_table.c \
	health_pool.c \
	ini_parser.c \
	init.c \
	inventory.c \
	json_parser.c \
	letter.c \
	load_armies.c \
	mailbag.c \
	mailbox.c \
	mailroom.c \
	main.c \
	main_menu.c \
	maps.c \
	memory_arena.c \
	morale.c \
	morale_policy.c \
	peripherals.c \
	player.c \
	player_collision.c \
	position.c \
	projectile.c \
	python_process.c \
	quadrant.c \
	scene_handler.c \
	scene_registry.c \
	shared_memory_control.c \
	shared_memory_orders.c \
	spreadsheet.c \
	sprites.c \
	steering.c \
	table.c \
	threat_policy.c \
	type_tables.c \
	unit.c \
	unit_collision.c \
	unit_table.c \
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