project("scalar-variant.catch-tests"
    LANGUAGES CXX
    VERSION 1.0.0
)

add_executable(${PROJECT_NAME}
    "src/catch_main.cpp"
    "src/test.cpp"
)

set(JUNIT_TARGET "${PROJECT_NAME}-junit")
add_custom_target(${JUNIT_TARGET}
    COMMAND
        ${PROJECT_NAME} -d yes -r junit -o "${JUNIT_TARGET}.xml"
    BYPRODUCTS
        "${JUNIT_TARGET}.xml"
    DEPENDS
        ${PROJECT_NAME}
)

add_custom_target("${PROJECT_NAME}-reports"
    DEPENDS
        ${JUNIT_TARGET}
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
        setup_target_for_coverage_gcovr_xml(
            NAME "${PROJECT_NAME}-cobertura"
            BASE_DIRECTORY "${CMAKE_SOURCE_DIR}"
            EXECUTABLE ${PROJECT_NAME} -r compact
            DEPENDENCIES ${PROJECT_NAME}
        )

        add_dependencies("${PROJECT_NAME}-reports"
            "${PROJECT_NAME}-cobertura"
        )
    endif()

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

else()
    message(FATAL_ERROR "no configuration for compiler ${CMAKE_CXX_COMPILER_ID}")

endif()
