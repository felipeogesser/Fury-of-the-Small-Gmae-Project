#ifndef FIELD_ENTRY_TYPES_H
#define FIELD_ENTRY_TYPES_H

typedef struct FieldEntry FieldEntry;
typedef struct ComplexFieldEntry ComplexFieldEntry;

enum Type {

    NOT_DEFINED = -1,
    BOOL,
    U_CHAR,
    S_CHAR,
    U_SHORT,
    S_SHORT,
    U_INT,
    S_INT,
    U_LONG,
    S_LONG,
    FLOAT,
    DOUBLE,
    STRUCT,
    TYPE_COUNT

};

#endif