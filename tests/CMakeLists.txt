enable_testing()

add_subdirectory("avx2-variant.native-tests")
add_subdirectory("scalar-variant.catch-tests")

add_custom_target(tests
    DEPENDS
        scalar-variant.catch-tests
)

add_custom_target(reports
    DEPENDS
        "scalar-variant.catch-tests-reports"
)

