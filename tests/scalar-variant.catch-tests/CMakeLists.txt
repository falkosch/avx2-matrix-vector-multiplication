project("scalar-variant.catch-tests"
    LANGUAGES CXX
    VERSION 1.0.0
)

add_executable(${PROJECT_NAME}
    "src/catch_main.cpp"
    "src/test.cpp"
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
    "scalar-variant"
)

find_package(Catch2 REQUIRED)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        "scalar-variant"
        Catch2::Catch2
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(COVERAGE)
        setup_target_for_coverage_gcovr_html(
            NAME "${PROJECT_NAME}-coverage"
            BASE_DIRECTORY "${CMAKE_SOURCE_DIR}"
            EXECUTABLE ${PROJECT_NAME}
            DEPENDENCIES ${PROJECT_NAME}
        )
    endif()

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

else()
    message(FATAL_ERROR "no configuration for compiler ${CMAKE_CXX_COMPILER_ID}")

endif()
