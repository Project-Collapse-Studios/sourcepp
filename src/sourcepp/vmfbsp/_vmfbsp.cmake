list(APPEND ${PROJECT_NAME}_vmfbsp_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/vmfbsp/Brush.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/vmfbsp/BrushFace.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/vmfbsp/DisplacementInfo.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/vmfbsp/EditorInfo.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/vmfbsp/Entity.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/vmfbsp/Material.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/vmfbsp/Output.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/vmfbsp/VMFBSP.h"
)

add_library(${PROJECT_NAME}_vmfbsp STATIC
        ${${PROJECT_NAME}_vmfbsp_HEADERS}
        "${CMAKE_CURRENT_LIST_DIR}/Brush.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/BrushFace.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/DisplacementInfo.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Entity.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Material.cpp"
)

target_precompile_headers(${PROJECT_NAME}_vmfbsp PUBLIC ${${PROJECT_NAME}_vmfbsp_HEADERS})

target_link_libraries(${PROJECT_NAME}_vmfbsp PUBLIC ${PROJECT_NAME} sourcepp_math)
