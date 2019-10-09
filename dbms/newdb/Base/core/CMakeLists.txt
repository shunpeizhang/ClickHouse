

aux_source_directory(. DIR_LIB_SRCS)


add_library(BaseCore ${DIR_LIB_SRCS})
target_link_libraries(BaseCore ${CoordinationClient} ${CAPNP_LIBRARY} ${Poco_Foundation_LIBRARY} ${Poco_Util_LIBRARY} ${Poco_Net_LIBRARY} ${Poco_JSON_LIBRARY} ${Common})
target_include_directories(BaseCore PUBLIC ${CAPNP_INCLUDE_DIR} ${Poco_INCLUDE_DIRS} ${GRPC_INCLUDE_DIR} ${Protobuf_INCLUDE_DIR})








