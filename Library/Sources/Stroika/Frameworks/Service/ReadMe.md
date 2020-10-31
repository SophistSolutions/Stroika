# Stroika::[Frameworks](../ReadMe.md)::Service

This Folder contains the [Frameworks](../ReadMe.md)::Service Framework source code.

## Overview

The Stroika service framework is a simple framework to allow deploying applications as
'unix' or 'windows' style services.

What this means differs quite a bit from platform to platform, but basically it means
easily hooking into operating system facilities to package your functionality so it can
be started, stopped, configured etc externally, and run independently (autostart).

This is (or will be) a portable framework, supporting either UNIX style services
(e.g /etc/init.d/myservice start), or the windows service framework.

## Modules

- [Main.h](Main.h)
