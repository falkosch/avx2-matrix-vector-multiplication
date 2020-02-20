enable_testing()

add_subdirectory("scalar-variant.catch-tests")
add_subdirectory("avx2-model.catch-tests")
add_subdirectory("avx2-variant.catch-tests")
add_subdirectory("avx2-variant-mt.catch-tests")

add_custom_target(tests
    DEPENDS
        "scalar-variant.catch-tests"
        "avx2-model.catch-tests"
        "avx2-variant.catch-tests"
        "avx2-variant-mt.catch-tests"
)

add_custom_target(reports
    DEPENDS
        "scalar-variant.catch-tests-reports"
        "avx2-model.catch-tests-reports"
        "avx2-variant.catch-tests-reports"
        "avx2-variant-mt.catch-tests-reports"
)
