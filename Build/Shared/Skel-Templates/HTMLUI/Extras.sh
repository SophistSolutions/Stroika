
echo "Running template specific test for HTMLUI Template."

TEMPLATE_SRC_DIR=$MY_PATH_/../Shared/Skel-Templates/HTMLUI
oldStroikaDir=$MY_PATH_/../../
cp -r ${oldStroikaDir}Samples/HTMLUI/{Backend,Docker,Installers,OpenAPI,QuasarBasedHTMLApp} ${APP_ROOT}
# built with CMake (the sample's CMakeLists.txt, the application's own): not make's per-directory build
rm -f ${APP_ROOT}/{Backend,Installers,QuasarBasedHTMLApp}/Makefile

LC_APP_NAME=`echo -n "${APP_NAME}" | tr '[:upper:]' '[:lower:]'`

sed s/\$\{APP_NAME\}/${APP_NAME}/g < $MY_PATH_/../Shared/Skel-Templates/$TEMPLATE/Makefile > $APP_ROOT/Makefile

# The sample's CMakeLists.txt, the application's: its version the application's own (APP_VERSION)
sed 's|/\.\./\.\./STROIKA_VERSION"|/APP_VERSION"|' < $oldStroikaDir/Samples/HTMLUI/CMakeLists.txt > $APP_ROOT/CMakeLists.txt

# Patch HTMLUI => {APP_NAME} in various source files: each of the sample's names to a placeholder, and only then the
# placeholders to the application's names - so a name with HTMLUI in it is not renamed again, within itself
echo "Patching for HTMLUI Template::Sources..."
for i in ${APP_ROOT}/CMakeLists.txt ${APP_ROOT}/{Backend,Installers,QuasarBasedHTMLApp}/*.cmake ${APP_ROOT}/{Backend,QuasarBasedHTMLApp}/CMakeLists.txt \
         ${APP_ROOT}/Installers/*.wix ${APP_ROOT}/Installers/*.wxl ${APP_ROOT}/Installers/stroika-sample-htmlui.* \
         ${APP_ROOT}/Docker/Dockerfile \
         ${APP_ROOT}/QuasarBasedHTMLApp/src/{pages,layouts}/*.vue \
         ${APP_ROOT}/Backend/Sources/*.{h,inl,cpp};
do
    sed -e "s/Samples-HTMLUI/SKEL_APP_NAME_/g" \
        -e "s/Samples\/HTMLUI/SKEL_APP_NAME_/g" \
        -e "s/stroika-sample-htmlui/SKEL_LC_APP_NAME_/g" \
        -e "s/Stroika-Sample-HTMLUI/SKEL_APP_NAME_/g" \
        -e "s/Stroika Sample HTMLUI/SKEL_APP_NAME_/g" \
        -e "s/Sample HTMLUI/SKEL_APP_NAME_/g" \
        -e "s/HTMLUI/SKEL_APP_NAME_/g" \
        -e "s/htmlui/SKEL_LC_APP_NAME_/g" \
        -e "s/SKEL_APP_NAME_/${APP_NAME}/g" \
        -e "s/SKEL_LC_APP_NAME_/${LC_APP_NAME}/g" < $i > $i.tmp
    mv  $i.tmp $i
done


cat  ${APP_ROOT}/Docker/Dockerfile | \
  sed 's/make CONFIGURATION=${CONFIGURATION} libraries.*/make CONFIGURATION=${CONFIGURATION} all/' | \
  sed 's/make CONFIGURATION=${CONFIGURATION} --directory.*//' > ${APP_ROOT}/Docker/Dockerfile.new
mv ${APP_ROOT}/Docker/Dockerfile.new ${APP_ROOT}/Docker/Dockerfile


cat  ${APP_ROOT}/Docker/ReadMe.md | \
  sed 's/..\/..\/..\//..\//' > ${APP_ROOT}/Docker/ReadMe.md.new
mv ${APP_ROOT}/Docker/ReadMe.md.new ${APP_ROOT}/Docker/ReadMe.md


(pushd ${APP_ROOT}/Installers; \
     mv stroika-sample-htmlui.control.static ${LC_APP_NAME}.control.static; \
     mv stroika-sample-htmlui.rpm.spec ${LC_APP_NAME}.rpm.spec; \
     mv stroika-sample-htmlui.service ${LC_APP_NAME}.service; \
     ) > /dev/null
