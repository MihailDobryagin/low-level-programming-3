#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "requests.h"
#include "req_structure.h"
#include "database/client/manage.h"
#include "database/db/entities.h"
#include "database/client/queries.h"

static struct Answer _answer_from_array_node(Array_node nodes);
static Properties_filter _map_request_filter_to_properties_filter(struct Filter req_filter);
static Array_node _do_select_request(Database* db, struct View view);
static Array_node _do_insert_request(Database* db, struct View view);
static Array_node _do_delete_request(Database* db, struct View view);
static Array_node _do_update_request(Database* db, struct View view);

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

static struct Value _map_field_to_request_value(Field field) {
	struct Value result;
	switch(field.type) {
		case STRING: 
			result = (struct Value) {.type = STRING_TYPE};
			strcpy(result.string, field.string);
			break;
		case NUMBER:
			result = (struct Value) {.type = INTEGER_TYPE, .integer = field.number};
			break;
		case BOOLEAN:
			result = (struct Value) {.type = BOOLEAN_TYPE, .boolean = field.boolean};
			break;
		default: printf("UNSUPPORTED BY REQUEST FIELD-TYPE '%d'\n", field.type); assert(0);
	}
	return result;
}

struct Answer do_request(Database* db, struct View view) {
	Array_node output_nodes;
	
	switch(view.operation) {
		case CRUD_QUERY:
			printf("SELECT OPERATION\n");
			output_nodes = _do_select_request(db, view);
			break;
		case CRUD_INSERT:
			printf("INSERT OPERATION\n");
			output_nodes = _do_insert_request(db, view);
			break;
		case CRUD_REMOVE:
			printf("DELETE OPERATION\n");
			output_nodes = _do_delete_request(db, view);
			break;
		case CRUD_UPDATE:
			printf("UPDATE OPERATION\n");
			output_nodes = _do_update_request(db, view);
			break;
		default:
			printf("UNKNOWN CRUD OPERATION\n");
			output_nodes = (Array_node){0, NULL};
			break;
	}
	
	printf("Start building ANSWER from returned nodes...\n");
	struct Answer answer = _answer_from_array_node(output_nodes);
	printf("ANSWER built\n");
	
	for(size_t i = 0; i < output_nodes.size; i++) free_node_internal(output_nodes.values[i]);
	if(output_nodes.size) free(output_nodes.values);
	
	return answer;
}

static struct Node_view _node_to_view(Node node) {
	struct Node_view view = {};
	strcpy(view.tag_name, node.tag);
	
	view.native_fields_count = node.properties_size + 1;
	strcpy(view.native_fields[0].name, "id");
	view.native_fields[0].value = _map_field_to_request_value(node.id);
	
	for(size_t i = 0; i < node.properties_size; i++) {
		strcpy(view.native_fields[i + 1].name, node.properties[i].name);
		view.native_fields[i + 1].value = _map_field_to_request_value(node.properties[i].field);
	}
	
	return view;
}

static struct Answer _answer_from_array_node(Array_node nodes) {
	struct Answer result = {.nodes_count = nodes.size, .nodes = malloc(sizeof(struct Node_view) * nodes.size)};
	for(size_t i = 0; i < nodes.size; i++) result.nodes[i] = _node_to_view(nodes.values[i]);
	return result;
}

