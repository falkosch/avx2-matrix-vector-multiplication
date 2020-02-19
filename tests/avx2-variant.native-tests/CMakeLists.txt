project("avx2-variant.native-tests"
    LANGUAGES CXX
    VERSION 1.0.0
)

add_library(${PROJECT_NAME}
    SHARED
        "src/avx2_variant_test.cpp"
)

source_group(
    TREE ${CMAKE_CURRENT_SOURCE_DIR}
    PREFIX "Files"
    REGULAR_EXPRESSION ".+\\.(h|cpp)"
)

add_dependencies(${PROJECT_NAME}
    avx2-variant
)

target_link_directories(${PROJECT_NAME}
    PRIVATE
        "$ENV{VCInstallDir}/Auxiliary/VS/UnitTest/lib"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        avx2-variant
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        "$ENV{VCInstallDir}/Auxiliary/VS/UnitTest/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
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
