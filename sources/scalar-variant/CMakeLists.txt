cmake_minimum_required(VERSION 3.13.0 FATAL_ERROR)

project("scalar-variant"
    LANGUAGES CXX
    VERSION 1.0.0
)

add_library(${PROJECT_NAME}
    STATIC
        "src/scalar-variant.cpp"
)

source_group(
    TREE ${CMAKE_CURRENT_SOURCE_DIR}
    PREFIX "Files"
    REGULAR_EXPRESSION ".+\\.(h|cpp)"
)

target_compile_features(${PROJECT_NAME}
    PUBLIC
        cxx_std_17
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        $<$<STREQUAL:${CMAKE_VS_PLATFORM_NAME},x64>:
            X64
        >
        $<$<STREQUAL:${CMAKE_VS_PLATFORM_NAME},x86>:
            X86
        >
        $<$<CONFIG:Release>:
            NDEBUG
        >
        $<$<CONFIG:Debug>:
            _DEBUG
            DEBUG
        >
    PRIVATE
        _LIB
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${PROJECT_NAME}
        PUBLIC
            $<$<CONFIG:Release>:
                -O3
            >
            $<$<CONFIG:Debug>:
                -Og
                -g2
            >
    )

    target_link_options(${PROJECT_NAME}
        PUBLIC
            $<$<CONFIG:Release>:
                -O3
                -flto
                -fwhole-program
            >
    )

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_link_options(${PROJECT_NAME}
        PUBLIC
            $<$<CONFIG:Release>:
                /OPT:REF
                /OPT:ICF
            >
    )

else()
    message(FATAL_ERROR "no configuration for compiler ${CMAKE_CXX_COMPILER_ID}")

endif()
