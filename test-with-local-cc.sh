#!/usr/bin/env bash

. ./build-with-local-cc.sh

find ${TESTS_DIR} -executable -type f -print -exec {} -d yes \;
