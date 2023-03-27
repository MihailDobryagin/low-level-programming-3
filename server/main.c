#include <stdlib.h>
#include <stdio.h>
#include <glib-object.h>
#include <signal.h>
#include <thrift/c_glib/thrift.h>
#include <thrift/c_glib/protocol/thrift_binary_protocol_factory.h>
#include <thrift/c_glib/protocol/thrift_protocol_factory.h>
#include <thrift/c_glib/server/thrift_server.h>
#include <thrift/c_glib/server/thrift_simple_server.h>
#include <thrift/c_glib/transport/thrift_buffered_transport_factory.h>
#include <thrift/c_glib/transport/thrift_server_socket.h>
#include <thrift/c_glib/transport/thrift_server_transport.h>

#include "../transport/gen-c_glib/d_b_request.h"
#include "../transport/gen-c_glib/schema_types.h"
#include "req_structure.h"
#include "requests.h"
#include "transport.h"

#include "database/db/db.h"



// ================ START OF DECLARATIONS ================

G_BEGIN_DECLS

#define TYPE_D_B_REQUEST_MY_HANDLER \
  (d_b_request_my_handler_impl_get_type ())

#define D_B_REQUEST_MY_HANDLER(obj)                                \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj),                                   \
                               TYPE_D_B_REQUEST_MY_HANDLER,        \
                               DBRequestMyHandlerImp))

#define D_B_REQUEST_MY_HANDLER_CLASS(c)                    \
  (G_TYPE_CHECK_CLASS_CAST ((c),                                \
                            TYPE_D_B_REQUEST_MY_HANDLER,   \
                            DBRequestMyHandlerImpClass))

#define IS_D_B_REQUEST_MY_HANDLER(obj)                             \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),                                   \
                               TYPE_D_B_REQUEST_MY_HANDLER))

#define IS_D_B_REQUEST_MY_HANDLER_CLASS(c)                 \
  (G_TYPE_CHECK_CLASS_TYPE ((c),                                \
                            TYPE_D_B_REQUEST_MY_HANDLER))

#define D_B_REQUEST_MY_HANDLER_GET_CLASS(obj)              \
  (G_TYPE_INSTANCE_GET_CLASS ((obj),                            \
                              TYPE_D_B_REQUEST_MY_HANDLER, \
                              DBRequestMyHandlerImpClass))

struct _DBRequestMyHandlerImpl {
    DBRequestHandlerClass parent_instance;
};
typedef struct _DBRequestMyHandlerImpl DBRequestMyHandlerImp;

struct _DBRequestMyHandlerImpClass {
    DBRequestHandlerClass parent_class;
};

typedef struct _DBRequestMyHandlerImpClass DBRequestMyHandlerImpClass;

GType d_b_request_my_handler_impl_get_type(void);

G_END_DECLS


// ================ END OF DECLARATIONS ================

// ================ START OF IMPLEMENTATION ================

Database* database;

G_DEFINE_TYPE(DBRequestMyHandlerImp,
              d_b_request_my_handler_impl,
              TYPE_D_B_REQUEST_HANDLER
)

static void
d_b_request_my_handler_impl_finalize (GObject *object)
{
    DBRequestMyHandlerImp *self =
    D_B_REQUEST_MY_HANDLER(object);

    G_OBJECT_CLASS (d_b_request_my_handler_impl_parent_class)->
            finalize (object);
}

static void
d_b_request_my_handler_impl_init (DBRequestMyHandlerImp *self)
{
}


static gboolean handler_ping (DBRequestIf *iface, GError **error){
	THRIFT_UNUSED_VAR (iface);
	THRIFT_UNUSED_VAR (error);

	printf("ping()\n");

	return TRUE;
}

static gboolean handler_do_request (DBRequestIf *iface, Answer_TRANSPORT **_return, const Request_TRANSPORT *req, GError **error) {
	printf("request()\n");
	THRIFT_UNUSED_VAR (iface);
	THRIFT_UNUSED_VAR (error);
	struct View view = request_from_transport_request(req);
	printf("Tag -> %s\n", view.header.tag);
	struct Answer answer = do_request(database, view);
	
	return TRUE;
}

static void
d_b_request_my_handler_impl_class_init (DBRequestMyHandlerImpClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    DBRequestHandlerClass *handler_klass =
    D_B_REQUEST_HANDLER_CLASS(klass);

    gobject_class->finalize = d_b_request_my_handler_impl_finalize;

    handler_klass->ping = handler_ping;
    handler_klass->do_request = handler_do_request;
}

// ================ END OF IMPLEMENTATION ================

int main (int argc, char * argv[])
{
	size_t port = 9090;
	database = init_database("db_file.txt");

	DBRequestProcessor *processor;

	DBRequestMyHandlerImp *handler;

	ThriftServerTransport *server_transport;
	ThriftTransportFactory *transport_factory;
	ThriftProtocolFactory *protocol_factory;
	ThriftServer *server = NULL;

	GError *error = NULL;
	int exit_status = 0;

	#if (!GLIB_CHECK_VERSION (2, 36, 0))
		g_type_init ();
	#endif

	handler = g_object_new (TYPE_D_B_REQUEST_MY_HANDLER, NULL);

	processor = g_object_new (TYPE_D_B_REQUEST_PROCESSOR, "handler", handler, NULL);

	server_transport = g_object_new (THRIFT_TYPE_SERVER_SOCKET, "port", port, NULL);

	transport_factory = g_object_new (THRIFT_TYPE_BUFFERED_TRANSPORT_FACTORY, NULL);

	protocol_factory = g_object_new (THRIFT_TYPE_BINARY_PROTOCOL_FACTORY, NULL);

	server = g_object_new (THRIFT_TYPE_SIMPLE_SERVER,
		"processor",                processor,
		"server_transport",         server_transport,
		"input_transport_factory",  transport_factory,
		"output_transport_factory", transport_factory,
		"input_protocol_factory",   protocol_factory,
		"output_protocol_factory",  protocol_factory,
	NULL);

	puts ("Starting the server...");
	thrift_server_serve (server, &error);

	puts ("done.");

	g_object_unref (server);
	g_object_unref (transport_factory);
	g_object_unref (protocol_factory);
	g_object_unref (server_transport);

	g_object_unref (processor);
	g_object_unref (handler);

	return exit_status;
}
