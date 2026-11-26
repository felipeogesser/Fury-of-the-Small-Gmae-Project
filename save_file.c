#include "save_file.h"
#include <SDL2/SDL.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#define SAVE_ORG "cgame"
#define SAVE_APP "cgame"
#define SAVE_MAGIC_SIZE 4u
#define SAVE_HEADER_SIZE (SAVE_MAGIC_SIZE + 8u)
#define SAVE_CHECKSUM_SIZE 4u
#define SAVE_MAX_FILE (SAVE_HEADER_SIZE + SAVE_FILE_MAX_PAYLOAD + SAVE_CHECKSUM_SIZE)

typedef struct SaveRaw {

    uint8_t bytes[SAVE_MAX_FILE];
    size_t size;

} SaveRaw;

static SaveRaw raw;

void save_buffer_reset(SaveBuffer *buffer) {

    buffer->size = 0;
    buffer->cursor = 0;
    buffer->is_valid = true;

}

void save_buffer_put_u8(SaveBuffer *buffer, uint8_t value) {

    if (buffer->size + 1 > SAVE_FILE_MAX_PAYLOAD) {
        buffer->is_valid = false;
        return;
    }
    buffer->bytes[buffer->size++] = value;

}

void save_buffer_put_u32(SaveBuffer *buffer, uint32_t value) {

    for (unsigned int i = 0; i < 4; i++) save_buffer_put_u8(buffer, (uint8_t)(value >> (8 * i)));

}

void save_buffer_put_u64(SaveBuffer *buffer, uint64_t value) {

    for (unsigned int i = 0; i < 8; i++) save_buffer_put_u8(buffer, (uint8_t)(value >> (8 * i)));

}

uint8_t save_buffer_get_u8(SaveBuffer *buffer) {

    if (buffer->cursor + 1 > buffer->size) {
        buffer->is_valid = false;
        return 0;
    }
    return buffer->bytes[buffer->cursor++];

}

uint32_t save_buffer_get_u32(SaveBuffer *buffer) {

    uint32_t value = 0;
    for (unsigned int i = 0; i < 4; i++) value |= (uint32_t)save_buffer_get_u8(buffer) << (8 * i);
    return value;

}

uint64_t save_buffer_get_u64(SaveBuffer *buffer) {

    uint64_t value = 0;
    for (unsigned int i = 0; i < 8; i++) value |= (uint64_t)save_buffer_get_u8(buffer) << (8 * i);
    return value;

}

_Bool save_buffer_is_consumed(const SaveBuffer *buffer) {

    return buffer->is_valid && buffer->cursor == buffer->size;

}

static uint32_t checksum(const uint8_t *bytes, size_t size) {

    uint32_t hash = 2166136261u;
    for (size_t i = 0; i < size; i++) {
        hash ^= bytes[i];
        hash *= 16777619u;
    }
    return hash;

}

static void put_raw_u32(uint8_t *out, uint32_t value) {

    for (unsigned int i = 0; i < 4; i++) out[i] = (uint8_t)(value >> (8 * i));

}

static uint32_t get_raw_u32(const uint8_t *in) {

    uint32_t value = 0;
    for (unsigned int i = 0; i < 4; i++) value |= (uint32_t)in[i] << (8 * i);
    return value;

}

_Bool save_file_path(const char *relative, char *out, size_t out_size) {

    char *dir = SDL_GetPrefPath(SAVE_ORG, SAVE_APP);
    if (dir == NULL) {
        fprintf(stderr, "save: no writable directory: %s\n", SDL_GetError());
        return false;
    }
    const int written = snprintf(out, out_size, "%s%s", dir, relative);
    SDL_free(dir);
    return written > 0 && (size_t)written < out_size;

}

_Bool save_file_ensure_dir(const char *relative) {

    char path[SAVE_FILE_PATH_SIZE];
    if (!save_file_path(relative, path, sizeof(path))) return false;
    if (mkdir(path, 0755) == 0 || errno == EEXIST) return true;
    fprintf(stderr, "save: could not create %s: %s\n", path, strerror(errno));
    return false;

}

_Bool save_file_write(const char *path, const char magic[4], uint32_t version, const SaveBuffer *payload) {

    if (!payload->is_valid) return false;

    memcpy(raw.bytes, magic, SAVE_MAGIC_SIZE);
    put_raw_u32(raw.bytes + SAVE_MAGIC_SIZE, version);
    put_raw_u32(raw.bytes + SAVE_MAGIC_SIZE + 4, (uint32_t)payload->size);
    memcpy(raw.bytes + SAVE_HEADER_SIZE, payload->bytes, payload->size);
    put_raw_u32(raw.bytes + SAVE_HEADER_SIZE + payload->size, checksum(payload->bytes, payload->size));
    raw.size = SAVE_HEADER_SIZE + payload->size + SAVE_CHECKSUM_SIZE;

    char tmp_path[SAVE_FILE_PATH_SIZE];
    const int written = snprintf(tmp_path, sizeof(tmp_path), "%s.tmp", path);
    if (written <= 0 || (size_t)written >= sizeof(tmp_path)) return false;

    FILE *file = fopen(tmp_path, "wb");
    if (file == NULL) {
        fprintf(stderr, "save: could not open %s: %s\n", tmp_path, strerror(errno));
        return false;
    }
    const _Bool is_written = fwrite(raw.bytes, 1, raw.size, file) == raw.size;
    const _Bool is_flushed = fflush(file) == 0;
    const _Bool is_closed = fclose(file) == 0;

    if (!is_written || !is_flushed || !is_closed || rename(tmp_path, path) != 0) {
        fprintf(stderr, "save: could not write %s\n", path);
        remove(tmp_path);
        return false;
    }
    return true;

}

static _Bool read_raw(const char *path) {

    FILE *file = fopen(path, "rb");
    if (file == NULL) return false;

    raw.size = fread(raw.bytes, 1, SAVE_MAX_FILE, file);
    const _Bool is_too_large = fgetc(file) != EOF;
    const _Bool has_error = ferror(file) != 0;
    fclose(file);
    return !is_too_large && !has_error;

}

_Bool save_file_read(const char *path, const char magic[4], uint32_t max_version, SaveBuffer *payload, uint32_t *version) {

    save_buffer_reset(payload);
    if (!read_raw(path)) return false;

    if (raw.size < SAVE_HEADER_SIZE + SAVE_CHECKSUM_SIZE || memcmp(raw.bytes, magic, SAVE_MAGIC_SIZE) != 0) {
        fprintf(stderr, "save: %s is not a save file\n", path);
        return false;
    }
    *version = get_raw_u32(raw.bytes + SAVE_MAGIC_SIZE);
    if (*version == 0 || *version > max_version) {
        fprintf(stderr, "save: %s has an unsupported version\n", path);
        return false;
    }

    const size_t payload_size = get_raw_u32(raw.bytes + SAVE_MAGIC_SIZE + 4);
    if (payload_size != raw.size - SAVE_HEADER_SIZE - SAVE_CHECKSUM_SIZE) {
        fprintf(stderr, "save: %s is truncated\n", path);
        return false;
    }
    if (get_raw_u32(raw.bytes + SAVE_HEADER_SIZE + payload_size) != checksum(raw.bytes + SAVE_HEADER_SIZE, payload_size)) {
        fprintf(stderr, "save: %s is corrupted\n", path);
        return false;
    }

    memcpy(payload->bytes, raw.bytes + SAVE_HEADER_SIZE, payload_size);
    payload->size = payload_size;
    return true;

}
