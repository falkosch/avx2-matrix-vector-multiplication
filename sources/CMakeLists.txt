add_subdirectory("sources-build-aggregate")
add_subdirectory("scalar-variant")
add_subdirectory("scalar-variant-client")
add_subdirectory("avx2-model")
add_subdirectory("avx2-variant")
add_subdirectory("avx2-variant-client")
add_subdirectory("avx2-variant-mt")
add_subdirectory("avx2-variant-mt-client")

add_custom_target(sources
    DEPENDS
        "scalar-variant-client"
        "avx2-variant-client"
        "avx2-variant-mt-client"
)
