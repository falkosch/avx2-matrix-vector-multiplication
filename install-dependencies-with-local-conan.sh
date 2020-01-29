#!/usr/bin/env bash

. _build-env.sh

CONAN_ARCH_TYPE="${ARCH_TYPE}"
if [ "${ARCH_TYPE}" = "x64" ]; then
    CONAN_ARCH_TYPE="x86_64"
fi

conan install . \
    --install-folder "build/conan/${ARCH_TYPE}" \
    --build missing \
    --setting arch=${CONAN_ARCH_TYPE}
