option (ENABLE_FDB "Enable Cap'n Proto" ON)

if (ENABLE_FDB)
    # cmake 3.5.1 bug:
    # capnproto uses this cmake feature:
    # target_compile_features(kj PUBLIC cxx_constexpr)
    # old cmake adds -std=gnu++11 to end of all compile commands (even if -std=gnu++17 already present in compile string)
    # cmake 3.9.1 (ubuntu artful) have no this bug (c++17 support added to cmake 3.8.2)
    if (CMAKE_VERSION VERSION_LESS "3.12.0")
       set (USE_INTERNAL_FDB_LIBRARY_DEFAULT 0)
       set (MISSING_INTERNAL_FDB_LIBRARY 1)
    else ()
       set (USE_INTERNAL_FDB_LIBRARY_DEFAULT 1)
    endif ()

    option (USE_INTERNAL_FDB_LIBRARY "Set to FALSE to use system capnproto library instead of bundled" ${USE_INTERNAL_FDB_LIBRARY_DEFAULT})

    if (NOT EXISTS "${PROJECT_SOURCE_DIR}/contrib/fdb/CMakeLists.txt")
       if (USE_INTERNAL_FDB_LIBRARY)
           message (WARNING "submodule contrib/fdb is missing. to fix try run: \n git submodule update --init --recursive")
       endif ()
       set (USE_INTERNAL_FDB_LIBRARY 0)
       set (MISSING_INTERNAL_FDB_LIBRARY 1)
    endif ()

    if (NOT USE_INTERNAL_FDB_LIBRARY)
        set (FDB_PATHS "/usr/local/lib")
        set (FDB_INCLUDE_PATHS "/usr/local/include")
        find_library (FDB fdb_c PATHS ${FDB_PATHS})
        set (FDB_LIBRARY ${FDB})
        find_path (FDB_INCLUDE_DIR NAMES fdb_c_options.g.h fdb_c.h PATHS ${FDB_INCLUDE_PATHS})
    endif ()

    if (FDB_INCLUDE_DIR AND FDB_LIBRARY)
        set(USE_FDB 1)
    elseif (NOT MISSING_INTERNAL_FDB_LIBRARY)
        set (USE_INTERNAL_FDB_LIBRARY 1)
        set (FDB_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/contrib/fdb/bindings/c/foundationdb" "${CMAKE_BINARY_DIR}/contrib/fdb/bindings/c/foundationdb")
        set (FDB_LIBRARY fdb_c)
        set (USE_FDB 1)

    endif ()
endif ()

if (USE_FDB)
    message (STATUS "Using fdb=${USE_FDB}: ${FDB_INCLUDE_DIR} : ${FDB_LIBRARY}")
else ()
    message (STATUS "Build without fdb (support for foundation format will be disabled)")
endif ()




