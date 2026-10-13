add_pretty_parser(vmfpp
        DEPS kvpp
        SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vmfpp/VMF.h"
        "${CMAKE_CURRENT_LIST_DIR}/VMF.cpp")