static Array_node _do_select_request(Database* db, struct View view) {
	Select_nodes main_node_query = {
		.selection_mode = ALL_NODES,
		.tag_name = view.header.tag,
		.filter.has_filter = view.header.filter_not_null
	};
	
	printf("FILTER NOT NULL -> %d\n", main_node_query.filter.has_filter);
	
	if(main_node_query.filter.has_filter) {
		main_node_query.filter.container = (Filter_container){
			.type = PROPERTY_FILTER, 
			.properties_filter = _map_request_filter_to_properties_filter(view.header.filter)
		};
	}
	
	printf("Start selecting MAIN node...\n");
	Array_node main_node_as_array = nodes(db, main_node_query);
	printf("MAIN node selected\n");
	if(main_node_as_array.size == 0) {
		printf("No main nodes found\n");
		return main_node_as_array;
	}
	if(main_node_as_array.size != 1) {
		printf("Too many main nodes\n");
		return main_node_as_array;
	}
	
	printf("Main nodes size -> %d\n", main_node_as_array.size);
	
	if(view.related_nodes_count == 0) {
		printf("No RELATED nodes. Result returned\n");
		return main_node_as_array;
	}
	
	size_t result_nodes_capacity = 10;
	Array_node result = main_node_as_array;
	realloc(result.values, sizeof(Node) * result_nodes_capacity);
	
	printf("Start selecting RELATED nodes (%d relations)...\n", view.related_nodes_count);
	
	Node main_node = main_node_as_array.values[0];
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
	
	printf("RELATED nodes selected (%d nodes)\n", result.size - 1);
	
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
	
	for(size_t i = 0; i < view.related_nodes_count; i++) {
		if(!view.related_nodes[i].header.filter_not_null) {
			printf("filter for bounded node is null\n");
			continue;
		}
		Properties_filter related_node_filter = _map_request_filter_to_properties_filter(view.related_nodes[i].header.filter);
		Select_nodes related_nodes_query = {
			.tag_name = view.related_nodes[i].header.tag,
			.selection_mode = ALL_NODES,
			.filter = {
				.has_filter = true,
				.container = (Filter_container){.type = PROPERTY_FILTER, .properties_filter = related_node_filter}
			}
		};
		
		Array_node related_nodes = nodes(db, related_nodes_query);
		for(size_t i = 0; i < related_nodes.size; i++) {
			Edge new_edge = {
				.tag = "edges",
				.id = (Field) {.type = STRING, .string = "<edge-common-id>"},
				.node1_id = new_node.id,
				.node2_id = related_nodes.values[i].id,
				.properties_size = 0,
				.properties = NULL
			};
			create_edge(db, (Create_edge){.edge = new_edge});
		}
	}
	
	return (Array_node){0, NULL}; // STUB
}

static Array_node _do_delete_request(Database* db, struct View view) {
	Select_nodes delete_query = {
		.selection_mode = ALL_NODES,
		.tag_name = view.header.tag,
		.filter.has_filter = view.header.filter_not_null
	};
	
	if(delete_query.filter.has_filter) {
		delete_query.filter.container = (Filter_container){
			.type = PROPERTY_FILTER, 
			.properties_filter = _map_request_filter_to_properties_filter(view.header.filter)
		};
	}
	
	printf("Start searching NODES to DELETE...\n");
	Array_node to_delete_nodes = nodes(db, delete_query);
	printf("NODES to DELETED selected (%d)\n", to_delete_nodes);
	printf("DELETE nodes...\n");
	delete_nodes(db, delete_query);
	printf("NODES DELETED\n");
	
	return to_delete_nodes;
}

static Array_node _do_update_request(Database* db, struct View view) {
	Select_nodes select_node_query = {
		.selection_mode = ALL_NODES,
		.tag_name = view.header.tag,
		.filter.has_filter = view.header.filter_not_null
	};
	
	assert(select_node_query.filter.has_filter);
	if(select_node_query.filter.has_filter) {
		select_node_query.filter.container = (Filter_container){
			.type = PROPERTY_FILTER, 
			.properties_filter = _map_request_filter_to_properties_filter(view.header.filter)
		};
	}
	
	Array_node node_to_update_as_array = nodes(db, select_node_query);
	
	if(node_to_update_as_array.size == 0) return (Array_node){0, NULL};
	if(node_to_update_as_array.size != 1) {
		printf("Too many NODES to update (%d)\n", node_to_update_as_array.size);
		return (Array_node){0, NULL};
	}
	
	Node node = node_to_update_as_array.values[0];
	
	for(size_t i = 0; i < view.native_fields_count; i++) {
		for(size_t prop_idx = 0; prop_idx < node.properties_size; prop_idx++) {
			if(strcmp(node.properties[prop_idx].name, view.native_fields[i].name) == 0) {
				node.properties[prop_idx].field = _map_request_value_to_field(view.native_fields[i].value);
				break;
			}
		}
	}
	
	printf("UPDATE NODE...\n");
	change_node(db, (Change_node){.changed_node = node});
	printf("NODE updated\n");
	
	return (Array_node){.size = 0, .values = (Node[1]){node}};
}