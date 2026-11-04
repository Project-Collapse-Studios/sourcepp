add_pretty_parser(vmfpp
        DEPS sourcepp_parser sourcepp::kvpp
        SOURCES
        # HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vmfpp/VMF.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vmfpp/EditorInfo.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/vmfpp/VMFEntity.h"

        # SOURCE FILES
        "${CMAKE_CURRENT_LIST_DIR}/VMF.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/VMFEntity.cpp"
)
