#!/usr/bin/env bash

ARCH_TYPE="${ARCH_TYPE:-x64}"
BUILD_TYPE="${BUILD_TYPE:-Release}"
OUTPUT_DIR="${OUTPUT_DIR:-build/${ARCH_TYPE}-local-cc-${BUILD_TYPE}}"
TARGET="${TARGET:-sources_and_tests}"

CONFIGURE_ADD_ARGS="${CONFIGURE_ADD_ARGS:-}"
BUILD_ADD_ARGS="${BUILD_ADD_ARGS:-}"

mkdir -p ${OUTPUT_DIR}
