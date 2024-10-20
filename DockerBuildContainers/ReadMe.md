# Docker Build Containers

Stroika does **not** depend on Docker, but Docker can provide a quickstart to building Stroika.

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
docker run -it --storage-opt "size=100GB" sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k22
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
    sophistsolutionsinc/stroika-buildvm-windows-cygwin-vs2k22
docker exec \
    FRED \
    git clone https://github.com/SophistSolutions/Stroika.git
docker exec \
    --workdir c:/Stroika \
    --env USE_TEST_BASENAME=Windows_vs2k22-In-Docker \
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

#### Windows troubleshooting

+ DNS is generally broken with docker/windows (https://stroika.atlassian.net/browse/STK-742).

An easy workaround: Docker Engine Config file:
~~~
{
  "dns": [
    "8.8.8.8"
  ]
}
~~~

+ "Reports of corruption during runs/builds"

There could be many causes, but one I've noticed is if the host PC runs out of disk space, instead of reporting as running out of disk space,
docker frequently reports this as corruption and 500 errors and no other indication of what is going wrong.


## Docker Files

- Basics
  - Linux
    - Ubuntu
      -  [Ubuntu2004-Small/DockerFile](Ubuntu2004-Small/DockerFile)
      -  [Ubuntu2204-Small/DockerFile](Ubuntu2204-Small/DockerFile)
    - Windows
      -  [Windows-VS2k22/DockerFile](Windows-VS2k22/DockerFile)   **build first**
      -  [Windows-Cygwin/DockerFile](Windows-Cygwin/DockerFile)   **based on VS2k... image**
      -  [Windows-MSYS/DockerFile](Windows-MSYS/DockerFile)       **based on VS2k... image**
- Derived
  - Linux
    - Ubuntu
      -  [Ubuntu2204-RegressionTests/DockerFile](Ubuntu2204-RegressionTests/DockerFile)
      -  [Ubuntu2404-RegressionTests/DockerFile](Ubuntu2404-RegressionTests/DockerFile)
   - Stroika-Dev
      -  [Stroika-Dev/DockerFile](Stroika-Dev/DockerFile)

## Create and setup for ssh Stroika-Dev images

(used for LGP development - very optional)

- `make stroika-dev-containers`
- `for i in Stroika-Dev Stroika-Dev-2204 Stroika-Dev-2310 Stroika-Dev-2404; do docker start $i; docker exec -it $i sudo service ssh start; done`

## Troubleshooting

### Windows

Docker for windows is exceedingly flakey, nearly to the point of unusability. These tricks may help
edge it into the usuable domain.

- DNS Resolution error on Docker Desktop windows
  "Invoke-WebRequest : The remote name could not be resolved: 'github.com'" or "Could not resolve host: github.com"
  - See https://stroika.atlassian.net/browse/STK-742
  - See https://forums.docker.com/t/dns-broken-on-docker-desktop-for-windows/130121
  - See https://github.com/docker/for-win/issues/3810

  - Workaround
    ~~~
    --network "Default Switch"
    ~~~
    in docker run command (and similarly EXTRA_DOCKER_BUILD_ARGS_= --network "Default Switch" in Docker build Makefile)
