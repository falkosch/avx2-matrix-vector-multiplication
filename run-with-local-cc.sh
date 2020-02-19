#!/usr/bin/env bash

. ./build-with-local-cc.sh

SOURCES_DIR="$(pwd)/${OUTPUT_DIR}/sources"

find "${SOURCES_DIR}" -executable -type f -print -exec {} \;
