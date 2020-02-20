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

add_dependencies(${PROJECT_NAME}
    "avx2-model"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        "avx2-model"
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        _LIB
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE
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
