#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>
#include <thrift/c_glib/protocol/thrift_binary_protocol.h>
#include <thrift/c_glib/transport/thrift_buffered_transport.h>
#include <thrift/c_glib/transport/thrift_socket.h>

#include "parser/y.tab.h"
#include "parser/lex.h"

#include "../transport/gen-c_glib/d_b_request.h"
#include "../transport/gen-c_glib/schema_types.h"
#include "req_structure.h"
#include "transport.h"

static DBRequestIf* client;

static char* _scan_query() {
	bool is_end = false;
	uint32_t query_capacity = 1010;
	uint32_t query_size = 0;
	char* query_str = (char*)malloc(query_capacity);
	
	uint32_t str_buff_capacity = 1000;
	char* str_buff = (char*)malloc(str_buff_capacity);
	
	while(!is_end) {
		fgets(str_buff, str_buff_capacity, stdin);
		if(strcmp("---END---\n", str_buff) == 0) {
			is_end = true;
			break;
		}
		
		uint32_t str_buff_len = strlen(str_buff);
		
		if(query_size + str_buff_len >= query_capacity) {
			query_capacity += str_buff_capacity + 1;
			realloc(query_str, query_capacity);
		}
		
		strcpy(query_str + query_size, str_buff);
		query_size += str_buff_len;
	}
}

int main (int argc, char** argv) {
	while(1) {
		char* query = _scan_query();
		yy_scan_string(query);
		struct View view;
		yyparse(&view);
		yylex_destroy();
	}
	return 0;
}

int stub_main(int argc, char * argv[])
{
  char* host = "localhost";
  size_t port = 9090;
  
  ThriftSocket *socket;
  ThriftTransport *transport;
  ThriftProtocol *protocol;

  GError *error = NULL;

  int exit_status = 0;

#if (!GLIB_CHECK_VERSION (2, 36, 0))
  g_type_init ();
#endif

  socket    = g_object_new (THRIFT_TYPE_SOCKET,
                            "hostname",  host,
                            "port",      port,
                            NULL);
  transport = g_object_new (THRIFT_TYPE_BUFFERED_TRANSPORT,
                            "transport", socket,
                            NULL);
  protocol  = g_object_new (THRIFT_TYPE_BINARY_PROTOCOL,
                            "transport", transport,
                            NULL);

  thrift_transport_open (transport, &error);

  client = g_object_new (TYPE_D_B_REQUEST_CLIENT,
                         "input_protocol",  protocol,
                         "output_protocol", protocol,
                         NULL);

  if (!error && d_b_request_if_ping(client, &error)) {
    printf("Connected\n");
  }
  else
  {
    printf("Сonneсtion error\n");
  }


  thrift_transport_close (transport, NULL);

  g_object_unref (client);
  g_object_unref (protocol);
  g_object_unref (transport);
  g_object_unref (socket);

  return exit_status;
}
