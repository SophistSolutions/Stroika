ARG BASED_ON_IMAGE=sophistsolutionsinc/stroika-buildvm-ubuntu2404-regression-tests

FROM $BASED_ON_IMAGE

# Get latest packages system, so can do installs
RUN apt-get update

# Cuz often connect to it via ssh.
# Installing it generates sshd host keys: delete them IN THE SAME RUN (a later RUN would leave them in this layer), so
# the image - which is published - carries no private host key, and no two containers share one. Each container makes
# its own at first start - see stroika-dev-entrypoint.sh.
RUN apt-get install -y openssh-server && rm -f /etc/ssh/ssh_host_*

# handy dev tools to debug, etc...
RUN apt-get install -y clang-format curl vim apt-file nodejs iproute2 lldb

EXPOSE 22

# Fixes the old '@todo have not figured out how to get sshd to start automatically'. The work cannot be
# done in a RUN step: these containers bind-mount the host's /etc/passwd, so the user identity and home
# path only exist at container start - see the comments in the script.
COPY Shared-Files/stroika-dev-entrypoint.sh /usr/local/bin/stroika-dev-entrypoint.sh
RUN chmod +x /usr/local/bin/stroika-dev-entrypoint.sh

CMD ["/usr/local/bin/stroika-dev-entrypoint.sh"]
