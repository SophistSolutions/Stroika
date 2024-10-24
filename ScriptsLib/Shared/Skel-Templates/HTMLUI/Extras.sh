
echo "Running template specific test for HTMLUI Template."

TEMPLATE_SRC_DIR=$MY_PATH_/Shared/Skel-Templates/HTMLUI
oldStroikaDir=$MY_PATH_/../
cp -r ${oldStroikaDir}Samples/HTMLUI/{Backend,Docker,Installers,OpenAPI,QuasarBasedHTMLApp} ${APP_ROOT}

sed s/\$\{APP_NAME\}/${APP_NAME}/g < $TEMPLATE_SRC_DIR/Makefile > $APP_ROOT/Makefile

sed s/\$\{APP_NAME\}/${APP_NAME}/g < $TEMPLATE_SRC_DIR/Makefile > $APP_ROOT/Makefile

# Patch TOP_ROOT and StroikaRoot, and HTMLUI->AppName in Makefiles
echo "Patching for HTMLUI Template::Makefiles..."
for i in Backend Installers QuasarBasedHTMLApp;
do
    cat $oldStroikaDir/Samples/HTMLUI/$i/Makefile  |
    sed 's/export TOP_ROOT:=.*/export TOP_ROOT:=$(abspath ..)\//' |
    sed 's/StroikaRoot:=.*/StroikaRoot:=$(TOP_ROOT)ThirdPartyComponents\/Stroika\/StroikaRoot\//' |
    sed 's/SrcDir.*:=.*/SrcDir:=$(TOP_ROOT)Backend\//' |
    sed 's/VERSION_FILE_IN=$(StroikaRoot)STROIKA_VERSION/VERSION_FILE_IN=$(TOP_ROOT)VERSION/' |
    sed "s/Samples-HTMLUI/${APP_NAME}/g" |
    sed "s/Samples\/HTMLUI/${APP_NAME}/g" |
    sed "s/HTMLUI/${APP_NAME}/g" > $APP_ROOT/$i/Makefile
done


# Patch HTMLUI => {APP_NAME} in various source files
echo "Patching for HTMLUI Template::Sources..."
for i in ${APP_ROOT}/Installers/*.wix ${APP_ROOT}/QuasarBasedHTMLApp/src/pages/*.vue ${APP_ROOT}/Backend/Sources/*.{h,inl,cpp};
do
    echo PATCHING HTMLUI NAME in: $i
    cat $i |
    sed "s/Samples-HTMLUI/${APP_NAME}/g" |
    sed "s/Samples\/HTMLUI/${APP_NAME}/g" |
    sed "s/Sample HTMLUI/${APP_NAME}/g" |
    sed "s/HTMLUI/${APP_NAME}/g" > $i.tmp
    mv  $i.tmp $i
done

