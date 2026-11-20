# About this project

This project is created by the Stroika Skel utility.

This is folder is a standalone project, which aggregates its dependencies (like Stroika).

## Building

~~~
make all
~~~

or for a quicker build

~~~
make CONFIGURATION=Debug all -j10
~~~

which builds Stroika's libraries for the configuration, then this project - with CMake (CMakeLists.txt), on Stroika's CMake
package, by the configuration's preset (CMakeUserPresets.json, which applying the configuration writes here). So, once
Stroika's libraries are built, an IDE that reads CMake presets (VS Code's CMake Tools) builds it too:

~~~
cmake --preset Debug
cmake --build --preset Debug
~~~

## IDES

Run make apply-configurations first (or build: make all): it makes the default configurations, if there are none, and
writes each one's CMake preset (CMakeUserPresets.json) - without it Visual Studio lists no configurations. Then Visual
Studio opens this directory (File > Open > Folder); VS Code has a workspace, in Workspaces/VSCode.


