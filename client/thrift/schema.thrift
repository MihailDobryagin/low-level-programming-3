/**
 * The first thing to know about are types. The available types in Thrift are:
 *
 *  bool        Boolean, one byte
 *  i8 (byte)   Signed 8-bit integer
 *  i16         Signed 16-bit integer
 *  i32         Signed 32-bit integer
 *  i64         Signed 64-bit integer
 *  double      64-bit floating point value
 *  string      String
 *  binary      Blob (byte array)
 *  map<t1,t2>  Map from one type to another
 *  list<t1>    Ordered list of one type
 *  set<t1>     Set of unique elements of one type
 *
 * Did you also notice that Thrift supports C style comments?
 */
 
enum Crud_operation {
    CRUD_GET = 0,
    CRUD_REMOVE = 1,
    CRUD_NEW = 2,
    CRUD_UPDATE = 3
}

enum Condition_code {
    OP_EQUAL = 0,
    OP_GREATER = 1,
    OP_LESS = 2,
    OP_NOT_GREATER = 3,
    OP_NOT_LESS = 4,
    OP_SUBSTR = 5
}

enum Type {
    STRING_TYPE = 0,
    INTEGER_TYPE = 1,
    BOOLEAN_TYPE = 2
}

union Values {
	1: i32 Integer,
	2: bool Boolean,
	3: string String
}

struct Field_value {
	1: Type type,
	2: Value value
}

struct Field {
	1: string name,
    2: Field_value value
}

struct Entity {
	1: i16 fields_count,
	2: i16 rel_count,
    3: list<Field> fields,
    4: list<Field_value> rel_ids
}

struct Field_filter {
	1: Condition_code op,
	2: string field_name,
	3: Field_value field_value
}

union Condition_union {
	1: Field_value id,
	2: Field_filter field_filter
}

struct Condition {
	1: i8 is_negative,
    2: i8 is_id,
	3: Condition_union
}

struct Filter_list {
    1: i8 is_negative,
    2: list<Condition> and_conditions
}

struct View {
    1: enum Crud_operation op,
    2: list<string> field_names_to_output,
    3: list<Filter_list> *tree,
    4: Entity entity
}