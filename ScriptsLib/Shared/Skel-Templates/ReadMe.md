# About this project

This project is created by the Stroika Skel utility

## Building

~~~
make all
~~~

or for a quicker build

~~~
make CONFIGURATION=Debug all -j10
~~~

## IDES

Stroika's Skel utility directly supports Visual Studio.Net 2019, and 2022, as well as Visual Studio Code.

To get the full benefit of these project files, you MAY need to run

~~~
make project-files
~~~

but generally if you do any other sort of make call, it will take care of building the project files for you


