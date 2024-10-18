# Construct docker container (built from sources) with HTMLUI application

For now - just support Ubuntu 24.04, but thats good enuf for now...

~~~
docker build -t test --target deploy ../../../ -f Dockerfile
~~~
OR
~~~
docker build -t test --target deploy ../../../ -f Dockerfile CONFIGURATION=Debug JFLAG=-j10
~~~


and run the produced container with:
~~~
docker run --rm -it -p 8080:80 test
~~~

then BROWSE the your target machine - port 8080.
