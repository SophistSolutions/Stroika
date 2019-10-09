# Docker Build Containers

There is no need to use this, but it can provide a quickstart to building Stroika

Just create one or more of these docker images, and run them as in:

- `make docker-images`
- `docker run -it sophistsolutionsinc/stroika-buildvm-ubuntu1804-small`
  - then inside the docker container find a /Getting-Started-With-Stroika.md, and follow its instructions
    to build (basically clone Stroika and type make)
  - n.b. you may need to use

    ~~~bash
    --security-opt seccomp=unconfined
    ~~~

    to run debugger (ptrace/address randomization disable, or for debug builds that use sanitizers)
- to create a fancier container with more options, consider using - ScriptsLib/RunInDockerEnvironment - which just sets a bunch of useful container options.
  for example:

  ~~~bash
  cd Sandbox/Stroika-Dev && SANDBOX_FROM=/Sandbox CONTAINER_NAME=Stroika-Dev CONTAINER_IMAGE=sophistsolutionsinc/stroika-dev INCLUDE_EXTRA_PERSONAL_MOUNT_FILES=1 ScriptsLib/RunInDockerEnvironment
  ~~~

And, first thing when you run docker image, cat /Getting-Started-With-Stroika.md
  
  ~~~bash
  cat /Getting-Started-With-Stroika.md
  ~~~
