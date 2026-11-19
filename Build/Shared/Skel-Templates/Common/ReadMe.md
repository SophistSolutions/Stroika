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

Stroika's Skel utility directly supports Visual Studio.Net 2019, and 2022, as well as Visual Studio Code.

To get the full benefit of these project files, you may need do to an initial build of Stroika
for each configuration you wish to use from a project file (see above).


