enable_testing()

add_subdirectory("avx2-variant.catch-tests")
add_subdirectory("scalar-variant.catch-tests")

add_subdirectory("avx2-variant.native-tests")

add_custom_target(tests
    DEPENDS
        "avx2-variant.catch-tests"
        "scalar-variant.catch-tests"
)

add_custom_target(reports
    DEPENDS
        "avx2-variant.catch-tests-reports"
        "scalar-variant.catch-tests-reports"
)

