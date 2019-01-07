#!/bin/sh
VER=`dos2unix < STROIKA_VERSION | xargs echo -n`
echo "STROIKA_VERSION=$VER"
echo "Checking out V2.1-Release"
git checkout V2.1-Release
echo "Pull..."
git pull --rebase
echo "Merge in V2.1-Dev..."
git merge V2.1-Dev
echo "OK to commit/push?: CTRLC to stop or RETURN to continue"
read isOK
#git commit
git push
git tag v$VER
git push origin v$VER
git checkout V2.1-Dev
