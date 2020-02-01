cmake_minimum_required(VERSION 3.13.0 FATAL_ERROR)

project("scalar-variant"
    LANGUAGES CXX
    VERSION 1.0.0
)

add_library(${PROJECT_NAME}
    STATIC
        "src/scalar-variant.cpp"
)

add_dependencies(${PROJECT_NAME}
    sources-build-aggregate
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        sources-build-aggregate
)

source_group(
    TREE ${CMAKE_CURRENT_SOURCE_DIR}
    PREFIX "Files"
    REGULAR_EXPRESSION ".+\\.(h|cpp)"
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
    set_target_properties(${PROJECT_NAME}
        PROPERTIES
            INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE
    )

else()
    message(FATAL_ERROR "no configuration for compiler ${CMAKE_CXX_COMPILER_ID}")

endif()
