#!/bin/bash
rev="Release"
#-----

shopt -s extglob

echo $'\e[1m'"Grabbing version..."$'\e[0m'
make clean build || exit 1
ver="$(./clibasic --version | grep "version" | sed 's/.* version //;s/ [(].*//')"
echo "[$ver]"

echo $'\e[1m'"Cleaning..."$'\e[0m'
make clean || exit 1
make cross clean || exit 1

echo $'\e[1m'"Testing..."$'\e[0m'
./build.sh || exit 1

echo $'\e[1m'"Committing and releasing [3 sec to cancel]..."$'\e[0m'
sleep 3s

echo $'\e[1m'"Updating docs submodule..."$'\e[0m'
cd docs/
git add *
git commit -S -m "Updated docs to $ver"
git push
cd ..

echo $'\e[1m'"Updating examples submodule..."$'\e[0m'
cd examples/
git add *
git commit -S -m "Updated examples to $ver"
git push
cd ..

echo $'\e[1m'"Adding files..."$'\e[0m'
mv lib .lib
echo LICENSE Makefile *.c *.h *.sh *.md .gitmodules .gitattributes */
git add LICENSE Makefile *.c *.h *.sh *.md .gitmodules .gitattributes */
e=$?
mv .lib lib
[ $e -ne 0 ] && exit 1

echo $'\e[1m'"Committing..."$'\e[0m'
msg="$ver"
extmsg="$(cat .changelog)"
echo $'\e[1m'"Message:"$'\e[0m'" $msg"
echo $'\e[1mExtended message:\e[0m\n-----\n'"$extmsg"$'\n-----'
git commit -S -m "$msg" -m "$extmsg" || exit 1

echo $'\e[1m'"Packaging..."$'\e[0m'
./package.sh || exit 1

echo $'\e[1m'"Pushing..."$'\e[0m'
git push

echo $'\e[1m'"Making tag..."$'\e[0m'
git tag -s "$ver" -m "$extmsg"

echo $'\e[1m'"Making release..."$'\e[0m'
gh release create "$ver" --title "$rev $ver" --notes "$(./release-text.sh)" *.zip

echo $'\e[1m'"Updating AUR packages..."$'\e[0m'
cd .aur/
./update.sh "$ver"
cd ..

