# --------------- runs clang-format in place using the style file ---------------

if(CMAKE_SOURCE_DIR AND CLANG_FORMAT)
    # get C++ sources file list (ignoring packages)
    file(GLOB_RECURSE ALL_SOURCE_FILES
            ${PROJECT_SOURCE_DIR}/src/**.[hc]pp
            ${PROJECT_SOURCE_DIR}/include/**.[hc]pp
            ${PROJECT_SOURCE_DIR}/tests/src/**.[hc]pp
            ${PROJECT_SOURCE_DIR}/tests/include/**.[hc]pp
            )

    # apply style to the file list
    foreach(SOURCE_FILE ${ALL_SOURCE_FILES})
        execute_process(COMMAND "${CLANG_FORMAT}" -style=file -verbose -i "${SOURCE_FILE}")
    endforeach()
endif()
