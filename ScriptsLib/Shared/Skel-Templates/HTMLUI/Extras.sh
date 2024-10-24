
echo "Running template specific test for HTMLUI Template."

TEMPLATE_SRC_DIR=$MY_PATH_/Shared/Skel-Templates/HTMLUI
oldStroikaDir=$MY_PATH_/../
cp -r ${oldStroikaDir}Samples/HTMLUI/{Backend,Docker,Installers,OpenAPI,QuasarBasedHTMLApp} ${APP_ROOT}

sed s/\$\{APP_NAME\}/${APP_NAME}/g < $TEMPLATE_SRC_DIR/Makefile > $APP_ROOT/Makefile

