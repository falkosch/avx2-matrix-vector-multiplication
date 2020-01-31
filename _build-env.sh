#!/usr/bin/env bash

ARCH_TYPE="${ARCH_TYPE:-x64}"
BUILD_TYPE="${BUILD_TYPE:-Release}"
OUTPUT_DIR="${OUTPUT_DIR:-build/${ARCH_TYPE}-local-cc-${BUILD_TYPE}}"
TARGET="${TARGET:-sources_and_tests}"
COVERAGE="${COVERAGE:-0}"

mkdir -p ${OUTPUT_DIR}
