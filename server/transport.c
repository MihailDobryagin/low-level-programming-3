#include <stdio.h>
#include <assert.h>
#include "transport.h"

#ifndef TRANSPORT_OBJECTS_CREATION
#define TRANSPORT_OBJECTS_CREATION
#define _g_object_type_for_transport(OBJECT_NAME) OBJECT_NAME##__t_r_a_n_s_p_o_r_t_get_type()
#define _g_object_new(OBJECT_NAME) g_object_new(_g_object_type_for_transport(OBJECT_NAME), NULL)
#endif

static struct Filter filter_from_transport(Filter_TRANSPORT* filter_transport);
static struct Value value_from_transport(Value_TRANSPORT* value_transport);
static Value_TRANSPORT* value_for_transport(struct Value value);
struct Header header_from_transport(Header_TRANSPORT* header_transport);

struct View request_from_transport_request(Request_TRANSPORT* req) {
	struct View view = {};
	Header_TRANSPORT* header_transport; GPtrArray* fields_transport; GPtrArray* related_nodes_transport;
	g_object_get(req,
		"operation", &(view.operation), "header", &header_transport,
		"fields", &fields_transport, "related_nodes", &related_nodes_transport,
	NULL);
	
	view.header = header_from_transport(header_transport);
	view.native_fields_count = fields_transport->len;
	view.related_nodes_count = related_nodes_transport->len;
	
	for(size_t i = 0; i < view.native_fields_count; i++) {
		Native_field_TRANSPORT* field_transport = g_ptr_array_index(fields_transport, i);
		char* getted_name;
		g_object_get(field_transport, "name", &getted_name, NULL);
		strcpy(view.native_fields[i].name, getted_name);
		if(view.operation == CRUD_INSERT || view.operation == CRUD_UPDATE) {
			Value_TRANSPORT* value_transport;
			g_object_get(field_transport, "value", &value_transport, NULL);
			view.native_fields[i].value = value_from_transport(value_transport);
		}
	}
	
	for(size_t i = 0; i < view.related_nodes_count; i++) {
		Related_node_TRANSPORT* related_node_transport = g_ptr_array_index(related_nodes_transport, i);
		Header_TRANSPORT* related_node_header_transport; GPtrArray* related_node_field_names;
		g_object_get(related_node_transport,
			"header", &related_node_header_transport, "field_names", &related_node_field_names,
		NULL);
		
		view.related_nodes[i].header = header_from_transport(related_node_header_transport);
		view.related_nodes[i].native_fields_count = related_node_field_names->len;
		
		for(size_t name_idx = 0; name_idx < view.related_nodes[i].native_fields_count; name_idx++) {
			view.related_nodes[i].field_names[name_idx] = g_ptr_array_index(related_node_field_names, name_idx);
		}
	}
	
	return view;
}

Answer_TRANSPORT* answer_for_transport(struct Answer answer) {
	Answer_TRANSPORT* answer_transport = _g_object_new(answer);
	
	GPtrArray* nodes_transport = g_ptr_array_new();
	for(size_t i = 0; i < answer.nodes_count; i++) {
		GPtrArray* fields_transport = g_ptr_array_new();
		const struct Node_view node = answer.nodes[i];
		for(size_t field_idx = 0; field_idx < node.native_fields_count; field_idx++) {
			struct Native_field field = node.native_fields[field_idx];
			Native_field_TRANSPORT* field_transport = _g_object_new(native_field);
			g_object_set(field_transport,
				"name", field.name,
				"value", value_for_transport(field.value),
			NULL);
			g_ptr_array_add(fields_transport, field_transport);
		}
		Node_view_TRANSPORT* node_view_transport = _g_object_new(node_view);
		g_object_set(node_view_transport, "tag_name", node.tag_name, "fields", fields_transport, NULL);
		g_ptr_array_add(nodes_transport, node_view_transport);
	}
	
	g_object_set(answer_transport, "code", 0, "nodes", nodes_transport, NULL);
	return answer_transport;
}

