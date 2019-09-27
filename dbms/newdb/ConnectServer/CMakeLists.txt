aux_source_directory(. DIR_LIB_SRCS)

add_executable(ConnectServer ${DIR_LIB_SRCS})
target_link_libraries(ConnectServer BaseCore Common CoordinationClient)
target_include_directories(ConnectServer PUBLIC ${CAPNP_INCLUDE_DIR} ${Poco_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/src/Base)



