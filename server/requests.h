#ifndef REQUESTS
#define REQUESTS

#include "req_structure.h"

#include "database/client/queries.h"
#include "database/db/entities.h"
#include "database/db/db.h"

struct Answer do_request(Database* db, struct View view);

#endif // !REQUESTS