# CLIBASIC <br>
<!----> <br>
### <ins>C</ins>ommand <ins>L</ins>ine <ins>I</ins>nterface <ins>B</ins>eginners <ins>A</ins>ll-purpose <ins>S</ins>ymbolic <ins>I</ins>nstruction <ins>C</ins>ode <br>
CLIBASIC is a version of BASIC designed to run in the command line or terminal
<!----> <br>
[![image](docs/clibasic_logo_rounded.png)](#?)
<!----> <br>
---
### Required Packages/Programs <br>
#### Linux <br>
Debian (`apt`): `build-essential`, `libreadline-dev` <br>
Arch (`pacman`): `base-devel`, `readline` <br>
Alpine (`apk`): `build-base`, `readline-dev` <br>
#### Windows <br>
NT - 10 (download): [`MinGW`](http://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/installer/mingw-w64-install.exe/download), [`Make for Windows`](http://gnuwin32.sourceforge.net/packages/make.htm) <br>
7 - 10 (`choco`): `mingw`, `make` <br>
#### MacOS <br>
Mojave - Big Sur (`brew`): `gcc`, `make`, `readline` <br>

---
### Recommended Packages/Programs <br>
#### Linux <br>
Debian (`apt`): `git` <br>
Arch (`pacman`): `git` <br>
Alpine (`apk`): `git` <br>
#### Windows <br>
NT - 10 (download): [`git`](https://git-scm.com/download/win) <br>
7 - 10 (`choco`): `git` <br>
#### MacOS <br>
Mojave - Big Sur (`brew`): `git` <br>

---
### Building and Running <br>
#### Linux/MacOS <br>
To build, use `make build`. <br>
To run, use `make run` or `./clibasic`. <br>
To build then run, use `make` (same as `make all`). <br>
#### Windows <br>
Make sure the bin folders of MinGW and Make are in the %PATH%. <br>
Type `gcc --version` into CMD and if you received a "Can't recognize" message, MinGW is not in your %PATH%. <br>
Type `make --version` into CMD and if you received a "Can't recognize" message, Make is not in your %PATH%. <br>
How to add MinGW and/or Make to the %PATH%: <br>
1. Navigate to where MinGW or Make is installed
2. <ins>For MinGW</ins>, open the `mingw64` folder
3. Open the `bin` folder
4. Copy the location
5. Add the location you copied to the %PATH% environment variable
    - For Windows 7 and older
        1. Open the Start Menu
        2. Right-click on Computer and click Properties
        3. Click Advanced system settings
        4. Click the Advanced tab
        5. Click Environment Variables
        6. Under System variables, find Path and click Edit
        7. Add the copied location
    - For Windows 8 and newer
        1. Open the Start Menu
        2. Search for and run 'Edit the system environment variables'
        3. Click Environment Variables
        4. Under System variables, find Path and click Edit
        5. Add the copied location

To build, use `make build`. <br>
To run, use `make run` or `./clibasic`. <br>
To build then run, use `make` (same as `make all`). <br>
To build CLIBASIC with support for VT escape codes, add `vt` before the rest of the rules. <br>

---
### Notes <br>
- On Arch Linux, you can install CLIBASIC by installing either the [`clibasic`](https://aur.archlinux.org/packages/clibasic/) or [`clibasic-bin`](https://aur.archlinux.org/packages/clibasic-bin/) AUR package.
- On unix-like OSs, if CLIBASIC is not run in a terminal it will attempt to open in XTerm unless GUI_CHECK is undefined.
- Due to Windows not having proper fork() and exec\*() functions, EXEC, EXEC(), and EXEC$() are passed through system() under Windows and one issue out of the many with this is a space parsing issue where running `EXEC "test prog"` will attempt to execute `EXEC "test", "prog"` if `test prog` cannot be found in the current directory or %PATH%.
- On Windows, pressing CTRL+C will not display a new prompt line due to the Windows version of readline catching and ignoring the CTRL+C.
- If the file `.clibasic_history` is present in the user's home directory CLIBASIC will automatically save history there. Run `_AUTOCMDHIST`, `_SAVECMDHIST` (without any arguments), or create the file `.clibasic_history` in your home/user folder to enable this feature. Remove the file to disable this feature.
- CLIBASIC will look for `~/.clibasicrc`, `autorun.bas`, then `.autorun.bas` in this order in the user's home directory and run the first file found.
- The development scripts are `build.sh` which is for testing if CLIBASIC compiles correctly for Linux and Windows, `package.sh` which creates the zip files for making a CLIBASIC release, and `release-text.sh` which generates the text for making a CLIBASIC release.
