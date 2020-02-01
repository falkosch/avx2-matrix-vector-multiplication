#!/usr/bin/env bash

. ./build-with-local-cc.sh

(cd $(pwd)/${OUTPUT_DIR}/sources && find . -executable -type f -print -exec {} \;)
