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

enum Logic_op_TRANSPORT {
    OP_AND = 0,
    OP_OR = 1,
    OP_NOT = 2
}

enum Type_TRANSPORT {
    STRING_TYPE = 0,
    INTEGER_TYPE = 1,
    BOOLEAN_TYPE = 2
}

union Value_union_TRANSPORT {
	1: i32 Integer,
	2: bool Boolean,
	3: string String
}

union Value_TRANSPORT {
	1: Type_TRANSPORT type,
	2: Value_union_TRANSPORT value
}

struct Native_filter_TRANSPORT {
    1: string name,
    2: Condition_code_TRANSPORT opcode,
    3: Value_TRANSPORT value
}

struct Logic_func_TRANSPORT {
    1: Logic_op_TRANSPORT type,
    2: list<Filter_TRANSPORT> filters
}

union Filter_union_TRANSPORT {
	1: Logic_func_TRANSPORT func,
	2: Native_filter_TRANSPORT filter
}

struct Filter_TRANSPORT {
    1: i8 is_native,
    2: Filter_union_TRANSPORT filter
}

struct Native_field_TRANSPORT {
    1: string name,
    2: Value_TRANSPORT value
}

struct Header_TRANSPORT {
    1: string tag,
	2: i8 filter_not_null,
    3: optional Filter_TRANSPORT filter
}

struct Related_node_TRANSPORT {
    1: Header_TRANSPORT header,
    2: list<string> field_names
}

struct Request_TRANSPORT {
    1: Crud_operation_TRANSPORT operation,
    2: Header_TRANSPORT header,
	3: list<Native_field_TRANSPORT> fields,
    4: list<Related_node_TRANSPORT> related_nodes
}

struct Node_TRANSPORT {
	1: string tag_name,
	2: list<Native_field_TRANSPORT> fields,
	3: list<Value_TRANSPORT> related_node_ids
}

struct Answer_TRANSPORT {
	1: i16 code,
	2: string error_message
	3: list<Node_TRANSPORT> nodes
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
