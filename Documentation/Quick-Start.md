# Quick-Start {#Quick-Start}

Building your application with Stroika

## Rough Outline

- **run docker container** to build
- checkout/clone Stroika
- **Skel** to create your app shell/outline
- **MakeBuildRoot** - once, in the tree you build (your app's, or Stroika's): where the build goes; every make but
  `make help` needs it
- **make** - to build it all
- run/test

## Docker

Stroika doesn't depend on docker. But - it depends on lots of build tools (e.g. make, c compilers etc); containers
provide a very convenient way to package those dependencies. You can install those dependencies yourself on your platform, and
just use the Dockerfile as a recipe to see what you need. Or you can just install docker, and run under it to quick start.

- [Install Docker](https://docs.docker.com/engine/install/)

## Details

Stroika provides several container images to choose from, depending on your goals, and build platform.
- [Stroika provided containers](../Build/Docker/ReadMe.md)

### Simple Example

- Windows
  ~~~
  docker run -it sophistsolutionsinc/stroika-buildvm-windows-msys-vs2k22
  ~~~
- Unix
  ~~~
  docker run -it sophistsolutionsinc/stroika-buildvm-ubuntu2404-small
  ~~~

and in the container - get stroika:

~~~
git clone https://github.com/SophistSolutions/Stroika.git
~~~

Next create your app:

~~~
cd Stroika
./Build/Scripts/Skel --help
./Build/Scripts/Skel --appRoot ../myStarterApp
~~~

Now you have a folder ready to commit to git, or build:
~~~
cd ../myStarterApp
git add .
git commit -m "my first stroika-based app"
~~~

Make its build root, once - where its build goes: IntermediateFiles/ and Builds/, in the app's directory (without one,
make stops, saying to run this):
~~~
ThirdPartyComponents/Stroika/StroikaRoot/Build/Scripts/MakeBuildRoot
~~~

now build and run the app
~~~
make CONFIGURATION=Debug all -j10
./Builds/Debug/myStarterApp/myStarterApp
~~~

(To build Stroika itself - its tests and samples - it is the same in Stroika's own directory: `Build/Scripts/MakeBuildRoot`,
then `make all -j10`.)

### Using an IDE

Before opening Stroika's tree, or your application's, in an IDE, run once (after MakeBuildRoot, above):

~~~
make apply-configurations
~~~

It makes the default configurations (if there are none) and writes each one's CMake preset (`CMakeUserPresets.json`) -
without it, Visual Studio lists no configurations. Then:

- Visual Studio: File > Open > Folder, and pick a configuration in the toolbar's configure-preset box. The first
  configure of a configuration builds its third-party components, which takes a while.
- VS Code: open `Workspaces/VSCode/<name>.code-workspace`.

A configuration you add later (`./configure ...`) appears once it is applied: `make CONFIGURATION=<name> apply-configuration`,
or a build of it.


### More complex examples

Next to try - is same as above - but with --template HTMLUI
This creates a much more complex, and useful application (webservices in C++,
HTML UI, talking to those webservices, installers etc)

But it also requires NodeJS, and quasar. So either install these yourself, or choose a container
that already has them installed

Abbreviated instructions based on the Simple example:

~~~
docker run -it sophistsolutionsinc/stroika-buildvm-ubuntu2404-regression-tests  # OR USE sophistsolutionsinc/stroika-buildvm-windows-msys-vs2k22
git clone https://github.com/SophistSolutions/Stroika.git
cd Stroika
./Build/Scripts/Skel --appRoot ../myHTMLGUIApp --template HTMLUI
cd ../myHTMLGUIApp
ThirdPartyComponents/Stroika/StroikaRoot/Build/Scripts/MakeBuildRoot
make CONFIGURATION=Debug all -j10
~~~
