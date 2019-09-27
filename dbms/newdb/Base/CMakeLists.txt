include_directories ("./")

#execute_process(COMMAND chmod 755 ${UltraDB_SOURCE_DIR}/tools/capaproto/capnp)
#execute_process(COMMAND chmod 755 ${UltraDB_SOURCE_DIR}/tools/capaproto/capnpc-c++)

add_subdirectory(protocol)
add_subdirectory(CoordinationClient)
add_subdirectory(IndexMetadataClient)
add_subdirectory(core)




