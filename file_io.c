#include "file_io.h"
#include <stddef.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>

// private prototypes
static char *find_file_path(const char *restrict file_name);

char *open_read_close_file(const char *restrict file_name) {

    const char *restrict file_path_buf = find_file_path(file_name);
    if (!file_path_buf) {

        fprintf(stderr, "find_file_path returned NULL\n");
        return NULL;

    }

    errno = 0;
    FILE *file = fopen(file_path_buf, "rb");
    if (!file) {

        if (errno != 0) {
            perror("fopen failed");
        }
        free((void *)file_path_buf);
        fprintf(stderr, "fopen returned NULL\n");
        return NULL;

    }

    signed int fseek_status = 0;
    errno = 0;
    fseek_status = fseek(file, 0, SEEK_END);
    if (fseek_status != 0) {
        
        if (errno != 0) {
            perror("fseek failed");
        }
        fprintf(stderr, "fseek to end failed\n");
        fclose(file);
        free((void *)file_path_buf);
        return NULL;

    }

    errno = 0;
    signed long int file_size = ftell(file);
    if (file_size == -1L) {

        if (errno != 0) {
            perror("ftell failed");
        }
        fprintf(stderr, "ftell returned -1\n");
        fclose(file);
        free((void *)file_path_buf);
        return NULL;

    }

    errno = 0;
    fseek_status = fseek(file, 0, SEEK_SET);
    if (fseek_status != 0) {

        if (errno != 0) {
            perror("fseek failed");
        }
        fprintf(stderr, "fseek to start failed\n");
        fclose(file);
        free((void *)file_path_buf);
        return NULL;

    }

    errno = 0;
    char *buffer = calloc(1, file_size + 64); // +64 byes gives enough buffer for all callers. This prevents memcmp or any intrinsics that reads past the end of a buffer unable to touch protected memory and cause SIGSEGV.
    if (!buffer) {

        if (errno != 0) {
            perror("calloc failed");
        }
        fprintf(stderr, "calloc returned NULL for variable buffer at function open_read_close_file\n");
        fclose(file);
        free((void *)file_path_buf);
        return NULL;

    }

    errno = 0;
    size_t read = fread(buffer, 1, file_size, file);
    if (read != (size_t)file_size) {
        fprintf(stderr, "at function open_read_close_file, fread returned a value different from file_size\n");
        if (ferror(file)) {
            if (errno != 0) {
                perror("fread failed");
            }
            fprintf(stderr, "a read error occurred\n");
        } else if (feof(file)) {
            fprintf(stderr, "unexpected end of file (file may have shrunk since ftell)\n");
        } else {
            fprintf(stderr, "short read with neither error nor eof set (unexpected)\n");
        }
        clearerr(file);
        if (read == 0) {
            fprintf(stderr, "fread returned zero\n");
        }
        if (read > (size_t)file_size) {
            fprintf(stderr, "fread read more than file_size\n");
        }

        fclose(file);
        free(buffer);
        free((void *)file_path_buf);
        return NULL;

    }
    buffer[file_size] = '\0';
    
    fclose(file);
    free((void *)file_path_buf);

    return buffer;

}

static char *find_file_path(const char *restrict file_name) {

    #define PATH_CAPACITY 1024

    errno = 0;
    char *restrict file_path_buf = calloc(1, PATH_CAPACITY);
    if (!file_path_buf) {

        if (errno != 0) {
            perror("calloc failed");
        }
        fprintf(stderr, "calloc returned NULL for variable file_path_buf at function find_file_path\n");
        return NULL;

    }

    char exe_path[PATH_CAPACITY];
    errno = 0;
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (len == -1) {
        
        if (errno != 0) {
            perror("readlink failed");
        }
        free(file_path_buf);
        fprintf(stderr, "readlink failed, len == -1\n");
        return NULL;

    }
    if (len >= PATH_CAPACITY - 1) {

        free(file_path_buf);
        fprintf(stderr, "readlink succeded, but len >= PATH_CAPACITY - 1, possible truncation\n");
        return NULL;

    }
    exe_path[len] = '\0';
    char *dir = dirname(exe_path);

    errno = 0;
    signed int written = snprintf(file_path_buf, PATH_CAPACITY, "%s/%s", dir, file_name);
    if (written < 0) {
        
        if (errno != 0) {
            perror("snprintf failed");
        }
        free(file_path_buf);
        fprintf(stderr, "snprintf failed, written == -1\n");
        return NULL;

    }
    if (written >= PATH_CAPACITY) {

        free(file_path_buf);
        fprintf(stderr, "snprintf succeded, but written >= PATH_CAPACITY, possible truncation\n");
        return NULL;

    }
    #undef PATH_CAPACITY

    return file_path_buf;

}
