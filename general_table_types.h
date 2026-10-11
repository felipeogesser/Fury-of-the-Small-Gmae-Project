#ifndef GENERAL_TABLE_TYPES_H
#define GENERAL_TABLE_TYPES_H

#include "field_entry_internal.h"

typedef struct GeneralTable GeneralTable;

typedef struct GeneralStaticTable GeneralStaticTable;
typedef struct GeneralStaticTableDiff GeneralStaticTableDiff;

typedef struct GeneralDynamicTable GeneralDynamicTable;
typedef struct GeneralDynamicTableDiff GeneralDynamicTableDiff;

typedef struct GeneralComparisonTable GeneralComparisonTable;

typedef struct MockGeneralStaticTable MockGeneralStaticTable;
typedef struct MockGeneralStaticTableDiff MockGeneralStaticTableDiff;

typedef struct MockGeneralDynamicTable MockGeneralDynamicTable;
typedef struct MockGeneralDynamicTableDiff MockGeneralDynamicTableDiff;

//extern const ComplexFieldEntry *const general_table_field_table_ptr;

#endif