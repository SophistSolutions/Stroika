# Stroika::[Frameworks](../)::UPnP

This Folder contains the [Frameworks](../)::UPnP Framework source code.

## Overview

The Stroika UPnP framework provides the parts of the UPnP Device Architecture (1.1) a device and a control point need:

- Discovery (section 1): SSDP - advertising a device, and finding devices
- Description (section 2): a device's description, and each of its services' (its SCPD)
- Control (section 3): an action's request, and its response or error - SOAP's RPC ([WebService::SOAP](../WebService/SOAP.h))
  with UPnP's conventions on top
- Eventing (section 4): GENA - a service telling its subscribers each change of its state

## References

- https://upnp.org/specs/arch/UPnP-arch-DeviceArchitecture-v1.1.pdf
- http://www.upnp-hacks.org/upnp.html
- http://quimby.gnus.org/internet-drafts/draft-cai-ssdp-v1-03.txt
- https://wiki.gnome.org/action/show/Projects/GUPnP?action=show&redirect=GUPnP

## Sample/Demo apps

- [SSDPClient](../../../../../Samples/SSDPClient/)

  A control point: finds devices (listening, and searching), reads their descriptions, switches each light it finds, and
  watches each service's events.

- [SSDPServer](../../../../../Samples/SSDPServer/)

  A device - a light: advertises itself, serves its descriptions, does its actions, and tells subscribers each change.

## Modules

- [DataTypes.h](DataTypes.h) - UPnP's data types, as text and back
- [Device.h](Device.h)
- [DeviceDescription.h](DeviceDescription.h)
- [GENA/](GENA/)
- [ServiceDescription.h](ServiceDescription.h)
- [SOAP/](SOAP/)
- [SSDP/](SSDP/)
