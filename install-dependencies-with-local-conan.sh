#!/usr/bin/env bash

. _build-env.sh

CONAN_ARCH_TYPE="${ARCH_TYPE}"
if [ "${ARCH_TYPE}" = "x64" ]; then
    CONAN_ARCH_TYPE="x86_64"
fi

# Try having conan available in cygwin although conan is already installed for the parent Windows system
command -v python3 &>/dev/null || { echo "Please make sure 'python3' is available."; exit 1; }
python3 -m pip install conan &>/dev/null

command -v conan &>/dev/null || { echo "Please make sure 'conan' is available."; exit 1; }
conan install . \
    --install-folder "build/conan/${ARCH_TYPE}" \
    --build missing \
    --setting arch=${CONAN_ARCH_TYPE}
