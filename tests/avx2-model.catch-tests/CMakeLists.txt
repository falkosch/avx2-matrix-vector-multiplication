project("avx2-model.catch-tests"
    LANGUAGES CXX
    VERSION 1.0.0
)

add_executable(${PROJECT_NAME}
    "src/catch_main.cpp"
    "src/modifiable-avx2-matrices.cpp"
    "src/modifiable-avx2-vectors.cpp"
    "src/scalable-avx2-matrices.cpp"
    "src/scalable-avx2-vectors.cpp"
)

source_group(
    TREE ${CMAKE_CURRENT_SOURCE_DIR}
    PREFIX "Files"
    REGULAR_EXPRESSION ".+\\.(h|cpp)"
)

add_dependencies(${PROJECT_NAME}
    "avx2-model"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        "avx2-model"
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

add_catch2_and_reporting_targets(
    NAME "${PROJECT_NAME}-reports"
    TARGET ${PROJECT_NAME}
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

else()
    message(FATAL_ERROR "no configuration for compiler ${CMAKE_CXX_COMPILER_ID}")

endif()
