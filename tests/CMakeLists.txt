add_custom_target(tests)

enable_testing()

add_subdirectory("avx2-variant.native-tests")
