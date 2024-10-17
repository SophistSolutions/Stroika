# Construct docker container (built from sources) with HTMLUI application

For now - just support Ubuntu 24.04, but thats good enuf for now...

~~~
docker build -t test --target deploy ../../../ -f Dockerfile
~~~