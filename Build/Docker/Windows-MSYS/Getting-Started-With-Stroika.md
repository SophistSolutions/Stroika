# Steps to build

## Git clone

~~~
git clone https://github.com/SophistSolutions/Stroika.git
cd Stroika
~~~

## Make the build root

Where the build goes: IntermediateFiles/ and Builds/, in the tree's top (`Build/Scripts/MakeBuildRoot --help` to build
elsewhere). make stops without one.
~~~
Build/Scripts/MakeBuildRoot
~~~

## Create default configurations

This is not needed, and done automatically if you do not, but good to understand you CAN create whatever configurations you want.
~~~
make default-configurations
~~~

## Build one configuration

~~~
make CONFIGURATION=Debug all -j8
~~~

## Build all configurations

~~~
make all -j8
~~~
