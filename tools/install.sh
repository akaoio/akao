#!/usr/bin/env sh
# Master skill installer entry point.
# Usage:  ./tools/install.sh
#         ./tools/install.sh --skills gen-tests --targets claudecode,cursor
node "$(dirname "$0")/install.js" "$@"
