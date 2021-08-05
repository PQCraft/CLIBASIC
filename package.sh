# erase old versions
rm -f clibasic-linux-x64.zip
rm -f clibasic-linux-x86.zip
rm -f clibasic-windows-x64.zip
rm -f clibasic-windows-x86.zip
rm -f examples.zip

# backup built executables
mv clibasic clibasic.tmp 2> /dev/null
mv clibasic.exe clibasic.exe.tmp 2> /dev/null

# package the examples
zip -r examples.zip examples

# build
rm -f clibasic
make build 1> /dev/null
zip clibasic-linux-x64.zip clibasic
make clean 1> /dev/null

rm -f clibasic
make build32 1> /dev/null
zip clibasic-linux-x86.zip clibasic
make clean 1> /dev/null

rm -f clibasic.exe
make -f Makefile-Cross build 1> /dev/null
zip clibasic-windows-x64.zip clibasic.exe *.dll
make -f Makefile-Cross clean 1> /dev/null

rm -f clibasic.exe
make -f Makefile-Cross build32 1> /dev/null
zip clibasic-windows-x86.zip clibasic.exe *.dll
make -f Makefile-Cross clean 1> /dev/null

# clean up
mv clibasic.tmp clibasic 2> /dev/null
mv clibasic.exe.tmp clibasic.exe 2> /dev/null
