#!/usr/bin/env bash

TARGET="ci"
OUTPUT_DIR="build/ci"

CONFIGURE_ADD_ARGS=-DCOVERAGE="1"
BUILD_ADD_ARGS="--clean-first"

. ./build-with-local-cc.sh
