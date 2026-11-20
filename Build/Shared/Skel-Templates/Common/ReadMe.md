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

Visual Studio opens this directory (File > Open > Folder) and builds from each configuration's CMake preset
(CMakeUserPresets.json); VS Code has a workspace, in Workspaces/VSCode. A configuration's preset is written as it is
applied - by its first build (see above).


