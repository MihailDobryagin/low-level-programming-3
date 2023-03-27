#include "transport.h"
#include "assert.h"

#define _g_object_type_for_transport(OBJECT_NAME) OBJECT_NAME##__t_r_a_n_s_p_o_r_t_get_type()
#define _g_object_new(OBJECT_NAME) g_object_new(_g_object_type_for_transport(OBJECT_NAME), NULL)

static struct Value_TRANSPORT* value_for_transport(struct Value value);
static struct Filter_TRANSPORT* filter_for_transport(struct Filter filter);
static struct Header_TRANSPORT* header_for_transport(struct Header header);

Request_TRANSPORT* transport_request_from_view_format(struct View view) {
	Request_TRANSPORT* request_transport = g_object_new(_g_object_type_for_transport(request), NULL);
	g_object_set(request_transport, 
		"operation", view.operation,
		"header", header_for_transport(view.header),
	NULL);
	
	GPtrArray* fields_transport = g_ptr_array_new();
	for(size_t i = 0; i < view.native_fields_count; i++) {
		struct Native_field_TRANSPORT* native_field_transport = _g_object_new(native_field);
		g_object_set(native_field_transport, "name", view.native_fields[i].name, NULL);
		if(view.operation == CRUD_INSERT || view.operation == CRUD_UPDATE) {
			g_object_set(native_field_transport, "value", value_for_transport(view.native_fields[i].value), NULL);
		}
		g_ptr_array_add(fields_transport, native_field_transport);
	}
	
	GPtrArray* related_nodes_transport = g_ptr_array_new();
	for(size_t i = 0; i < view.related_nodes_count; i++) {
		struct Related_node_TRANSPORT* related_node_transport = _g_object_new(related_node);
		GPtrArray* field_names_transport = g_ptr_array_new();
		
		for(size_t name_idx = 0; name_idx < view.related_nodes_count; name_idx++) g_ptr_array_add(field_names_transport, view.related_nodes[i].field_names[name_idx]);
			
		g_object_set(related_node_transport,
			"header", header_for_transport(view.related_nodes[i].header),
			"field_names", field_names_transport,
		NULL);
		
		g_ptr_array_add(related_nodes_transport, related_node_transport);
	}
	
	g_object_set(request_transport,
		"fields", fields_transport,
		"related_nodes", related_nodes_transport,
	NULL);
	
	return request_transport;
}

static struct Header_TRANSPORT* header_for_transport(struct Header header) {
	struct Header_TRANSPORT* header_transport = _g_object_new(header);
	printf("HEADER TAG -> %s\n", header.tag);
	g_object_set(header_transport, "tag", header.tag, "filter_not_null", header.filter_not_null, NULL);
	char* getted_header_tag; g_object_get(header_transport, "tag", &getted_header_tag, NULL);
	printf("GETTED HEADER TAG -> %s\n", getted_header_tag);
	
	if(header.filter_not_null) {
		struct Filter_TRANSPORT* filter_transport = filter_for_transport(header.filter);
		g_object_set(header_transport, "filter", filter_transport, NULL);
	}
	
	return header_transport;
}

static struct Logic_func_TRANSPORT* logic_func_for_transport(struct Logic_func* logic_func);

static struct Filter_TRANSPORT* filter_for_transport(struct Filter filter) {
	struct Filter_TRANSPORT* filter_transport = _g_object_new(filter);
	struct Filter_union_TRANSPORT* filter_union_transport = _g_object_new(filter_union);
	
	if(filter.is_native) {
		struct Native_filter_TRANSPORT* native_filter_transport = _g_object_new(native_filter);
		struct Native_filter* native_filter = filter.filter;
		g_object_set(native_filter_transport, 
			"name", native_filter->name, "opcode", native_filter->opcode, "value", value_for_transport(native_filter->value), NULL
		);
		g_object_set(filter_union_transport, "native_filter", native_filter_transport, NULL);
	} else {
		g_object_set(filter_union_transport, "func", logic_func_for_transport(filter.func), NULL);
	}
	
	g_object_set(filter_transport, "is_native", filter.is_native, "filter", filter_union_transport, NULL);
	return filter_transport;
}

static struct Logic_func_TRANSPORT* logic_func_for_transport(struct Logic_func* logic_func) {
	struct Logic_func_TRANSPORT* logic_func_transport = _g_object_new(logic_func);
	GPtrArray* filters_transport = g_ptr_array_new();
	
	for(size_t i = 0; i < logic_func->filters_count; i++) {
		g_ptr_array_add(filters_transport, filter_for_transport(logic_func->filters[i]));
	}
	
	g_object_set(logic_func_transport, "type", logic_func->type, "filters", filters_transport, NULL);
}

static struct Value_TRANSPORT* value_for_transport(struct Value value) {
	struct Value_TRANSPORT* result = _g_object_new(value);
	struct Value_union_TRANSPORT* result_union = _g_object_new(value_union);
	switch(value.type) {
		case STRING_TYPE: g_object_set(result_union, "String", value.string, NULL); break;
		case INTEGER_TYPE: g_object_set(result_union, "Integer", value.integer, NULL); break;
		case BOOLEAN_TYPE: g_object_set(result_union, "Boolean", value.boolean, NULL); break;
		default: assert(0);
	}
	g_object_set(result, "type", value.type, "value", result_union, NULL);
	return result;
}