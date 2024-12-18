# Load the debug and release variables
file(GLOB DATA_FILES "${CMAKE_CURRENT_LIST_DIR}/fmt-*-data.cmake")

foreach(f ${DATA_FILES})
    include(${f})
endforeach()

# Create the targets for all the components
foreach(_COMPONENT ${fmt_COMPONENT_NAMES} )
    if(NOT TARGET ${_COMPONENT})
        add_library(${_COMPONENT} INTERFACE IMPORTED)
        message(${fmt_MESSAGE_MODE} "Conan: Component target declared '${_COMPONENT}'")
    endif()
endforeach()

if(NOT TARGET fmt::fmt)
    add_library(fmt::fmt INTERFACE IMPORTED)
    message(${fmt_MESSAGE_MODE} "Conan: Target declared 'fmt::fmt'")
endif()
# Load the debug and release library finders
file(GLOB CONFIG_FILES "${CMAKE_CURRENT_LIST_DIR}/fmt-Target-*.cmake")

foreach(f ${CONFIG_FILES})
    include(${f})
endforeach()