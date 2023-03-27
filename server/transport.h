#ifndef TRANSPORT_H
#define TRANSPORT_H

#include "../transport/gen-c_glib/schema_types.h"
#include "req_structure.h"

struct View request_from_transport_request(Request_TRANSPORT* req);

Answer_TRANSPORT* answer_for_transport(struct Answer answer);

#endif // !TRANSPORT_H
