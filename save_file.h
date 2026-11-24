#ifndef SAVE_FILE_H
#define SAVE_FILE_H

#include <stddef.h>
#include <stdint.h>

#define SAVE_FILE_MAX_PAYLOAD 4096u
#define SAVE_FILE_PATH_SIZE 1024u

typedef struct SaveBuffer {

    uint8_t bytes[SAVE_FILE_MAX_PAYLOAD];
    size_t size;
    size_t cursor;
    _Bool is_valid;

} SaveBuffer;

void save_buffer_reset(SaveBuffer *buffer);
void save_buffer_put_u8(SaveBuffer *buffer, uint8_t value);
void save_buffer_put_u32(SaveBuffer *buffer, uint32_t value);
void save_buffer_put_u64(SaveBuffer *buffer, uint64_t value);
uint8_t save_buffer_get_u8(SaveBuffer *buffer);
uint32_t save_buffer_get_u32(SaveBuffer *buffer);
uint64_t save_buffer_get_u64(SaveBuffer *buffer);
_Bool save_buffer_is_consumed(const SaveBuffer *buffer);

_Bool save_file_path(const char *relative, char *out, size_t out_size);
_Bool save_file_ensure_dir(const char *relative);

_Bool save_file_write(const char *path, const char magic[4], uint32_t version, const SaveBuffer *payload);
_Bool save_file_read(const char *path, const char magic[4], uint32_t version, SaveBuffer *payload);

#endif
