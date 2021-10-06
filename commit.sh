#!/bin/bash
rev="Beta"
#-----
shopt -s extglob

echo $'\e[1m'"Grabbing version..."$'\e[0m'
make clean build || exit 1
ver="$(./clibasic --version | grep "version" | sed 's/.* version //;s/ [(].*//')"

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
git commit -m "Updated docs to $ver"
git push
cd ..

echo $'\e[1m'"Adding files:" !(clibasic|clibasic.exe|*.dll|*.zip|*.bas) $'\e[0m'
git add !(clibasic|clibasic.exe|*.dll|*.zip|*.bas) || exit 1

echo $'\e[1m'"Committing..."$'\e[0m'
msg="$ver"
extmsg="$(cat .changelog)"
echo $'\e[1m'"Message: $msg"$'\e[0m'
echo $'\e[1mExtended message:\e[0m\n-----\n'"$(cat .changelog)"$'\n-----'
git commit -m "$msg" -m "$extmsg" || exit 1

echo $'\e[1m'"Packaging..."$'\e[0m'
subrun ./package.sh || exit 1

echo $'\e[1m'"Pushing..."$'\e[0m'
git push

echo $'\e[1m'"Making release..."$'\e[0m'
gh release create "$ver" --title "$rev $ver" --notes "$(./release-text.sh)" *.zip

echo $'\e[1m'"Updating AUR packages..."$'\e[0m'
cd .aur/
subrun ./update.sh "$ver"
cd ..

