#include "transport.h"
#include "assert.h"

#define _g_object_type_for_transport(OBJECT_NAME) OBJECT_NAME##__t_r_a_n_s_p_o_r_t_get_type()
#define _g_object_new(OBJECT_NAME) g_object_new(_g_object_type_for_transport(OBJECT_NAME), NULL)

static struct Value_TRANSPORT* value_for_transport_from_value(struct Value value);
static struct Filter_TRANSPORT* filter_for_transport_from_filter_tree(struct Filter filter);
static struct Header_TRANSPORT* header_for_transport_from_header(struct Header header);

Request_TRANSPORT* transport_request_from_view_format(struct View view) {
	Request_TRANSPORT* request_transport = g_object_new(_g_object_type_for_transport(request), NULL);
	g_object_set(request_transport, "operation", view.operation, NULL);	
	
	
}

static struct Header_TRANSPORT* header_for_transport_from_header(struct Header header) {
	struct Header_TRANSPORT* header_transport = _g_object_new(header);
	g_object_set(header_transport, "tag", header.tag, "filter_not_null", header.filter_not_null, NULL);
	
	if(header.filter_not_null) {
		struct Filter_TRANSPORT* filter_transport = filter_for_transport_from_filter_tree(header.filter);
		g_object_set(header_transport, "filter", filter_transport, NULL);
	}
	
	return header_transport;
}

static struct Filter_TRANSPORT* filter_for_transport_from_filter_tree(struct Filter filter) {
	struct Filter_TRANSPORT* filter_transport = _g_object_new(filter);
	struct Filter_union_TRANSPORT* filter_union_transport = _g_object_new(filter_union);
	
	if(filter.is_native) {
		struct Native_filter_TRANSPORT* native_filter_transport = _g_object_new(native_filter);
		struct Native_filter* native_filter = filter.filter;
		g_object_set(native_filter_transport, 
			"name", native_filter->name, "opcode", native_filter->opcode, "value", value_for_transport_from_value(native_filter->value), NULL
		);
		g_object_set(filter_union_transport, "native_filter", native_filter_transport, NULL);
	}
	
	g_object_set(filter_transport, "is_native", filter.is_native, "filter", filter_union_transport, NULL);
	return filter_transport;
}

static struct Value_TRANSPORT* value_for_transport_from_value(struct Value value) {
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