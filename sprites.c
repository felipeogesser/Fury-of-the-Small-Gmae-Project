#include "sprites.h"
#include "sprites_internal.h"
#include "animation_types.h"
#include "engine_internal.h"
#include "field_entry_internal.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <SDL2/SDL.h>
#include <SDL_image.h>

static const ComplexFieldEntry sprite_complex_field_table[] = {

    #define OFFSET_OF(type, member) offsetof(type, member)
    #define SIZE_OF(type, member) (sizeof(((type *)0)->member))

    { "texture",       OFFSET_OF(Sprite, texture),       SIZE_OF(Sprite, texture),       NOT_DEFINED, NULL },
    { "frames_count",  OFFSET_OF(Sprite, frames_count),  SIZE_OF(Sprite, frames_count),  U_CHAR,      NULL },
    { "width",         OFFSET_OF(Sprite, width),         SIZE_OF(Sprite, width),         S_SHORT,     NULL },
    { "height",        OFFSET_OF(Sprite, height),        SIZE_OF(Sprite, height),        S_SHORT,     NULL },
    { "obj_size",      0,                                sizeof(Sprite),                 NOT_DEFINED, NULL },

    #undef OFFSET_OF
    #undef SIZE_OF

};

const ComplexFieldEntry *const sprite_complex_field_table_ptr = sprite_complex_field_table;

const ComplexFieldEntry sprite_info_complex_field_table[] = {

    #define OFFSET_OF(type, member) offsetof(type, member)
    #define SIZE_OF(type, member) (sizeof(((type *)0)->member))

    { "type",      OFFSET_OF(SpriteInfo, type),  SIZE_OF(SpriteInfo, type),  S_INT,       NULL },
    { "w",         OFFSET_OF(SpriteInfo, w),     SIZE_OF(SpriteInfo, w),     U_INT,       NULL },
    { "h",         OFFSET_OF(SpriteInfo, h),     SIZE_OF(SpriteInfo, h),     U_INT,       NULL },
    { "obj_size",  0,                            sizeof(SpriteInfo),         NOT_DEFINED, NULL },

    #undef OFFSET_OF
    #undef SIZE_OF

};

static const ComplexFieldEntry sprite_pack_complex_field_table[] = {

    #define OFFSET_OF(type, member) offsetof(type, member)
    #define SIZE_OF(type, member) (sizeof(((type *)0)->member))

    { "sprite",        OFFSET_OF(SpritePack, sprite),        SIZE_OF(SpritePack, sprite),        NOT_DEFINED, sprite_complex_field_table },
    { "sprite_count",  OFFSET_OF(SpritePack, sprite_count),  SIZE_OF(SpritePack, sprite_count),  U_INT,       NULL },
    { "obj_size",      0,                                    sizeof(SpritePack),                 NOT_DEFINED, NULL },

    #undef OFFSET_OF
    #undef SIZE_OF

};

const ComplexFieldEntry *const sprite_pack_complex_field_table_ptr = sprite_pack_complex_field_table;

static const FieldEntry sprite_field_table[] = {

    #define OFFSET_OF(type, member) offsetof(type, member)
    #define SIZE_OF(type, member) (sizeof(((type *)0)->member))

    { "texture",       OFFSET_OF(Sprite, texture),       SIZE_OF(Sprite, texture)       },
    { "frames_count",  OFFSET_OF(Sprite, frames_count),  SIZE_OF(Sprite, frames_count)  },
    { "width",         OFFSET_OF(Sprite, width),         SIZE_OF(Sprite, width)         },
    { "height",        OFFSET_OF(Sprite, height),        SIZE_OF(Sprite, height)        },
    { "obj_size",      0,                                sizeof(Sprite)                 },

    #undef OFFSET_OF
    #undef SIZE_OF

};

const FieldEntry *const sprite_field_table_ptr = sprite_field_table;

static const FieldEntry sprite_info_field_table[] = {

    #define OFFSET_OF(type, member) offsetof(type, member)
    #define SIZE_OF(type, member) (sizeof(((type *)0)->member))

    { "type",      OFFSET_OF(SpriteInfo, type),  SIZE_OF(SpriteInfo, type)  },
    { "w",         OFFSET_OF(SpriteInfo, w),     SIZE_OF(SpriteInfo, w)     },
    { "h",         OFFSET_OF(SpriteInfo, h),     SIZE_OF(SpriteInfo, h)     },
    { "obj_size",  0,                            sizeof(SpriteInfo)         },

    #undef OFFSET_OF
    #undef SIZE_OF

};

