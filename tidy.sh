#!/bin/sh
EXEC=clang-tidy
ARGS="--fix --format-style=file -p cmake/compile_commands.json"
find $(dirname $0) -name "*.cpp" -o -name "*.h" -exec $EXEC $ARGS {} \;
