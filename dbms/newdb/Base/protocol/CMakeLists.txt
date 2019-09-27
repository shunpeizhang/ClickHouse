#set(ENV{PATH} ${UltraDB_SOURCE_DIR}/tools/capaproto:$ENV{PATH})
#execute_process(COMMAND ${UltraDB_SOURCE_DIR}/tools/capaproto/capnp -I${UltraDB_SOURCE_DIR}/third_party/capnproto/c++/src compile --src-prefix=${UltraDB_SOURCE_DIR}/src/Base/protocol -oc++:${UltraDB_SOURCE_DIR}/src/Base/protocol ${UltraDB_SOURCE_DIR}/src/Base/protocol/base.capnp)



aux_source_directory(. DIR_LIB_SRCS)

add_library(protocol ${DIR_LIB_SRCS})
target_link_libraries(protocol ${CAPNP_LIBRARY})
target_include_directories(protocol PUBLIC ${CAPNP_INCLUDE_DIR})
