cmake_minimum_required(VERSION 3.13.0 FATAL_ERROR)

project("avx2-variant-client"
    LANGUAGES CXX
    VERSION 1.0.0
)

add_executable(${PROJECT_NAME}
    "src/avx2-variant-client.cpp"
)

source_group(
    TREE ${CMAKE_CURRENT_SOURCE_DIR}
    PREFIX "Files"
    REGULAR_EXPRESSION ".+\\.(h|cpp)"
)

add_dependencies(${PROJECT_NAME}
    scalar-variant
    avx2-variant
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        avx2-variant
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        _CONSOLE
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(${PROJECT_NAME}
        PRIVATE
            stdc++
    )

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_target_properties(${PROJECT_NAME}
        PROPERTIES
            INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE
    )

    target_link_options(${PROJECT_NAME}
        PRIVATE
            /SUBSYSTEM:CONSOLE
    )

else()
    message(FATAL_ERROR "no configuration for compiler ${CMAKE_CXX_COMPILER_ID}")

endif()
