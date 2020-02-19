#!/usr/bin/env bash

ARCH_TYPE="${ARCH_TYPE:-x64}"
BUILD_TYPE="${BUILD_TYPE:-Release}"
TARGET="${TARGET:-sources_and_tests}"

#TOOLCHAIN_FILE="${TOOLCHAIN_FILE:-${VCPKG_DIR}/scripts/buildsystems/vcpkg.cmake}"
CONFIGURE_ADD_ARGS="${CONFIGURE_ADD_ARGS:-}"
BUILD_ADD_ARGS="${BUILD_ADD_ARGS:-}"

OUTPUT_DIR="${OUTPUT_DIR:-build/${ARCH_TYPE}-local-cc-${BUILD_TYPE}}"
SOURCES_DIR="${SOURCES_DIR:-${OUTPUT_DIR}/sources}"
TESTS_DIR="${TESTS_DIR:-${OUTPUT_DIR}/tests}"

mkdir -p ${OUTPUT_DIR}
