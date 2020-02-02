#!/usr/bin/env bash

. ./build-with-local-cc.sh

TESTS_DIR="$(pwd)/${OUTPUT_DIR}/tests"

find "${SOURCES_DIR}" -executable -type f -print -exec {} -d yes \;
