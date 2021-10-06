#!/bin/bash
rev="Beta"
#-----
shopt -s extglob

function subrun() {
    ("$@") 2>&1 | while read line; do echo "> $line"; done
}

echo "Grabbing version..."
make clean build || exit 1
ver="$(./clibasic --version | grep "version" | sed 's/.* version //;s/ [(].*//')"

echo "Cleaning..."
make clean || exit 1
make cross clean || exit 1

echo "Testing..."
./build.sh || exit 1

echo "Committing and releasing [3 sec to cancel]..."
sleep 3s

echo "Updating docs submodule..."
cd docs/
subrun git add *
subrun git commit -m "Updated docs to $ver..."
subrun git push
cd ..

echo "Adding files:" !(clibasic|clibasic.exe|*.dll|*.zip|*.bas)
git add !(clibasic|clibasic.exe|*.dll|*.zip|*.bas) || exit 1

echo "Committing..."
msg="$ver"
extmsg="$(cat .changelog)"
echo "Message: $msg"
echo $'Extended message:\n-----\n'"$(cat .changelog)"$'\n-----'
git commit -m "$msg" -m "$extmsg" || exit 1

echo "Packaging..."
./package.sh || exit 1

echo "Pushing..."
git push

echo "Making release..."
gh release create "$ver" --title "$rev $ver" --notes "$(./release-text.sh)" *.zip

echo "Updating AUR packages..."
cd .aur/
subrun ./update.sh "$ver"
cd ..

