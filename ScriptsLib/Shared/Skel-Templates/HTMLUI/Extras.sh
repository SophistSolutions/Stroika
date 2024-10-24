
echo "Running template specific test for HTMLUI Template."

# shell script after base Skel run
# TEMPLATE_SRC_DIR=$MY_PATH_/Shared/Skel-Templates/Basic/MyApp
# sed s/\$\{APP_NAME\}/${APP_NAME}/g < $TEMPLATE_SRC_DIR/Makefile > $APP_ROOT/$APP_NAME/Makefile
# sed s/\$\{APP_NAME\}/${APP_NAME}/g < $TEMPLATE_SRC_DIR/Sources/Main.cpp > $APP_ROOT/$APP_NAME/Sources/Main.cpp
# sed s/\$\{APP_NAME\}/${APP_NAME}/g < $TEMPLATE_SRC_DIR/Sources/Makefile > $APP_ROOT/$APP_NAME/Sources/Makefile

# sed s/\$\{APP_NAME\}/${APP_NAME}/g < $TEMPLATE_SRC_DIR/Workspaces/VisualStudio.Net-2022/MyApp.sln > $APP_ROOT/Workspaces/VisualStudio.Net-2022/$APP_NAME.sln

# sed s/\$\{APP_NAME\}/${APP_NAME}/g < $TEMPLATE_SRC_DIR/Projects/VisualStudio.Net-2022/MyApp.vcxproj > $APP_ROOT/$APP_NAME/Projects/VisualStudio.Net-2022/$APP_NAME.vcxproj
# sed s/\$\{APP_NAME\}/${APP_NAME}/g < $TEMPLATE_SRC_DIR/Projects/VisualStudio.Net-2022/MyApp.vcxproj.filters > $APP_ROOT/$APP_NAME/Projects/VisualStudio.Net-2022/$APP_NAME.vcxproj.filters

TEMPLATE_SRC_DIR=$MY_PATH_/Shared/Skel-Templates/HTMLUI
oldStroikaDir=$MY_PATH_/../
cp -r ${oldStroikaDir}Samples/HTMLUI/{Backend,Docker,Installers,OpenAPI,QuasarBasedHTMLApp} ${APP_ROOT}

sed s/\$\{APP_NAME\}/${APP_NAME}/g < $TEMPLATE_SRC_DIR/Makefile > $APP_ROOT/Makefile

