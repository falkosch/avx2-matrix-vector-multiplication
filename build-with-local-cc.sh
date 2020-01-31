#!/usr/bin/env bash

. _build-env.sh

cmake -B ${OUTPUT_DIR} \
    -DCMAKE_VS_PLATFORM_NAME="${ARCH_TYPE}" \
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
    -DCOVERAGE="${COVERAGE}"

cmake --build ${OUTPUT_DIR} \
    --target ${TARGET} \
    --config ${BUILD_TYPE}
