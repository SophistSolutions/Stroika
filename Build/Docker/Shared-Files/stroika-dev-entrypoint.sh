#!/bin/bash
#
# Entrypoint for the Stroika-Dev containers: make the home directory usable, then run sshd.
#
# Why this must run at CONTAINER START and cannot be a Dockerfile RUN: these containers bind-mount the
# HOST's /etc/passwd (and /etc/group, /etc/shadow), so the user identity - name, uid, home path - is
# not known when the image is built. Only at run time does `id`/`~` resolve to the real developer.
#
# Why it exists at all: the previous start command was
#       sudo mkdir -p ~; sudo chmod a+w ~; sudo mkdir /run/sshd; sudo /usr/sbin/sshd -D
# and `sudo mkdir` created the home owned by ROOT while `chmod a+w` made it world-writable. sshd runs
# with StrictModes yes (the default), which REFUSES to read ~/.ssh/authorized_keys when the home
# directory is writable by anyone but its owner - so public-key auth could never work in these
# containers, for any key, and every login fell through to a password prompt. Owning the directory
# gives the developer the write access `a+w` was reaching for, without defeating StrictModes.
#
# Note only ~/.ssh is bind-mounted from the host, not all of ~, so the home directory itself comes
# from the image and starts out root-owned - hence the chown here rather than relying on the mount.

set -euo pipefail

HOME_DIR="$(getent passwd "$(id -u)" | cut -d: -f6)"
: "${HOME_DIR:=$HOME}"

sudo mkdir -p "$HOME_DIR"
sudo chown "$(id -u):$(id -g)" "$HOME_DIR"
sudo chmod 755 "$HOME_DIR" # NOT a+w - see above

# -p so a restart does not log 'mkdir: cannot create directory /run/sshd: File exists' on every boot
sudo mkdir -p /run/sshd

exec sudo /usr/sbin/sshd -D "$@"
