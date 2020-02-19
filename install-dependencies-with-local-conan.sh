#!/usr/bin/env bash

. _build-env.sh

CONAN_ARCH_TYPE="${CONAN_ARCH_TYPE:-${ARCH_TYPE}}"
if [ "${CONAN_ARCH_TYPE}" = "x64" ]; then
    CONAN_ARCH_TYPE="x86_64"
fi

CONAN_DEP_INSTALL_DIR="${CONAN_DEP_INSTALL_DIR:-build/conan/${ARCH_TYPE}}"

# Try having conan available in cygwin although conan is already installed for the parent Windows system
PYTHON=${PYTHON:-$(command -v python || command -v python3)}
[ -f ${PYTHON} ] || { echo "Please make sure 'python3' is available."; exit 1; }
${PYTHON} -m pip install conan &>/dev/null

CONAN=${CONAN:-$(command -v conan)}
[ -f ${CONAN} ] || { echo "Please make sure 'conan' is available."; exit 1; }

mkdir -p ${CONAN_DEP_INSTALL_DIR}
rm -rf ${CONAN_DEP_INSTALL_DIR}/*

CONAN_ADD_ARGS="${CONAN_ADD_ARGS:- --setting arch=${CONAN_ARCH_TYPE}}"

conan install . \
    --install-folder ${CONAN_DEP_INSTALL_DIR} \
    --build missing \
    ${CONAN_ADD_ARGS}
