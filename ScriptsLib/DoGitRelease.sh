#!/bin/sh
VER=`dos2unix < STROIKA_VERSION | xargs echo -n`
echo "STROIKA_VERSION=$VER"
git checkout V2-Release
git pull --rebase
git merge V2-Dev
echo "OK to commit/push?"
read isOK
#git commit
git push
git tag v$VER
git push origin v$VER
git checkout Dev-v1.1

