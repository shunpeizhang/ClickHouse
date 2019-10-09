aux_source_directory(. DIR_LIB_SRCS)

add_executable(ComputeServer ${DIR_LIB_SRCS})
target_link_libraries(ComputeServer BaseCore Common CoordinationClient)
target_include_directories(ComputeServer PUBLIC ${CAPNP_INCLUDE_DIR} ${Poco_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/dbms/newdb/Base)



