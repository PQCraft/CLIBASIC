<!----> <br>
[![image](https://github.com/PQCraft/clibasic-docs/raw/master/clibasic-banner.png)](#?)
### <p align="center"><ins>C</ins>ommand <ins>L</ins>ine <ins>I</ins>nterface <ins>B</ins>eginners <ins>A</ins>ll-purpose <ins>S</ins>ymbolic <ins>I</ins>nstruction <ins>C</ins>ode<br></p>
<p align="center">CLIBASIC is a version of BASIC designed to run in the command line or terminal</p>

<!----> <br>
---
### Links <br>
Examples: https://github.com/PQCraft/clibasic-examples <br>
Documentation: https://github.com/PQCraft/clibasic-docs <br>
Extensions: https://github.com/PQCraft/clibasic-extensions <br>
Windows Readline: https://github.com/PQCraft/clibasic-winrllib <br>

---
### Required Packages/Programs <br>
#### Linux <br>
Arch (`pacman`): `base-devel`, `readline` <br>
Debian (`apt`): `build-essential`, `libreadline-dev` <br>
Alpine (`apk`): `build-base`, `readline-dev` <br>
#### Windows <br>
NT - 10 (download): [`MinGW`](http://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/installer/mingw-w64-install.exe/download), [`Make for Windows`](http://gnuwin32.sourceforge.net/packages/make.htm) <br>
7 - 10 (`choco`): `mingw`, `make` <br>
#### MacOS <br>
Mojave - Big Sur (`brew`): `gcc`, `make`, `readline` <br>

---
### Recommended Packages/Programs <br>
#### Linux <br>
Arch (`pacman`): `git` <br>
Debian (`apt`): `git` <br>
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
Make sure you have downloaded the readline lib folder from [here](https://github.com/PQCraft/clibasic-winrllib).
1. Download the ZIP
2. Go into the .zip file you downloaded
3. Copy the `lib` folder in `clibasic-winrllib-master`
4. Paste the folder into the directory with your CLIBASIC source code

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
To run, use `make run` or `.\clibasic.exe`. <br>
To build then run, use `make` (same as `make all`). <br>
To build CLIBASIC with support for VT escape codes, add `vt` before the rest of the rules. <br>

---
### Demo <br>
[![asciicast](https://asciinema.org/a/447773.png)](https://asciinema.org/a/447773)

---
### Notes <br>
- On Arch Linux, you can install CLIBASIC by installing either the [`clibasic`](https://aur.archlinux.org/packages/clibasic/) or [`clibasic-bin`](https://aur.archlinux.org/packages/clibasic-bin/) AUR package.
- On Windows, pressing CTRL+C will not display a new prompt line due to the Windows version of readline catching and ignoring the CTRL+C.
- Most terminals cannot handle input while processing a cursor position request and INKEY$() will not reflect keys that were pressed during that time. The effect is more noticeable on terminals that are slow to respond such as Alacritty, Gnome Terminal, and Termux. A terminal that is fast to respond such as Konsole is recommended.
- If the file `.clibasic_history` is present in the user's home directory CLIBASIC will automatically save history there. Run `_AUTOCMDHIST`, `_SAVECMDHIST` (without any arguments), or create the file `.clibasic_history` in your home/user folder to enable this feature. Remove the file to disable this feature.
- CLIBASIC will look for `~/.clibasicrc`, `~/autorun.bas`, then `~/.autorun.bas` in this order in the user's home directory and run the first file found.
- The development scripts are `build.sh` which is for testing if CLIBASIC compiles correctly for Linux and Windows, `package.sh` which creates the zip files for making a CLIBASIC release, `commit.sh` which automates the build and push process, and `release-text.sh` which generates the text for making a CLIBASIC release.
- Include the `clibasic.h` file when making a clibasic extension.
