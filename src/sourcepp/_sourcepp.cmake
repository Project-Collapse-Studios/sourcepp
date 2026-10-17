list(APPEND ${PROJECT_NAME}_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/FS.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/Macros.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/Math.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/MathExtended.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/String.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/StringReference.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/IDGenerator.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/Templates.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/Math/ParametricLine.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/Math/Intersections.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/Math/Plane.h"
)

add_library(${PROJECT_NAME} STATIC
        ${${PROJECT_NAME}_HEADERS}
        "${CMAKE_CURRENT_LIST_DIR}/FS.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/String.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/IDGenerator.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/StringReference.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Math/Intersections.cpp"
)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_precompile_headers(${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_HEADERS})

target_link_libraries(${PROJECT_NAME} PUBLIC bufferstream sourcepp_half)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
