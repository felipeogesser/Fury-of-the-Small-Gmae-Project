#ifndef JSON_PARSER_TYPES_H
#define JSON_PARSER_TYPES_H

enum JsonKind {
    KIND_UNKNOWN = -1,
    KIND_GENERAL,
    KIND_UNIT,
    KIND_ITEM,
    KIND_WEAPON,
    KIND_COUNT
};

#endif