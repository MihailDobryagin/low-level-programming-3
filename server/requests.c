#include <assert.h>
#include <stdlib.h>
#include "requests.h"
#include "req_structure.h"
#include "database/client/manage.h"

static Properties_filter _map_request_filter_to_properties_filter(struct Filter req_filter);
static Array_node _do_select_request(Database* db, struct View view);

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

Array_node do_request(struct View view) {
	switch(view.operation) {
		case CRUD_QUERY:
			
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
	if(main_node_as_array.size == 1) {
		printf("To many main nodes");
		return main_node_as_array;
	}
	
	Node main_node = main_node_as_array.values[0];
	
	if(view.related_fields_count == 0) return main_node;
	
	size_t result_nodes_capacity = 10;
	Array_node result = main_node_as_array;
	realloc(result.values, sizeof(Node) * result_nodes_capacity);
	
	for(size_t i = 0; i < view.related_fields_count; i++) {
		Select_nodes related_nodes_query = {
			.selection_mode = NODES_BY_LINKED_NODE,
			.tag_name = view.related_fields[i].header.tag,
			.linked_node_id = main_node.id,
			.filter.has_filter = view.related_fields[i].header.filter_not_null
		}
		
		if(related_nodes_query.filter.has_filter) {
			related_nodes_query.filter.container = (Filter_container){
				.type = PROPERTY_FILTER, 
				.properties_filter = _map_request_filter_to_properties_filter(view.related_fields[i].header.filter)
			};
		}
		
		related_nodes = nodes(db, related_nodes_query);
		for(size_t node_idx = 0; node_idx < related_nodes.size; node_idx++) {
			if(result.size == result_nodes_capacity) {
				result_nodes_capacity += result_nodes_capacity / 2;
				realloc(result.values, sizeof(Node) * result_nodes_capacity);
			}
			result.values[result.size++] = related_nodes[node_idx];
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
			.terminal_filter = (Terminal_filter){
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
	
	Properties_filter result = {.logical_operation_type = log_op_type, is_terminal = false, .subfilters = {
		.size = req_filter.func->filters_count, .filters = malloc(sizeof(Properties_filter) * req_filter.func->filters_count)
	}};
	
	if(log_op_type == NOT_LO_TYPE) assert(result.subfilters.size == 1);
	
	for(size_t i = 0; i < result.subfilters.size; i++) result.subfilters.filters[i] = _map_request_filter_to_properties_filter(req_filter.func->filters[i]);
	
	return result;
}