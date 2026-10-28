list(APPEND ${PROJECT_NAME}_math_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/math/Intersections.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/math/Math.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/math/Matrix.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/math/ParametricLine.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/math/Plane.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/sourcepp/math/Vec.h"
)

add_library(${PROJECT_NAME}_math STATIC
        ${${PROJECT_NAME}_math_HEADERS}
        "${CMAKE_CURRENT_LIST_DIR}/Intersections.cpp"
)

target_precompile_headers(${PROJECT_NAME}_math PUBLIC ${${PROJECT_NAME}_math_HEADERS})

target_link_libraries(${PROJECT_NAME}_math PUBLIC ${PROJECT_NAME})
