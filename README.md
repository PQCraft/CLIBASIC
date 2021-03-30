# CLIBASIC <br>
<!----> <br>
### <ins>C</ins>ommand <ins>L</ins>ine <ins>I</ins>nterface <ins>B</ins>eginners <ins>A</ins>ll-purpose <ins>S</ins>ymbolic <ins>I</ins>nstruction <ins>C</ins>ode <br>
CLIBASIC is a version of BASIC designed to run in the command line or terminal
<!----> <br>
### Building and Running <br>
#### Linux <br>
To build, use `make build`. <br>
To run, use `make run` or `./clibasic`. <br>
To build then run, use `make` (this is the same as `make all`). <br>
#### Windows <br>
To build, use `make -f Makefile-Windows build`. <br>
To run, use `make -f Makefile-Windows run` or `.\clibasic.exe`. <br>
To build then run, use `make -f Makefile-Windows` (this is the same as `make -f Makefile-Windows all`). <br>
<br>
### Required Packages/Programs <br>
#### Linux <br>
Debian/Ubuntu: `libreadline-dev` <br>
Arch: `readline` <br>
Apline: `readline-dev` <br>
#### Windows <br>
Wimdows NT-10: [`MinGW`](http://mingw-w64.org/), [`Make for Windows`](http://gnuwin32.sourceforge.net/packages/make.htm) <br>
<br>
### Notes <br>
- On Windows please use Windows Terminal or execute the folowing command in CMD as Admin to enable ANSI escape sequnces on CMD: ```reg add HKEY_CURRENT_USER\Console /v VirtualTerminalLevel /t REG_DWORD /d 0x00000001 /f``` <br>
- Pressing <kbd>Ctrl</kbd>+<kbd>C</kbd> on Windows will for some reason, not work while scanf() is reading from the command-line (during `INPUT$()` and the `CLIBASIC> ` prompt). Pressing <kbd>Ctrl</kbd>+<kbd>C</kbd> and then <kbd>Enter</kbd> will fix the issue.
<br>
