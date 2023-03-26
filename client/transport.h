#ifndef TRANSPORT_H
#define TRANSPORT_H

#include "../transport/gen-c_glib/schema_types.h"
#include "req_structure.h"

Request_TRANSPORT* transport_request_from_view_format(struct View view);

#endif // !TRANSPORT_H
