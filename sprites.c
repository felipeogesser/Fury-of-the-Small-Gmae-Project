#include "sprites.h"
#include "engine_internal.h"
#include "memory_arena.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <SDL2/SDL.h>
#include <SDL_image.h>

int count_pngs_in_folder(const char *path);
int count_pngs_in_folder(const char *path) {
    DIR *dir = opendir(path);
    if (!dir) {
        return -1;
    }

    int count = 0;
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
        const char *name = entry->d_name;
        size_t len = strlen(name);

        if (len > 4 && strcmp(name + len - 4, ".png") == 0) {
            count++;
        }
    }

    closedir(dir);
    return count;
}

typedef struct SpritesPath {
    char file_path[512];
} SpritesPath;

signed int fill_pngs_from_folder(const char *folder, SpritesPath *out, signed int start_index);
signed int fill_pngs_from_folder(const char *folder, SpritesPath *out, signed int start_index) {
    DIR *dir = opendir(folder);
    if (!dir) return start_index;

    struct dirent *entry;
    signed int i = start_index;

    while ((entry = readdir(dir)) != NULL) {
        const char *name = entry->d_name;
        size_t len = strlen(name);
        if (len > 4 && strcmp(name + len - 4, ".png") == 0) {
            snprintf(out[i].file_path, sizeof(out[i].file_path), "%s/%s", folder, name);
            i++;
        }
    }

    closedir(dir);
    return i;
}

unsigned int parse_frame_count_from_path(const char *path);
unsigned int parse_frame_count_from_path(const char *path) {
    const char *filename = strrchr(path, '/');
    filename = filename ? filename + 1 : path;

    char *endptr;
    unsigned long frames = strtoul(filename, &endptr, 10);

    if (endptr == filename) {
        return 0;
    }

    return (unsigned int)frames;
}

typedef struct Sprite {
    unsigned int frames_count;
    SDL_Texture *texture;
} Sprite;

typedef struct SpritePack {
    Sprite *sprite;
    unsigned int sprite_count;
} SpritePack;

SpritePack sprite_pack = {0};

void load_sprites_into_memory(void) {

    signed int count = 0;
    count += count_pngs_in_folder("assets/sprites/Archer");
    count += count_pngs_in_folder("assets/sprites/Lancer");
    count += count_pngs_in_folder("assets/sprites/Warrior");

    SpritesPath *sprites_string = calloc((size_t)count, sizeof(SpritesPath));
    if (!sprites_string) {
        fprintf(stderr, "Alloc for sprites path failed.\n");
        exit(EXIT_FAILURE);
    }

    signed int idx = 0;
    idx = fill_pngs_from_folder("assets/sprites/Archer", sprites_string, idx);
    idx = fill_pngs_from_folder("assets/sprites/Lancer", sprites_string, idx);
    idx = fill_pngs_from_folder("assets/sprites/Warrior", sprites_string, idx);

    sprite_pack.sprite_count = (unsigned int)idx;

    SDL_Renderer *renderer = engine.renderer;
    sprite_pack.sprite = (Sprite *)memory_arena_push(sizeof(Sprite) * (size_t)idx, _Alignof(Sprite));
    for (int i = 0; i < count; i++) {
        SDL_RWops *rw = SDL_RWFromFile(sprites_string[i].file_path, "rb");
        SDL_Surface *surface = IMG_Load_RW(rw, 1);
        if (!surface) {
            fprintf(stderr, "Texture load failed: %s\n", IMG_GetError());
            exit(EXIT_FAILURE);
        }

        unsigned int frames = parse_frame_count_from_path(sprites_string[i].file_path);
        if (frames == 0) {
            fprintf(stderr, "Sprite '%s' missing frame-count prefix.\n", sprites_string[i].file_path);
            exit(EXIT_FAILURE);
        }

        sprite_pack.sprite[i].texture = SDL_CreateTextureFromSurface(renderer, surface);
        sprite_pack.sprite[i].frames_count = frames;
        SDL_FreeSurface(surface);
    }

    memset(sprites_string, 0, sizeof(SpritesPath) * (size_t)idx);
    free(sprites_string);

}