const FieldEntry *const sprite_info_field_table_ptr = sprite_info_field_table;

static const FieldEntry sprite_pack_field_table[] = {

    #define OFFSET_OF(type, member) offsetof(type, member)
    #define SIZE_OF(type, member) (sizeof(((type *)0)->member))

    { "sprite",        OFFSET_OF(SpritePack, sprite),        SIZE_OF(SpritePack, sprite)        },
    { "sprite_count",  OFFSET_OF(SpritePack, sprite_count),  SIZE_OF(SpritePack, sprite_count)  },
    { "obj_size",      0,                                    sizeof(SpritePack)                 },

    #undef OFFSET_OF
    #undef SIZE_OF

};

const FieldEntry *const sprite_pack_field_table_ptr = sprite_pack_field_table;

typedef struct SpritesPath {
    unsigned int sprite_index;
    char file_path[512];
} SpritesPath;

// private prototypes
static signed int count_pngs_in_folder(const char *path);
static signed int fill_pngs_from_folder(const char *folder, SpritesPath *out, signed int start_index, unsigned int sprite_index);
static unsigned int parse_frame_count_from_path(const char *path);
static signed int is_word_char(char c);
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

static signed int is_word_char(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
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
        if (j < nlen) continue;

        const _Bool starts_word = (i == 0) || !is_word_char(haystack[i - 1]);
        const _Bool ends_word = !is_word_char(haystack[i + nlen]);
        if (starts_word && ends_word) return 1;
    }
    return 0;
}

static signed int parse_animation_from_path(const char *path) {
    if (stristr_found(path, "Idle"))     return IDLE;
    if (stristr_found(path, "Walk"))     return WALK;
    if (stristr_found(path, "Run"))      return RUN;
    if (stristr_found(path, "Attack01")) return ATTACK_01;
    if (stristr_found(path, "Attack02")) return ATTACK_02;
    if (stristr_found(path, "Guard"))    return GUARD;
    if (stristr_found(path, "Death"))    return DEATH;
    return -1;
}

static SpritePack sprite_pack = {0};

void load_sprites_into_memory(void) {

    signed int count = 0;
    count += count_pngs_in_folder("assets/sprites/Archer");
    count += count_pngs_in_folder("assets/sprites/Cavalry");
    count += count_pngs_in_folder("assets/sprites/Guardsmen");
    count += count_pngs_in_folder("assets/sprites/Swordsmen");
    count += count_pngs_in_folder("assets/sprites/Templar");

    SpritesPath *sprites_string = calloc((size_t)count, sizeof(SpritesPath));
    if (!sprites_string) {
        fprintf(stderr, "Alloc for sprites path failed.\n");
        exit(EXIT_FAILURE);
    }

    signed int idx = 0;
    idx = fill_pngs_from_folder("assets/sprites/Archer",  sprites_string, idx, SPRITE_ARCHERS);
    idx = fill_pngs_from_folder("assets/sprites/Cavalry",  sprites_string, idx, SPRITE_CAVALRY);
    idx = fill_pngs_from_folder("assets/sprites/Guardsmen", sprites_string, idx, SPRITE_GUARDSMEN);
    idx = fill_pngs_from_folder("assets/sprites/Swordsmen", sprites_string, idx, SPRITE_SWORDSMEN);
    idx = fill_pngs_from_folder("assets/sprites/Templar", sprites_string, idx, SPRITE_TEMPLAR);

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

        if (sprite_pack.sprite[sprite][animation].texture != NULL) {
            fprintf(stderr, "Sprite '%s' is a second file for an animation already loaded.\n", sprites_string[k].file_path);
            for (signed int j = 0; j < k; j++) {
                const _Bool same_type = sprites_string[j].sprite_index == sprite;
                const _Bool same_animation = parse_animation_from_path(sprites_string[j].file_path) == anim;
                if (same_type && same_animation) fprintf(stderr, "  first loaded from '%s'.\n", sprites_string[j].file_path);
            }
            exit(EXIT_FAILURE);
        }
        sprite_pack.sprite[sprite][animation].texture      = SDL_CreateTextureFromSurface(renderer, surface);
        sprite_pack.sprite[sprite][animation].frames_count = (unsigned char)frames;
        sprite_pack.sprite[sprite][animation].width        = (signed short)surface->w;
        sprite_pack.sprite[sprite][animation].height       = (signed short)surface->h;
        SDL_FreeSurface(surface);

    }

    memset(sprites_string, 0, sizeof(SpritesPath) * (size_t)idx);
    free(sprites_string);

}