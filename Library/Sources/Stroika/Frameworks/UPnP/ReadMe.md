# Stroika::[Frameworks](../)::UPnP

This Folder contains the [Frameworks](../)::UPnP Framework source code.

## Overview

The Stroika UPnP framework provides the parts of the UPnP Device Architecture a device and a control point need - version
1.0, [on purpose](#upnp-device-architecture-version-10-not-11); the section numbers here and in the code are 1.1's:

- Discovery (section 1): SSDP - advertising a device, and finding devices
- Description (section 2): a device's description, and each of its services' (its SCPD)
- Control (section 3): an action's request, and its response or error - SOAP's RPC ([WebService::SOAP](../WebService/SOAP.h))
  with UPnP's conventions on top
- Eventing (section 4): GENA - a service telling its subscribers each change of its state

## UPnP Device Architecture version: 1.0, not 1.1

**Claiming a version, and using its features, are different things.** A device claims a version in its SERVER header
(UPnP/1.x - `SSDP::MakeServerHeaderValue`) and in its descriptions' `specVersion`, and a claim of 1.1 commits it to every
part 1.1 requires. Using a single 1.1 feature while claiming 1.0 is allowed - receivers ignore headers they do not know -
and common: most devices on a home network (surveyed 2026-10) claim 1.0, yet send 1.1's `BOOTID.UPNP.ORG`. A control
point claims next to nothing; it just makes use of whatever devices send.

Stroika's devices claim 1.0. They follow some of 1.1's rules anyway, as those cost nothing: NOTIFYs and M-SEARCHes go out
with TTL 2 (1.1, section 1.1); a multicast M-SEARCH is answered within its MX, at most 5 seconds (1.3.3), and a unicast one
at once (1.3.2); a description carries no `URLBase` unless the application sets one (2.3); and GENA gives a subscription
asking for "infinite" a finite duration (4.1.2).

### What claiming 1.1 would take

- `BOOTID.UPNP.ORG` and `CONFIGID.UPNP.ORG` in every NOTIFY and search answer (`SSDP::Advertisement`,
  `Server::PeriodicNotifier`, `Server::SearchResponder`). BOOTID must rise at every start, and whenever the device's
  networks change - after first sending an ssdp:update that carries `NEXTBOOTID.UPNP.ORG` (`Server::PeriodicNotifier`).
  CONFIGID must change whenever any description does.
- `specVersion` 1.1, and a `configId` attribute equal to CONFIGID, in `DeviceDescription`'s and every
  `ServiceDescription`'s document (the embedded schemas allow it).
- Then the claim itself: UPnP/1.1 in the SERVER header.

### Why not (decided 2026-10, Stroika v3.0d25)

- **The benefit is other people's, and modest**: their control points would notice a Stroika device restarting (BOOTID),
  cache its descriptions across restarts (CONFIGID), and not mistake its network changes for restarts (ssdp:update).
  Nothing built on Stroika needs it, and 1.1 control points must work with 1.0 devices anyway.
- **The risk**: a 1.1 control point TRUSTS these values. A BOOTID that goes backwards, or jumps without an ssdp:update,
  makes it drop its subscriptions; a CONFIGID left unchanged after a description changes makes it keep a stale copy.
  Claiming 1.0 rules out that whole class of bug - which sending BOOTID under a 1.0 claim does not, since control points
  read it whenever it is there.
- **The design it needs**: where BOOTID comes from - a stored counter, or the clock, which can go backwards (a Raspberry
  Pi's, before NTP sets it); and where CONFIGID comes from - the application, not `Server::BasicServer`, serves the
  descriptions, so one value has to reach both.
- **Not because other stacks skip 1.1**: of the common ones (checked 2026-10), miniupnpd and GNOME's gssdp send BOOTID
  and CONFIGID, and read BOOTID as control points; only libupnp does neither.

Revisit when a device built on Stroika must claim 1.1 (to be certified, say).

### The control point side - needs no claim, and is worth doing

When a device's BOOTID jumps without an ssdp:update, its subscriptions are gone: `GENA::Subscriber` should subscribe
again, told by `Client::CachingListener` tracking BOOTID ([#1211](https://github.com/SophistSolutions/Stroika/issues/1211)).
That works against most devices today, whatever they claim.

### Optional parts, claimed or not

Multicast eventing (4.2), extended data types (2.5.1), `USER-AGENT` headers saying UPnP/1.1 - and `SEARCHPORT.UPNP.ORG`
(1.2.2): a device that cannot receive unicast M-SEARCHes on port 1900 listens on a port of its own and announces it, and
a control point searching it by unicast sends there instead. On Windows, a unicast M-SEARCH reaches a Stroika device only
while Windows' own SSDP Discovery service is not running: that service (started on demand) binds port 1900 on each of
the machine's addresses, and Windows gives a unicast datagram to a socket bound to its address before one bound to all
(tested 2026-10, Windows 11). Not done: neither the common stacks nor a home network's devices send it (2026-10), so few
control points would look for it; and `Client::Search` searches only by multicast, which it does not touch.

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
