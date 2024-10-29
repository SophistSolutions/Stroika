
echo "Running template specific test for HTMLUI Template."

TEMPLATE_SRC_DIR=$MY_PATH_/Shared/Skel-Templates/HTMLUI
oldStroikaDir=$MY_PATH_/../
cp -r ${oldStroikaDir}Samples/HTMLUI/{Backend,Docker,Installers,OpenAPI,QuasarBasedHTMLApp} ${APP_ROOT}

LC_APP_NAME=`echo -n "${APP_NAME}" | tr '[:upper:]' '[:lower:]'`

sed s/\$\{APP_NAME\}/${APP_NAME}/g < $MY_PATH_/Shared/Skel-Templates/$TEMPLATE/Makefile > $APP_ROOT/Makefile

# Patch TOP_ROOT and StroikaRoot, and HTMLUI->AppName in Makefiles
echo "Patching for HTMLUI Template::Makefiles..."
for i in Backend Installers QuasarBasedHTMLApp;
do
    cat $oldStroikaDir/Samples/HTMLUI/$i/Makefile  | \
    sed 's/export TOP_ROOT:=.*/export TOP_ROOT:=$(abspath ..)\//' | \
    sed 's/StroikaRoot:=.*/StroikaRoot:=$(TOP_ROOT)ThirdPartyComponents\/Stroika\/StroikaRoot\//' | \
    sed 's/SrcDir.*:=.*/SrcDir:=$(TOP_ROOT)Backend\//' | \
    sed 's/VERSION_FILE_IN=$(StroikaRoot)STROIKA_VERSION/VERSION_FILE_IN=$(TOP_ROOT)VERSION/' | \
    sed "s/Samples-HTMLUI/${APP_NAME}/g" | \
    sed "s/Stroika-Sample-HTMLUI/${APP_NAME}/g" | \
    sed "s/Samples\/HTMLUI/${APP_NAME}/g" | \
    sed "s/stroika-sample-htmlui/${LC_APP_NAME}/g" | \
    sed "s/HTMLUI/${APP_NAME}/g" > $APP_ROOT/$i/Makefile
done

# Patch HTMLUI => {APP_NAME} in various source files
echo "Patching for HTMLUI Template::Sources..."
for i in ${APP_ROOT}/Installers/*.wix ${APP_ROOT}/Installers/*.wxl ${APP_ROOT}/Installers/stroika-sample-htmlui.* \
         ${APP_ROOT}/Docker/Dockerfile \
         ${APP_ROOT}/QuasarBasedHTMLApp/src/{pages,layouts}/*.vue \
         ${APP_ROOT}/Backend/Sources/*.{h,inl,cpp};
do
    #echo PATCHING HTMLUI NAME in: $i
    cat $i |
    sed "s/Samples-HTMLUI/${APP_NAME}/g" | \
    sed "s/Samples\/HTMLUI/${APP_NAME}/g" | \
    sed "s/stroika-sample-htmlui/${LC_APP_NAME}/g" | \
    sed "s/Stroika-Sample-HTMLUI/${APP_NAME}/g" | \
    sed "s/Stroika Sample HTMLUI/${APP_NAME}/g" | \
    sed "s/Sample HTMLUI/${APP_NAME}/g" | \
    sed "s/Samples-HTMLUI/${APP_NAME}/g" | \
    sed "s/HTMLUI/${APP_NAME}/g" | \
    sed "s/htmlui/${LC_APP_NAME}/g" > $i.tmp
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
