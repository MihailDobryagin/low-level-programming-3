#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <assert.h>

#include "req_utils.h"
#include "req_structure.h"

#define STRING_INITIAL_SIZE 1000

char* node_view_to_string(struct Node_view node) {
	char* result_str = malloc(STRING_INITIAL_SIZE + 10);
	sprintf(result_str, "NODE '%s' (", node.tag_name);
	size_t result_size = strlen(result_str);
	
	for(size_t i = 0; i < node.native_fields_count; i++) {
		struct Native_field field = node.native_fields[i];
		char* str_to_append = malloc(STRING_INITIAL_SIZE);
		
		char* value_buff = malloc(100);
		switch(field.value.type) {
			case STRING_TYPE: sprintf(value_buff, "\"%s\"", field.value.string); break;
			case INTEGER_TYPE: sprintf(value_buff, "%d", field.value.integer); break;
			case BOOLEAN_TYPE: sprintf(value_buff, field.value.boolean ? "True" : "False"); break;
			default: printf("UNKNOW VALUE TYPE"); assert(0);
		}
		
		
		sprintf(str_to_append, "%s: %s", field.name, value_buff);
		
		free(value_buff);
		
		if(i + 1 != node.native_fields_count) strcpy(str_to_append + strlen(str_to_append), ", ");
		
		size_t str_to_append_sz = strlen(str_to_append);
		strcpy(result_str + result_size, str_to_append);
		
		free(str_to_append);
		result_size += str_to_append_sz;
	}
	
	strcpy(result_str + result_size, ")");
	
	return result_str;
}

char* answer_to_string(struct Answer answer) {
	char* result_str = malloc((STRING_INITIAL_SIZE + 10) * answer.nodes_count);
	size_t result_size = 0;
	
	for(size_t i = 0; i < answer.nodes_count; i++) {
		char* node_as_str = node_view_to_string(answer.nodes[i]);
		size_t node_as_str_sz = strlen(node_as_str);
		sprintf(result_str + result_size, "%s\n", node_as_str);
		free(node_as_str);
		result_size += node_as_str_sz + 1;
	}
	
	return result_str;
}