# Tests if clibasic builds on linux and windows successfully

make build build32 clean || exit 1
make cross vt build build32 clean || exit 1
make cross build build32 clean || exit 1

