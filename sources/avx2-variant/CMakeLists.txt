cmake_minimum_required(VERSION 3.13.0 FATAL_ERROR)

project("avx2-variant"
    LANGUAGES CXX
    VERSION 1.0.0
)

add_library(${PROJECT_NAME}
    STATIC
        "src/avx2-variant.cpp"
)

source_group(
    TREE ${CMAKE_CURRENT_SOURCE_DIR}
    PREFIX "Files"
    REGULAR_EXPRESSION ".+\\.(h|cpp)"
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE
)

add_dependencies(${PROJECT_NAME}
    scalar-variant
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        scalar-variant
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        _LIB
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

else()
    message(FATAL_ERROR "no configuration for compiler ${CMAKE_CXX_COMPILER_ID}")

endif()
