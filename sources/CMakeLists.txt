add_subdirectory("scalar-variant")
add_subdirectory("avx2-variant")
add_subdirectory("avx2-variant-client")

add_custom_target(sources
    DEPENDS
        "avx2-variant-client"
)
