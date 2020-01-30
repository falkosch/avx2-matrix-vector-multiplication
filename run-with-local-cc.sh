#!/usr/bin/env bash

. ./build-with-local-cc.sh

find "$(pwd)/${OUTPUT_DIR}/sources/avx2-variant-client" -executable -type f -print -exec {} \;
