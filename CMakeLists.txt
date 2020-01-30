cmake_minimum_required(VERSION 3.9)

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "")
    set(CMAKE_VS_PLATFORM_NAME "x64")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/build/conan/${CMAKE_VS_PLATFORM_NAME}")

project("avx2-matrix-vector-multiplication")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_compile_definitions(
    UNICODE
    _UNICODE
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(
        -g3
        -march=native
        -fopenmp
        -Wall
        -Wextra
        -pedantic
        -Wabi
        -Wabi-tag
        -Waggregate-return
        -Walloc-zero
        -Walloca
        -Wc++11-compat
        -Wc++14-compat
        -Wc++1z-compat
        -Wcast-align
        -Wcast-qual
        -Wconditionally-supported
        -Wconversion
        -Wctor-dtor-privacy
        -Wdate-time
        -Wdelete-non-virtual-dtor
        -Wdisabled-optimization
        -Wdouble-promotion
        -Wduplicated-branches
        -Wduplicated-cond
        -Weffc++
        -Wfloat-conversion
        -Wfloat-equal
        -Wformat-nonliteral
        -Wformat-security
        -Wformat-signedness
        -Wformat-y2k
        -Winit-self
        -Winline
        -Winvalid-pch
        -Wlogical-op
        -Wmissing-include-dirs
        -Wmultichar
        -Wmultiple-inheritance
        -Wnoexcept
        -Wnoexcept-type
        -Wnon-virtual-dtor
        -Wnull-dereference
        -Wold-style-cast
        -Woverloaded-virtual
        -Wpacked
        -Wpadded
        -Wredundant-decls
        -Wregister
        -Wreorder
        -Wrestrict
        -Wshadow
        -Wshadow=compatible-local
        -Wshadow=local
        -Wsign-conversion
        -Wsign-promo
        -Wstack-protector
        -Wstrict-null-sentinel
        -Wsuggest-attribute=format
        -Wsuggest-attribute=noreturn
        -Wsuggest-final-methods
        -Wsuggest-final-types
        -Wsuggest-override
        -Wswitch-default
        -Wswitch-enum
        -Wsynth
        -Wtrampolines
        -Wundef
        -Wunsafe-loop-optimizations
        -Wunused-macros
        -Wuseless-cast
        -Wvector-operation-performance
        -Wvirtual-inheritance
        -Wwrite-strings
        -Wzero-as-null-pointer-constant
    )

    add_link_options(
        -g3
        -fopenmp
    )

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
    )
    foreach(CompilerFlag ${CompilerFlags})
        string(REGEX REPLACE "/[wW][0-9]" "" ${CompilerFlag} "${${CompilerFlag}}")
        string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
    endforeach()

    add_compile_definitions(
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )

    add_compile_options(
        /Zi
        /openmp
        /arch:AVX2
        /permissive-
        /Zc:twoPhase-
        /W4
    )

    add_link_options(
        /Debug
    )

else()
    message(FATAL_ERROR "no configuration for compiler ${CMAKE_CXX_COMPILER_ID}")

endif()

add_subdirectory(sources)
add_subdirectory(tests)

add_custom_target(sources_and_tests
    DEPENDS
        sources
        tests
)
