#include <stdio.h>
#include <assert.h>
#include "transport.h"

#define _g_object_type_for_transport(OBJECT_NAME) OBJECT_NAME##__t_r_a_n_s_p_o_r_t_get_type()
#define _g_object_new(OBJECT_NAME) g_object_new(_g_object_type_for_transport(OBJECT_NAME), NULL)

static struct Filter filter_from_transport(Filter_TRANSPORT* filter_transport);
static struct Value value_from_transport(Value_TRANSPORT* value_transport);
struct Header header_from_transport(Header_TRANSPORT* header_transport);

struct View request_from_transport_request(Request_TRANSPORT* req) {
	struct View view = (struct View){};
	Header_TRANSPORT* header_transport; GPtrArray* fields_transport; GPtrArray* related_nodes_transport;
	g_object_get(req, 
		"operation", &(view.operation), "header", &header_transport,
		"fields", &fields_transport, "related_nodes", &related_nodes_transport,
	NULL);
	
	view.native_fields_count = fields_transport->len;
	view.related_fields_count = related_nodes_transport->len;
	
	for(size_t i = 0; i < view.native_fields_count; i++) {
		Native_field_TRANSPORT* field_transport = g_ptr_array_index(fields_transport, i);
		g_object_get(field_transport, "name", &(view.native_fields[i]), NULL);
		if(view.operation == CRUD_INSERT || view.operation == CRUD_UPDATE) {
			Value_TRANSPORT* value_transport;
			g_object_get(field_transport, "value", &value_transport, NULL);
			view.native_fields[i].value = value_from_transport(value_transport);
		}
	}
	
	for(size_t i = 0; i < view.related_fields_count; i++) {
		Related_node_TRANSPORT* related_node_transport = g_ptr_array_index(related_nodes_transport, i);
		Header_TRANSPORT* related_node_header_transport; GPtrArray* related_node_field_names;
		g_object_get(related_node_transport,
			"header", &related_node_header_transport, "field_names", &related_node_field_names,
		NULL);
		
		view.related_fields[i].header = header_from_transport(related_node_header_transport);
		view.related_fields[i].native_fields_count = related_node_field_names->len;
		view.related_fields[i].field_names = (char**)malloc(sizeof(char*) * view.related_fields[i].native_fields_count);
		
		for(size_t name_idx = 0; name_idx < view.related_fields[i].native_fields_count; name_idx++) {
			view.related_fields[i].field_names[name_idx] = g_ptr_array_index(related_node_field_names, name_idx);
		}
	}
	
	return view;
}

struct Header header_from_transport(Header_TRANSPORT* header_transport) {
	int8_t is_filter_not_null; g_object_get(header_transport, "filter_not_null", is_filter_not_null, NULL);
	struct Filter filter;
	if(is_filter_not_null) {
		Filter_TRANSPORT* filter_transport;
		g_object_get(header_transport, "filter", &filter_transport, NULL);
		filter = filter_from_transport(filter_transport);
	}
	struct Header result = {
		.filter_not_null = is_filter_not_null,
		.filter = filter
	};
	g_object_get(header_transport, "tag", &result.tag, NULL);
	
	return result;
}

static struct Filter filter_from_transport(Filter_TRANSPORT* filter_transport) {
	int8_t is_native; g_object_get(filter_transport, "is_native", &is_native, NULL);
	Filter_union_TRANSPORT* filter_union_transport; g_object_get(filter_transport, "filter", &filter_union_transport, NULL);
	
	
	if(is_native) {
		Native_filter_TRANSPORT* native_filter_transport; g_object_get(filter_union_transport, "filter", native_filter_transport, NULL);
		Value_TRANSPORT* value_transport;
		struct Native_filter* native_filter = malloc(sizeof(struct Native_filter));
		
		g_object_get(native_filter_transport, "name", &(native_filter->name), "opcode", &(native_filter->opcode), "value", &value_transport, NULL);
		
		native_filter->value = value_from_transport(value_transport);
		
		return (struct Filter){.is_native = 1, .filter = native_filter};
		
	} else {
		Logic_func_TRANSPORT* func_transport; g_object_get(filter_union_transport, "func", &func_transport, NULL);
		struct Logic_func* func = malloc(sizeof(struct Logic_func));
		GPtrArray* filters_transport;
		g_object_get(func, "type", &(func->type), "filters", &filters_transport, NULL);
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
	
	switch(value_type) {
		case STRING_TYPE: g_object_get(value_transport, "String", &result.string, NULL); break;
		case INTEGER_TYPE: g_object_get(value_transport, "Integer", &result.integer, NULL); break;
		case BOOLEAN_TYPE: g_object_get(value_transport, "Boolean", &result.boolean, NULL); break;
		default: printf("UNKNOWN TYPE FOR VALUE\n"); assert(0);
	}
	
	return result;
}