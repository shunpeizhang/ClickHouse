


aux_source_directory(. DIR_LIB_SRCS)
aux_source_directory(./fdb DIR_LIB_SRCS)

add_library(IndexMetadataClient ${DIR_LIB_SRCS})
target_link_libraries(IndexMetadataClient ${FDB_LIBRARY})
target_include_directories(CoordinationClient PUBLIC "${FDB_INCLUDE_DIR}")

