cmake_minimum_required(VERSION 3.0.0)
project(LLP3 C)

set(THRIFT_LIB thrift_c_glib)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0 gobject-2.0)

include_directories("/usr/local/include/thrift/" "/usr/include/glib-2.0/*" "/usr/include/glib/*" "/usr/lib/x86_64-linux-gnu/glib-2.0/include/*" ${GLIB_INCLUDE_DIRS})
LINK_DIRECTORIES(${GLIB_LIBRARY_DIRS})

include_directories(${GLIB_INCLUDE_DIRS})
link_libraries(${THRIFT_LIB} ${GLIB_LDFLAGS})


add_executable(_client client/main.c client/transport.c transport/gen-c_glib/d_b_request.c transport/gen-c_glib/schema_types.c)
TARGET_LINK_LIBRARIES(_client ${THRIFT_LIB} ${GLIB_LIBRARIES})


add_executable(_server 
	server/main.c server/transport.c 
	server/database/client/manage.c server/database/db/db.c server/database/db/file.c server/database/db/entities.c
	transport/gen-c_glib/d_b_request.c transport/gen-c_glib/schema_types.c
)
TARGET_LINK_LIBRARIES(_client ${THRIFT_LIB} ${GLIB_LIBRARIES})

add_definitions (${GLIB_CFLAGS_OTHER})
