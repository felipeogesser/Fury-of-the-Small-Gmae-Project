#ifndef TYPE_TABLES_H
#define TYPE_TABLES_H

#include "general_types.h"
#include "general_types.h"
#include "json_parser_types.h"
#include "sprites_types.h"
#include <stddef.h>

enum JsonKind string_to_kind(const char *str);
const char *kind_to_string(enum JsonKind r);

enum GeneralType string_to_general_type(const char *str);
const char *general_type_to_string(enum GeneralType r);

enum BattalionType string_to_battalion_type(const char *str);
const char *battalion_type_to_string(enum BattalionType r);

enum Rarity string_to_rarity(const char *str);
const char *rarity_to_string(enum Rarity r);

enum Sprites string_to_sprite(const char *str);
const char *sprite_to_string(enum Sprites r);

typedef struct KindEntry {

    const char *name;
    enum JsonKind value;

} KindEntry;

typedef struct RarityEntry {

    const char *name;
    enum Rarity value;

} RarityEntry;

typedef struct GeneralTypeEntry {

    const char *name;
    enum GeneralType value;

} GeneralTypeEntry;

typedef struct BattalionTypeEntry {

    const char *name;
    enum BattalionType value;

} BattalionTypeEntry;

typedef struct SpritesEntry {

    const char *name;
    enum Sprites value;

} SpritesEntry;

typedef struct GenericEntry {

    const char *name;
    signed int value;

} GenericEntry;

typedef struct TableRef {

    const char *table_name;
    const void *entries;
    size_t entry_count;
    size_t entry_size;

} TableRef;

extern const KindEntry json_kind_table[];
extern const RarityEntry rarity_table[];
extern const GeneralTypeEntry general_type_table[];
extern const BattalionTypeEntry battalion_type_table[];
extern const SpritesEntry sprites_table[];
extern const TableRef all_tables[];
extern const size_t json_kind_table_count;
extern const size_t rarity_table_count;
extern const size_t general_type_table_count;
extern const size_t battalion_type_table_count;
extern const size_t sprites_table_count;
extern const size_t all_tables_count;

#endif