# avx2-matrix-vector-multiplication

This is a simple learning project about implementing an AVX2 based MxN-matrix-N-vector-multiplication.

# How to build

1. Use the `build-with-local-cc.sh` script, or
2. execute cmake at the repository root:
```bash
cmake -B build -DCMAKE_VS_PLATFORM_NAME:STRING="x64" -DCMAKE_BUILD_TYPE="Release"

cmake --build build --clean-first --target avx2-variant-client --config Release
```

# How to run

1. Use the `run-with-local-cc.sh` script, or
2. build manually with cmake as described above and execute `build/sources/avx2-variant-client/avx2-variant-client{.exe}`.
