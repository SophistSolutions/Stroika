# Quick-Start building your application with Stroika

## Rough Outline

- run docker container to build
- checkout/clone Stroika
- Skel to create your app shell/outline
- make - to build it all
- run/test

## Docker

Stroika doesn't depend on docker. But - it depends on lots of build tools (make, c compilers etc) and containers
provide a very convenient way to package those dependencies. You can install those dependencies yourself on your platform, and
just use the Dockerfile as a recipe to see what you need. Or you can just install docker, and run under it to quick start.

- [Install Docker](https://docs.docker.com/engine/install/)

## Details

Stroika provides several container images to choose from, depending on your goals, and build platform.
- [Stroika provided containers](../DockerBuildContainers/ReadMe.md)

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
./ScriptsLib/Skel --help
./ScriptsLib/Skel --appRoot ../myStarterApp
~~~

Now you have a folder ready to commit to git, or build:
~~~
cd ../myStarterApp
git add .
git commit -m "my first stroika-based app"
~~~

now build and run the app
~~~
make CONFIGURATION=Debug all -j10
./Builds/Debug/myStarterApp
~~~


### More complex examples

Next to try - is same as above - but with --template HTMLUI
This creates a much more complex, and useful application (webservices in C++,
HTML UI, talking to those webservices, installers etc)