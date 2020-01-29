#!/usr/bin/env bash

. ./build-with-local-cc.sh

find "$(pwd)/${OUTPUT_DIR}/tests" -executable -type f -print -exec {} \;
