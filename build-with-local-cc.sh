#!/usr/bin/env bash

. _build-env.sh

cmake -B ${OUTPUT_DIR} \
    -DCMAKE_VS_PLATFORM_NAME:STRING="${ARCH_TYPE}" \
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"

cmake --build ${OUTPUT_DIR} \
    --clean-first \
    --target sources tests \
    --config ${BUILD_TYPE}
