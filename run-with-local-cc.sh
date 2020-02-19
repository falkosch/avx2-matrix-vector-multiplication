#!/usr/bin/env bash

. ./build-with-local-cc.sh

find ${SOURCES_DIR} -executable -type f -print -exec {} \;
