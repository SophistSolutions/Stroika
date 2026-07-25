
# shell script after base Skel run
echo "Running template specific test for Basic Template."
TEMPLATE_SRC_DIR=$MY_PATH_/../Shared/Skel-Templates/Basic

mkdir -p  $APP_ROOT/$APP_NAME/Sources

sed s/\$\{APP_NAME\}/${APP_NAME}/g < $MY_PATH_/../Shared/Skel-Templates/$TEMPLATE/Makefile > $APP_ROOT/Makefile

sed s/\$\{APP_NAME\}/${APP_NAME}/g < $TEMPLATE_SRC_DIR/MyApp/Makefile > $APP_ROOT/$APP_NAME/Makefile
sed s/\$\{APP_NAME\}/${APP_NAME}/g < $TEMPLATE_SRC_DIR/MyApp/Sources/Main.cpp > $APP_ROOT/$APP_NAME/Sources/Main.cpp
sed s/\$\{APP_NAME\}/${APP_NAME}/g < $TEMPLATE_SRC_DIR/MyApp/Sources/Makefile > $APP_ROOT/$APP_NAME/Sources/Makefile

#
# Visual Studio projects/solution - one set per VS version. The templates under
# {Workspaces,MyApp/Projects}/VisualStudio.Net/ are version independent; the differences between one VS
# version and the next are just the toolset and the version stamped in the .sln, so they are substituted
# in here rather than kept as a separate copy of each file per version.
#
#	Fields: <year> <PlatformToolset> <.sln VisualStudioVersion>
#	Keep in sync with the versions Stroika itself ships projects for - the generated .sln refers to
#	StroikaRoot/Library/Projects/VisualStudio.Net-<year>, so a year Stroika has no projects for
#	would produce a solution that cannot load.
#
substVSVars_ ()
{
  sed -e "s/\${APP_NAME}/${APP_NAME}/g" \
      -e "s/\${VS_YEAR}/${VS_YEAR}/g" \
      -e "s/\${VS_PLATFORM_TOOLSET}/${VS_PLATFORM_TOOLSET}/g" \
      -e "s/\${VS_SLN_VERSION}/${VS_SLN_VERSION}/g" \
      -e "s/\${VS_SLN_MAJOR}/${VS_SLN_MAJOR}/g"
}
while read -r VS_YEAR VS_PLATFORM_TOOLSET VS_SLN_VERSION ; do
  if [ -z "$VS_YEAR" ] ; then continue ; fi
  VS_SLN_MAJOR=`echo $VS_SLN_VERSION | cut -d. -f1`
  echo "Making Visual Studio $VS_YEAR project files (PlatformToolset $VS_PLATFORM_TOOLSET)..."

  VS_WORKSPACE_DIR_=$APP_ROOT/Workspaces/VisualStudio.Net-$VS_YEAR
  VS_PROJECT_DIR_=$APP_ROOT/$APP_NAME/Projects/VisualStudio.Net-$VS_YEAR
  mkdir -p $VS_WORKSPACE_DIR_ $VS_PROJECT_DIR_

  substVSVars_ < $TEMPLATE_SRC_DIR/Workspaces/VisualStudio.Net/MyApp.sln > $VS_WORKSPACE_DIR_/$APP_NAME.sln
  substVSVars_ < $TEMPLATE_SRC_DIR/MyApp/Projects/VisualStudio.Net/MyApp.vcxproj > $VS_PROJECT_DIR_/$APP_NAME.vcxproj
  substVSVars_ < $TEMPLATE_SRC_DIR/MyApp/Projects/VisualStudio.Net/MyApp.vcxproj.filters > $VS_PROJECT_DIR_/$APP_NAME.vcxproj.filters
  cp $TEMPLATE_SRC_DIR/MyApp/Projects/VisualStudio.Net/.gitignore $VS_PROJECT_DIR_/
done <<EOF
2022 v143 17.4.33205.214
2026 v145 18.6.11822.322
EOF

cp $TEMPLATE_SRC_DIR/Workspaces/.gitignore $APP_ROOT/Workspaces/
