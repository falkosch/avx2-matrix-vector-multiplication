#!/usr/bin/env bash

. _build-env.sh

EXE_FILE="$(pwd)/${OUTPUT_DIR}/sources/avx2-variant-client/avx2-variant-client{.exe}"

(cd data && exec ${EXE_FILE})
