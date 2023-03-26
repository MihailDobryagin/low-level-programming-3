#include <inttypes.h>

#define MAX_NAME_SIZE 20
#define MAX_ARRAY_SIZE 20

enum Crud_operation {
    CRUD_GET = 0,
    CRUD_REMOVE,
    CRUD_NEW,
    CRUD_UPDATE
};

enum Condition_code {
    OP_EQUAL = 0,
    OP_GREATER,
    OP_LESS,
    OP_NOT_GREATER,
    OP_NOT_LESS,
    OP_SUBSTR,
};

enum Type {
    STRING_TYPE = 0,
    INTEGER_TYPE,
    BOOLEAN_TYPE
};

struct Field_value {
	enum Type type;
	union {
		char string[MAX_NAME_SIZE];
		int64_t integer;
		int64_t boolean;
	};
};

struct Field {
	char name[MAX_NAME_SIZE];
    struct Field_value value;
};

struct Entity {
	uint8_t fields_count;
	uint8_t rel_count;
    struct Field fields[MAX_ARRAY_SIZE];
    struct Field_value rel_ids[MAX_ARRAY_SIZE];
};

struct Condition {
	uint8_t is_negative;
    uint8_t is_id;
	union {
		struct Field_value id;
		struct {
			enum Condition_code op;
			char field_name[MAX_NAME_SIZE];
			struct Field_value field_value;
		} field_filter;
	};
    struct Condition *next;
};

struct Filter_list {
	uint8_t is_negative;
    struct Condition *condition;
    struct Filter_list *next;
};

struct Sample {
    struct Sample *root_child;
    char name[MAX_NAME_SIZE];
    struct Sample *next;
};

struct View {
    enum Crud_operation op;
    struct Sample *root; // Fields to output
    struct Filter_list *filters;
    struct Entity *entity;
};