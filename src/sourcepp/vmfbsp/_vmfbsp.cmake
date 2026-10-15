list(APPEND ${PROJECT_NAME}_vmfbsp_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/vmfbsp/Entity.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/vmfbsp/Output.h"
)

add_library(${PROJECT_NAME}_vmfbsp STATIC
        ${${PROJECT_NAME}_vmfbsp_HEADERS}
        "${CMAKE_CURRENT_LIST_DIR}/Entity.cpp"
)

target_precompile_headers(${PROJECT_NAME}_vmfbsp PUBLIC ${${PROJECT_NAME}_vmfbsp_HEADERS})

target_link_libraries(${PROJECT_NAME}_vmfbsp PUBLIC ${PROJECT_NAME})
