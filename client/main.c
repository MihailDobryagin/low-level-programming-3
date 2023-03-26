#include <stdlib.h>
#include <stdio.h>
#include <thrift/c_glib/protocol/thrift_binary_protocol.h>
#include <thrift/c_glib/transport/thrift_buffered_transport.h>
#include <thrift/c_glib/transport/thrift_socket.h>

#include "../transport/gen-c_glib/d_b_request.h"
#include "../transport/gen-c_glib/schema_types.h"
#include "req_structure.h"
#include "transport.h"

static DBRequestIf* client;

int main(int argc, char * argv[])
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
