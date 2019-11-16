#!/bin/sh
EXEC=clang-format
ARGS="-i --style=file"
find $(dirname $0) -name "*.cpp" -o -name "*.h" -exec $EXEC $ARGS {} \;
