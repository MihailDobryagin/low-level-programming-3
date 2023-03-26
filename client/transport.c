#include "transport.h"

#define _g_object_type_for_transport(OBJECT_NAME) OBJECT_NAME##__t_r_a_n_s_p_o_r_t_get_type()

Request_TRANSPORT* transport_request_from_view_format(struct View view) {
	Request_TRANSPORT* request_transport = g_object_new(_g_object_type_for_transport(request), NULL);
}