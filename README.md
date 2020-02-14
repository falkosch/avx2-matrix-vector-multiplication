# avx2-matrix-vector-multiplication

[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=avx2-matrix-vector-multiplication&metric=alert_status)](https://sonarcloud.io/dashboard?id=avx2-matrix-vector-multiplication)
[![Coverage](https://sonarcloud.io/api/project_badges/measure?project=avx2-matrix-vector-multiplication&metric=coverage)](https://sonarcloud.io/dashboard?id=avx2-matrix-vector-multiplication)
[![Lines of Code](https://sonarcloud.io/api/project_badges/measure?project=avx2-matrix-vector-multiplication&metric=ncloc)](https://sonarcloud.io/dashboard?id=avx2-matrix-vector-multiplication)

# What is it about?

`avx2-matrix-vector-multiplication` is a simple learning project about implementing an AVX2 based transformation of N-vectors by MxN-matrices. The implementation could be used e.g. for forward propagation of inputs in neural networks or simply for any large-scale matrix-vector-multiplication.

# Install dependencies first

* Run the script `install-dependencies-with-local-conan.sh` to install Catch2 into `build/Conan/x64`, or
* execute [Conan](https://conan.io/) at the repository root:
```
conan install . --install-folder "build/conan/x64" --build missing --setting arch=x86_64
```

If you do not have `conan` installed yet, the provided install script invokes `pip` to install `conan` for you.

# How to build

* Use the `build-with-local-cc.sh` script, or
* execute [cmake](https://cmake.org/) at the repository root:

```bash
cmake -B build -DCMAKE_VS_PLATFORM_NAME:STRING="x64" -DCMAKE_BUILD_TYPE="Release"

cmake --build build --target avx2-variant-client --config Release
```

# How to run

* Use the `run-with-local-cc.sh` script, or
* build manually with cmake as described above and execute `build/sources/avx2-variant-client/avx2-variant-client{.exe}`.

# Technical details

The project is split into `sources/` and `tests/` code. In `sources/` you will find the AVX2 based implementation as well as a classic scalar implementation. The classic scalar version is for validation purposes. Each version has a dedicated `*-client` executable to demonstrate their use.

Both implementations are backed by Catch2 BDD tests, which can be found in `tests/`.

To provide the Catch2 dependency for the build, the package manager [Conan](https://conan.io/) is used and integrated along with the CMakeLists scripts of the project.

The [CMake 3.x](https://cmake.org/) setup of the project supports a cross-platform build with currently GCC or MSBuild/MSVC compilers.

Static code analysis and coverage reports are collected by SonarScanner via a Jenkins Pipeline. The results are published on [sonarcloud.io](https://sonarcloud.io/dashboard?id=avx2-matrix-vector-multiplication).

There is also a `.clang-format` for proper code formatting in IDEs supporting [clang-tools](https://clang.llvm.org/docs/ClangFormat.html), e.g. VSCode.
