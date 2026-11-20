
# shell script after base Skel run
echo "Running template specific test for Basic Template."
TEMPLATE_SRC_DIR=$MY_PATH_/../Shared/Skel-Templates/Basic

mkdir -p  $APP_ROOT/$APP_NAME/Sources

sed s/\$\{APP_NAME\}/${APP_NAME}/g < $MY_PATH_/../Shared/Skel-Templates/$TEMPLATE/Makefile > $APP_ROOT/Makefile
sed s/\$\{APP_NAME\}/${APP_NAME}/g < $TEMPLATE_SRC_DIR/CMakeLists.txt > $APP_ROOT/CMakeLists.txt

sed s/\$\{APP_NAME\}/${APP_NAME}/g < $TEMPLATE_SRC_DIR/MyApp/CMakeLists.txt > $APP_ROOT/$APP_NAME/CMakeLists.txt
sed s/\$\{APP_NAME\}/${APP_NAME}/g < $TEMPLATE_SRC_DIR/MyApp/Sources/Main.cpp > $APP_ROOT/$APP_NAME/Sources/Main.cpp
