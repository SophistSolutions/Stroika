# Stroika::[Frameworks](../ReadMe.md)::UPnP

This Folder contains the [Frameworks](../ReadMe.md)::UPnP Framework source code.

## Overview

The Stroika UPnP framework is meant to eventually provide basic UPnP functionality
including

- SSDP (service discovery)
- Events
- Control

  For starters however, and probably for Stroika v2, we will ONLY support SSDP.

## References

- http://www.upnp-hacks.org/upnp.html
- http://quimby.gnus.org/internet-drafts/draft-cai-ssdp-v1-03.txt
- https://wiki.gnome.org/action/show/Projects/GUPnP?action=show&redirect=GUPnP

## Sample/Demo apps

- [SSDPClient](../../../../../Samples/SSDPClient/ReadMe.md)

  This sample demonstrates lisening for announcemnts, and searching for devices.

- [SSDPServer](../../../../../Samples/SSDPServer/ReadMe.md)

  This sample demonstrates lisening for searches and responding, and periodic (and up/down) announcements

## Modules

- [Device.h](Device.h)
- [DeviceDescription.h](DeviceDescription.h)
- [SSDP/](SSDP/ReadMe.md)