struct Header header_from_transport(Header_TRANSPORT* header_transport) {
	struct Header result = {};

	char* getted_tag;
	g_object_get(header_transport, "tag", &getted_tag, "filter_not_null", &result.filter_not_null, NULL);
	strcpy(result.tag, getted_tag);
	
	if(result.filter_not_null) {
		struct Filter filter;
		Filter_TRANSPORT* filter_transport;
		g_object_get(header_transport, "filter", &filter_transport, NULL);
		result.filter = filter_from_transport(filter_transport);
	}
	
	return result;
}

static struct Filter filter_from_transport(Filter_TRANSPORT* filter_transport) {
	int8_t is_native; g_object_get(filter_transport, "is_native", &is_native, NULL);
	Filter_union_TRANSPORT* filter_union_transport; g_object_get(filter_transport, "filter", &filter_union_transport, NULL);
	
	
	if(is_native) {
		Native_filter_TRANSPORT* native_filter_transport; g_object_get(filter_union_transport, "filter", &native_filter_transport, NULL);
		Value_TRANSPORT* value_transport;
		struct Native_filter* native_filter = malloc(sizeof(struct Native_filter));
		
		char* str_tmp;
		g_object_get(native_filter_transport, "name", &str_tmp, "opcode", &(native_filter->opcode), "value", &value_transport, NULL);
		strcpy(native_filter->name, str_tmp);
		
		native_filter->value = value_from_transport(value_transport);
		
		return (struct Filter){.is_native = 1, .filter = native_filter};
		
	} else {
		Logic_func_TRANSPORT* func_transport; g_object_get(filter_union_transport, "func", &func_transport, NULL);
		struct Logic_func* func = malloc(sizeof(struct Logic_func));
		GPtrArray* filters_transport;
		g_object_get(func_transport, "type", &(func->type), "filters", &filters_transport, NULL);
		func->filters_count = filters_transport->len;
		func->filters = malloc(sizeof(struct Filter) * func->filters_count);
		
		for(size_t i = 0; i < func->filters_count; i++) {
			Filter_TRANSPORT* filter_transport = g_ptr_array_index(filters_transport, i);
			func->filters[i] = filter_from_transport(filter_transport);
		}
		
		return (struct Filter){.is_native = 0, .func = func};
	}
}

static struct Value value_from_transport(Value_TRANSPORT* value_transport) {
	enum Value_type value_type; Value_union_TRANSPORT* value_union_transport;
	g_object_get(value_transport, "type", &value_type, "value", &value_union_transport, NULL);
	struct Value result = {.type = value_type};
	char* tmp_str;
	switch(value_type) {
		case STRING_TYPE: 
			g_object_get(value_union_transport, "String", &tmp_str, NULL); 
			strcpy(result.string, tmp_str);
			break;
		case INTEGER_TYPE: g_object_get(value_union_transport, "Integer", &result.integer, NULL); break;
		case BOOLEAN_TYPE: g_object_get(value_union_transport, "Boolean", &result.boolean, NULL); break;
		default: printf("UNKNOWN TYPE FOR VALUE\n"); assert(0);
	}
	
	return result;
}

static Value_TRANSPORT* value_for_transport(struct Value value) {
	Value_TRANSPORT* value_transport = _g_object_new(value);
	g_object_set(value_transport, "type", value.type, NULL);
	Value_union_TRANSPORT* value_union_transport = _g_object_new(value_union);
	switch(value.type) {
		case STRING_TYPE: g_object_set(value_union_transport, "String", value.string, NULL); break;
		case INTEGER_TYPE: g_object_set(value_union_transport, "Integer", value.integer, NULL); break;
		case BOOLEAN_TYPE: g_object_set(value_union_transport, "Boolean", value.integer, NULL); break;
		default: printf("UNSUPPORTED VALUE TYPE\n"); assert(0);
	}
	g_object_set(value_transport, "value", value_union_transport, NULL);
	return value_transport;
}