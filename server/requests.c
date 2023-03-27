#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "requests.h"
#include "req_structure.h"
#include "database/client/manage.h"
#include "database/db/entities.h"
#include "database/client/queries.h"

static Properties_filter _map_request_filter_to_properties_filter(struct Filter req_filter);
static Array_node _do_select_request(Database* db, struct View view);
static Array_node _do_insert_request(Database* db, struct View view);

static Field _map_request_value_to_field(struct Value value) {
	Field result = {};
	switch(value.type) {
		case STRING_TYPE: 
			result = (Field) {.type = STRING, .string = (char*)malloc(sizeof(char) * strlen(value.string) + 1)};
			strcpy(result.string, value.string);
			break;
		case INTEGER_TYPE:
			result = (Field) {.type = NUMBER, .number = value.integer};
			break;
		case BOOLEAN_TYPE:
			result = (Field) {.type = BOOLEAN, .boolean = value.boolean};
			break;
		default: printf("UNKNOWN TYPE\n"); assert(0);
	}
	return result;
}

Array_node do_request(Database* db, struct View view) {
	switch(view.operation) {
		case CRUD_QUERY:
			return _do_select_request(db, view);
		case CRUD_INSERT:
			return _do_insert_request(db, view);
		default:
			printf("UNKNOWN CRUD OPERATION\n");
			return (Array_node){0, NULL};
	}
}

static Array_node _do_select_request(Database* db, struct View view) {
	Select_nodes main_node_query = {
		.selection_mode = ALL_NODES,
		.tag_name = view.header.tag,
		.filter.has_filter = view.header.filter_not_null
	};
	
	if(main_node_query.filter.has_filter) {
		main_node_query.filter.container = (Filter_container){
			.type = PROPERTY_FILTER, 
			.properties_filter = _map_request_filter_to_properties_filter(view.header.filter)
		};
	}
	Array_node main_node_as_array = nodes(db, main_node_query);
	if(main_node_as_array.size == 0) {
		printf("No main nodes found");
		return main_node_as_array;
	}
	if(main_node_as_array.size != 1) {
		printf("Too many main nodes");
		return main_node_as_array;
	}
	
	Node main_node = main_node_as_array.values[0];
	
	if(view.related_nodes_count == 0) return (Array_node) main_node_as_array;
	
	size_t result_nodes_capacity = 10;
	Array_node result = main_node_as_array;
	realloc(result.values, sizeof(Node) * result_nodes_capacity);
	
	for(size_t i = 0; i < view.related_nodes_count; i++) {
		Select_nodes related_nodes_query = {
			.selection_mode = NODES_BY_LINKED_NODE,
			.tag_name = view.related_nodes[i].header.tag,
			.linked_node_id = main_node.id,
			.filter.has_filter = view.related_nodes[i].header.filter_not_null
		};
		
		if(related_nodes_query.filter.has_filter) {
			related_nodes_query.filter.container = (Filter_container){
				.type = PROPERTY_FILTER, 
				.properties_filter = _map_request_filter_to_properties_filter(view.related_nodes[i].header.filter)
			};
		}
		
		Array_node related_nodes = nodes(db, related_nodes_query);
		for(size_t node_idx = 0; node_idx < related_nodes.size; node_idx++) {
			if(result.size == result_nodes_capacity) {
				result_nodes_capacity += result_nodes_capacity / 2;
				realloc(result.values, sizeof(Node) * result_nodes_capacity);
			}
			result.values[result.size++] = related_nodes.values[node_idx];
		}
	}
	
	return result;
}

static Properties_filter _map_request_filter_to_properties_filter(struct Filter req_filter) {
	if(req_filter.is_native) {
		Property_filter_type filter_type;
		switch(req_filter.filter->opcode) {
			case OP_EQUAL: filter_type = EQ; break;
			case OP_GREATER: filter_type = GREATER; break;
			case OP_LESS: filter_type = LESS; break;
			case OP_NOT_GREATER: filter_type = L_EQ; break;
			case OP_NOT_LESS: filter_type = GT_EQ; break;
			default: printf("UNKNOWN FILTER OPERATION"); assert(0);
		}
		
		return (Properties_filter) {
			.logical_operation_type = AND_LO_TYPE, .is_terminal = true,
			.terminal_filter = (Terminal_property_filter){
				.type = filter_type, .value_to_compare = (Property){.name = req_filter.filter->name, .field = _map_request_value_to_field(req_filter.filter->value)}
			}
		};
	}
	
	Logical_operation_type log_op_type;
	switch(req_filter.func->type) {
		case OP_NOT: log_op_type = NOT_LO_TYPE; assert(req_filter.func->filters_count == 1); break;
		case OP_OR: log_op_type = OR_LO_TYPE; break;
		case OP_AND: log_op_type = AND_LO_TYPE; break;
		default: printf("UNKNOWN LOGICAL OPERATION TYPE"); assert(0);
	}
	
	Properties_filter result = {.logical_operation_type = log_op_type, .is_terminal = false, .subfilters = {
		.size = req_filter.func->filters_count, .filters = malloc(sizeof(Properties_filter) * req_filter.func->filters_count)
	}};
	
	if(log_op_type == NOT_LO_TYPE) assert(result.subfilters.size == 1);
	
	for(size_t i = 0; i < result.subfilters.size; i++) {
		Properties_filter* filters = result.subfilters.filters;
		filters[i] = _map_request_filter_to_properties_filter(req_filter.func->filters[i]);
	}
	
	return result;
}

static Array_node _do_insert_request(Database* db, struct View view) {
	Node new_node = {.tag = view.header.tag, .properties_size = view.native_fields_count - 1, .properties = (Property*)malloc(sizeof(Property) * (view.native_fields_count - 1) )};
	
	size_t cur_prop_size = 0;
	for(size_t i = 0; i < view.native_fields_count; i++) {
		Field field = _map_request_value_to_field(view.native_fields[i].value);
		if(strcmp(view.native_fields[i].name, "id") == 0) {
			new_node.id = field;
		}
		else {
			new_node.properties[cur_prop_size++] = (Property) {.name = view.native_fields[i].name, .field = field};
		}
	}
	
	create_node(db, (Create_node){.node = new_node});
	
	return (Array_node){0, NULL}; // STUB
}