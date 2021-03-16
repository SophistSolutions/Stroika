# Docker Build Containers

There is no need to use this, but it can provide a quickstart to building Stroika

Just create one or more of these docker images, and run them as in:

## Create Docker Images

(or you can pull them from docker hub - skipping this step)

- `make docker-images`

## Run container interactively

### Unix

- `docker run -it sophistsolutionsinc/stroika-buildvm-ubuntu2004-small`

  First thing when you run docker image, cat /Getting-Started-With-Stroika.md

  ```bash
  cat /Getting-Started-With-Stroika.md
  ```

#### Unix - Fancy

n.b. you may need to use

```bash
--security-opt seccomp=unconfined
```

to run debugger (ptrace/address randomization disable, or for debug builds that use sanitizers)

- to create a fancier container with more options, consider using - ScriptsLib/RunInDockerEnvironment - which just sets a bunch of useful container options.
  for example:

  ```bash
  cd Sandbox/Stroika-Dev && SANDBOX_FROM=/Sandbox CONTAINER_NAME=Stroika-Dev CONTAINER_IMAGE=sophistsolutionsinc/stroika-dev INCLUDE_EXTRA_PERSONAL_MOUNT_FILES=1 EXTRA_DOCKER_ARGS="--publish 10022:22" ECHO_DOCKER_COMMANDS=1 ScriptsLib/RunInDockerEnvironment
  ```

  ```bash
  cd Sandbox/Stroika-Dev && SANDBOX_FROM=/Sandbox CONTAINER_NAME=Stroika-Dev-2004 CONTAINER_IMAGE=sophistsolutionsinc/stroika-dev-2004 INCLUDE_EXTRA_PERSONAL_MOUNT_FILES=1 EXTRA_DOCKER_ARGS="--publish 10122:22" ECHO_DOCKER_COMMANDS=1 ScriptsLib/RunInDockerEnvironment
  ```

### Windows

```bash
docker run -it --storage-opt "size=100GB" sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k19
```

(defaults for RAM / disk space I found inadequate, but this maybe fixed by MSFT at some point)

First thing when you run docker image, cat /Getting-Started-With-Stroika.md

```bash
cat /Getting-Started-With-Stroika.md
```

#### Windows - Fancy

```bash
docker run --detach --interactive --tty\
    --storage-opt "size=100GB" \
    -m 8G \
    --name FRED \
    sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k19
docker exec \
    FRED \
    git clone https://github.com/SophistSolutions/Stroika.git
docker exec \
    --workdir c:/Stroika \
    --env USE_TEST_BASENAME=Windows_VS2k19-In-Docker \
    FRED \
    sh -c "ScriptsLib/RegressionTests"
# root around and fiddle inside the container (even while above running)
docker exec  --interactive --tty \
    FRED \
    cmd
```

And, first thing when you run docker image, cat /Getting-Started-With-Stroika.md

```bash
cat /Getting-Started-With-Stroika.md
```
