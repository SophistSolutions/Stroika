#!/bin/sh
VER=`dos2unix < STROIKA_VERSION | xargs echo -n`
echo "STROIKA_VERSION=$VER"
echo "Checking out V2-Release"
git checkout V2-Release
echo "Pull/Rebasing..."
git pull --rebase
echo "Merge in V2-Dev..."
git merge V2-Dev
echo "OK to commit/push?: CTRLC to stop or RETURN to continue"
read isOK
#git commit
git push
git tag v$VER
git push origin v$VER
git checkout V2-Dev
