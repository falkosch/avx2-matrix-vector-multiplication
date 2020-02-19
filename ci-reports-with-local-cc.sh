#!/usr/bin/env bash

TARGET="${TARGET:-ci}"
OUTPUT_DIR="${OUTPUT_DIR:-build/ci}"

. ./_build-env.sh

CONFIGURE_ADD_ARGS="${CONFIGURE_ADD_ARGS} -DCOVERAGE=\"1\""
BUILD_ADD_ARGS="${BUILD_ADD_ARGS} --clean-first"

. ./build-with-local-cc.sh
