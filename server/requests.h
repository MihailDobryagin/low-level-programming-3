#ifndef REQUESTS
#define REQUESTS

#include "req_structure.h"

#include "database/client/queries.h"
#include "database/db/entities.h"
#include "database/db/db.h"

Array_node do_request(Database* db, struct View view);

#endif // !REQUESTS