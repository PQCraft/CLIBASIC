# CLIBASIC <br>
<!----> <br>
### <ins>C</ins>ommand <ins>L</ins>ine <ins>I</ins>nterface <ins>B</ins>eginners <ins>A</ins>ll-purpose <ins>S</ins>ymbolic <ins>I</ins>nstruction <ins>C</ins>ode <br>
CLIBASIC is a version of BASIC designed to run in the command line or terminal
<!----> <br>
### Building and Running <br>
#### Linux/MacOS <br>
To build, use `make build`. <br>
To run, use `make run` or `./clibasic`. <br>
To build then run, use `make` (same as `make all`). <br>
#### Windows <br>
To build, use `make -f Makefile-Windows build`. <br>
To run, use `make -f Makefile-Windows run` or `.\clibasic.exe`. <br>
To build then run, use `make -f Makefile-Windows` (same as `make -f Makefile-Windows all`). <br>
<br>
### Required Packages/Programs <br>
#### Linux <br>
Debian (`apt`): `build-essential`, `libreadline-dev` <br>
Arch (`pacman`): `base-devel`, `readline` <br>
Apline (`apk`): `build-base`, `readline-dev` <br>
#### Windows <br>
Wimdows NT-10 (download): [`MinGW`](http://mingw-w64.org/doku.php/download/mingw-builds), [`Make for Windows`](http://gnuwin32.sourceforge.net/packages/make.htm) <br>
Windows 7-10 (`choco`): `mingw`, `make` <br>
#### MacOS <br>
MacOS Mojave-Big Sur (`brew`): `gcc`, `make`, `readline` <br>
<br>
### Notes <br>
- On Windows please use Windows Terminal or execute the folowing command in CMD as Admin to enable ANSI escape sequnces on CMD: ```reg add HKEY_CURRENT_USER\Console /v VirtualTerminalLevel /t REG_DWORD /d 0x00000001 /f```. <br>
- Windows `INPUT$()` is faulty due to a poor implementaion of readline. <kbd>Ctrl</kbd>+<kbd>C</kbd> must be followed by <kbd>Enter</kbd>. <br>
- On Linux if CLIBASIC is not run in a terminal or STDERR is redirected it will open in XTerm.
