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
 
enum Crud_operation_TRANSPORT {
    CRUD_GET = 0,
    CRUD_REMOVE = 1,
    CRUD_NEW = 2,
    CRUD_UPDATE = 3
}

enum Condition_code_TRANSPORT {
    OP_EQUAL = 0,
    OP_GREATER = 1,
    OP_LESS = 2,
    OP_NOT_GREATER = 3,
    OP_NOT_LESS = 4,
    OP_SUBSTR = 5
}

enum Type_TRANSPORT {
    STRING_TYPE = 0,
    INTEGER_TYPE = 1,
    BOOLEAN_TYPE = 2
}

union Value_TRANSPORT {
	1: i32 Integer,
	2: bool Boolean,
	3: string String
}

struct Field_value_TRANSPORT {
	1: Type_TRANSPORT type,
	2: Value_TRANSPORT value
}

struct Field_TRANSPORT {
	1: string name,
    2: Field_value_TRANSPORT value
}

struct Entity_TRANSPORT {
	1: i16 fields_count,
	2: i16 rel_count,
    3: list<Field_TRANSPORT> fields,
    4: list<Field_value_TRANSPORT> rel_ids
}

struct Field_filter_TRANSPORT {
	1: Condition_code_TRANSPORT op,
	2: string field_name,
	3: Field_value_TRANSPORT field_value
}

union Condition_union_TRANSPORT {
	1: Field_value_TRANSPORT id,
	2: Field_filter_TRANSPORT field_filter
}

struct Condition_TRANSPORT {
	1: i8 is_negative,
    2: i8 is_id,
	3: Condition_union_TRANSPORT condition_union;
}

struct Filter_list_TRANSPORT {
    1: i8 is_negative,
    2: list<Condition_TRANSPORT> and_conditions
}

struct Request_TRANSPORT {
    1: Crud_operation_TRANSPORT op,
    2: list<string> field_names_to_output,
    3: list<Filter_list_TRANSPORT> tree,
    4: Entity_TRANSPORT entity
}

struct Answer_TRANSPORT {
  1: i16 code,
  2: string error_message,
  3: optional list<Entity_TRANSPORT> entities
}

service DBRequest {

  /**
   * A method definition looks like C code. It has a return type, arguments,
   * and optionally a list of exceptions that it may throw. Note that argument
   * lists and exception lists are specified using the exact same syntax as
   * field lists in struct or exception definitions.
   */

   void ping(),

   Answer_TRANSPORT do_request(1: Request_TRANSPORT req),
}
