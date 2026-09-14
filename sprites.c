#include "sprites.h"
#include "sprites_internal.h"
#include "animation_types.h"
#include "engine_internal.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <SDL2/SDL.h>
#include <SDL_image.h>


typedef struct SpritesPath {
    unsigned int sprite_index;
    char file_path[512];
} SpritesPath;

// private prototypes
static signed int count_pngs_in_folder(const char *path);
static signed int fill_pngs_from_folder(const char *folder, SpritesPath *out, signed int start_index, unsigned int sprite_index);
static unsigned int parse_frame_count_from_path(const char *path);
static signed int stristr_found(const char *haystack, const char *needle);
static signed int parse_animation_from_path(const char *path);

static signed int count_pngs_in_folder(const char *path) {
    DIR *dir = opendir(path);
    if (!dir) {
        return -1;
    }

    signed int count = 0;
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

static signed int fill_pngs_from_folder(const char *folder, SpritesPath *out, signed int start_index, unsigned int sprite_index) {
    DIR *dir = opendir(folder);
    if (!dir) return start_index;

    struct dirent *entry;
    signed int i = start_index;

    while ((entry = readdir(dir)) != NULL) {
        const char *name = entry->d_name;
        size_t len = strlen(name);
        if (len > 4 && strcmp(name + len - 4, ".png") == 0) {
            snprintf(out[i].file_path, sizeof(out[i].file_path), "%s/%s", folder, name);
            out[i].sprite_index = sprite_index; 
            i++;
        }
    }

    closedir(dir);
    return i;
}

static unsigned int parse_frame_count_from_path(const char *path) {
    const char *filename = strrchr(path, '/');
    filename = filename ? filename + 1 : path;

    char *endptr;
    unsigned long frames = strtoul(filename, &endptr, 10);

    if (endptr == filename) {
        return 0;
    }

    return (unsigned int)frames;
}

static signed int stristr_found(const char *haystack, const char *needle) {
    size_t hlen = strlen(haystack);
    size_t nlen = strlen(needle);
    if (nlen > hlen) return 0;

    for (size_t i = 0; i <= hlen - nlen; i++) {
        size_t j = 0;
        while (j < nlen && tolower((unsigned char)haystack[i + j]) == tolower((unsigned char)needle[j])) {
            j++;
        }
        if (j == nlen) return 1;
    }
    return 0;
}

static signed int parse_animation_from_path(const char *path) {
    if (stristr_found(path, "Idle"))   return IDLE;
    if (stristr_found(path, "Run"))    return RUN;
    if (stristr_found(path, "Attack")) return ATTACK;
    if (stristr_found(path, "Guard"))  return GUARD;
    return -1;
}

static SpritePack sprite_pack = {0};

void load_sprites_into_memory(void) {

    signed int count = 0;
    count += count_pngs_in_folder("assets/sprites/Archer");
    count += count_pngs_in_folder("assets/sprites/Lancer");
    count += count_pngs_in_folder("assets/sprites/Warrior");
    count += count_pngs_in_folder("assets/sprites/rafa");

    SpritesPath *sprites_string = calloc((size_t)count, sizeof(SpritesPath));
    if (!sprites_string) {
        fprintf(stderr, "Alloc for sprites path failed.\n");
        exit(EXIT_FAILURE);
    }

    signed int idx = 0;
    idx = fill_pngs_from_folder("assets/sprites/Archer",  sprites_string, idx, ARCHER);
    idx = fill_pngs_from_folder("assets/sprites/Lancer",  sprites_string, idx, LANCER);
    idx = fill_pngs_from_folder("assets/sprites/Warrior", sprites_string, idx, WARRIOR);
    idx = fill_pngs_from_folder("assets/sprites/rafa", sprites_string, idx, RAFA);

    sprite_pack.sprite_count = (unsigned int)idx;

    SDL_Renderer *renderer = engine.renderer;
    engine.sprite_pack = &sprite_pack;
    for (signed int k = 0; k < idx; k++) {

        SDL_RWops *rw = SDL_RWFromFile(sprites_string[k].file_path, "rb");
        SDL_Surface *surface = IMG_Load_RW(rw, 1);
        if (!surface) {
            fprintf(stderr, "Texture load failed: %s\n", IMG_GetError());
            exit(EXIT_FAILURE);
        }

        unsigned int frames = parse_frame_count_from_path(sprites_string[k].file_path);
        if (frames == 0) {
            fprintf(stderr, "Sprite '%s' missing frame-count prefix.\n", sprites_string[k].file_path);
            exit(EXIT_FAILURE);
        }

        signed int anim = parse_animation_from_path(sprites_string[k].file_path);
        if (anim < 0) {
            fprintf(stderr, "Sprite '%s' has unrecognized animation name.\n", sprites_string[k].file_path);
            exit(EXIT_FAILURE);
        }

        unsigned int sprite = sprites_string[k].sprite_index;
        unsigned int animation = (unsigned int)anim;

        sprite_pack.sprite[sprite][animation].texture      = SDL_CreateTextureFromSurface(renderer, surface);
        sprite_pack.sprite[sprite][animation].frames_count = (unsigned char)frames;
        sprite_pack.sprite[sprite][animation].width        = (signed short)surface->w;
        sprite_pack.sprite[sprite][animation].height       = (signed short)surface->h;
        SDL_FreeSurface(surface);
        
    }

    memset(sprites_string, 0, sizeof(SpritesPath) * (size_t)idx);
    free(sprites_string);

}