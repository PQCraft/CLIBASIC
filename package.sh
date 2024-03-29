# clibasic release packager

# mkrel function to reduce sloc
mkrel() {
    echo "Making $1..."
    rm -f "$1"
    make $3 1> /dev/null || exit 1
    zip "$1" $2 1> /dev/null || exit 1
    make $4 1> /dev/null
}

pack() {
    echo "Packaging $1..."
    rm -f "$1".zip
    zip -r "$1".zip "$1"/* 1> /dev/null || exit 1
}

# backup built executables
mv clibasic clibasic.tmp 2> /dev/null
mv clibasic.exe clibasic.exe.tmp 2> /dev/null

# package extras
pack examples
pack docs
pack lib

# build release
mkrel "clibasic-linux-x64.zip" "clibasic" "clean build" "clean"
mkrel "clibasic-linux-x86.zip" "clibasic" "clean build32" "clean"
mkrel "clibasic-windows-vt-x64.zip" "clibasic.exe *.dll" "cross clean vt build" "cross clean"
mkrel "clibasic-windows-vt-x86.zip" "clibasic.exe *.dll" "cross clean vt build32" "cross clean"
mkrel "clibasic-windows-x64.zip" "clibasic.exe *.dll" "cross clean build" "cross clean"
mkrel "clibasic-windows-x86.zip" "clibasic.exe *.dll" "cross clean build32" "cross clean"

# clean up
mv clibasic.tmp clibasic 2> /dev/null
mv clibasic.exe.tmp clibasic.exe 2> /dev/null

exit 0
