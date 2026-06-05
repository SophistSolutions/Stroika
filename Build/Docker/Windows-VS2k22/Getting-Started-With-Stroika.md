# Steps to build

## Git clone

~~~
git clone https://github.com/SophistSolutions/Stroika.git
cd Stroika
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

## Make all configurations

~~~
make all -j8
~~~

for now, not enuf disk space on windows docker containers, and broken resetting size
