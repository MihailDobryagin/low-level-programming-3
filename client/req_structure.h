#ifndef REQ_STRUCTURE_H
#define REQ_STRUCTURE_H

#include <inttypes.h>

#define MAX_NAME_SIZE 100
#define MAX_ARRAY_SIZE 100

enum Crud_operation {
    CRUD_QUERY = 0,
    CRUD_REMOVE,
    CRUD_INSERT,
    CRUD_UPDATE
};

enum Condition_code {
    OP_EQUAL = 0,
    OP_GREATER,
    OP_LESS,
    OP_NOT_GREATER,
    OP_NOT_LESS
};

enum Logic_op {
    OP_AND = 0,
    OP_OR,
    OP_NOT
};

enum Type {
    STRING_TYPE = 0,
    INTEGER_TYPE,
    BOOLEAN_TYPE
};

struct Value {
    enum Type type;
    union {
        char string[MAX_NAME_SIZE];
        int32_t integer;
        int8_t boolean;
    };
};

struct Native_filter {
    char name[MAX_NAME_SIZE]; // field_name
    enum Condition_code opcode;
    struct Value value;
};

struct Logic_func;

struct Filter {
    uint8_t is_native;
    union {
        struct Logic_func *func;
        struct Native_filter *filter;
    };
};

struct Logic_func {
    enum Logic_op type;
	size_t filters_count;
    struct Filter* filters;
};

struct Native_field {
    char name[MAX_NAME_SIZE];
    struct Value value;
};

struct Header {
    char tag[MAX_NAME_SIZE];
	uint8_t filter_not_null;
    struct Filter filter;
};

struct Related_node {
    size_t native_fields_count;
    char* field_names[MAX_ARRAY_SIZE];
    struct Header header;
};

struct View {
    enum Crud_operation operation;
    struct Header header;
    size_t native_fields_count;
    struct Native_field native_fields[MAX_ARRAY_SIZE];
    size_t related_nodes_count;
    struct Related_node related_nodes[MAX_ARRAY_SIZE];
};

struct Node_view {
	char tag_name[MAX_ARRAY_SIZE];
	size_t native_fields_count;
	struct Native_field native_fields[MAX_ARRAY_SIZE];
};

struct Answer {
	size_t nodes_count;
	struct Node_view* nodes;
};

#endif // !REQ_STRUCTURE_H