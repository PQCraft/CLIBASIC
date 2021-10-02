# tests if clibasic builds on linux and windows successfully

make build build32 clean
make cross vt build build32 clean
make cross build build32 clean

