# Stroika::[Frameworks](../ReadMe.md)::SystemPerformance

This Folder contains the [Frameworks](../ReadMe.md)::SystemPerformance Framework source code.

## Overview

The Stroika UPnP framework is meant to eventually provide basic UPnP functionality
including

- SDP
- Events
- Control

  For starters however, and probably for Stroika v2, we will ONLY support SSDP.

SEE

- http://www.upnp-hacks.org/upnp.html
- http://quimby.gnus.org/internet-drafts/draft-cai-ssdp-v1-03.txt
- https://wiki.gnome.org/action/show/Projects/GUPnP?action=show&redirect=GUPnP

DEMO APP:
Write simple test app - while running broadcasts
Write simple test app - while running listens for all, or takes args to search for given string
(DEMO - NOT TEST)

    Client
    	o	Search
    	o	Listener
    	o	CachingListener


    Server:
    	o	Server

CLEINT CODE:
for listener:
create multicast socket. Send out igmp group add for multicast group.
listen for right format messages (distingish alive/dead and have different flags
we generate. Maybe even be smart about boucning, and track times, and if we get
a dead with TS out of order with alive, treat properly (based on TS, not order of
arrivaa)
for search - must send multicast message on port 1900.
Must allocate new UDP scoket on random port first (default). Send to muticast addr from that port and
response goes to sending (seraching) port.
Then listen (take param for timeout) - and return each answer. Maybe via callback interface for each
found Device.

## Modules

- [Device.h](Device.h)
- [DeviceDescription.h](DeviceDescription.h)
- [SSDP/](SSDP/ReadMe.md)
