# Stroika Library Frameworks

This folder contains all the Stroika Library Frameworks source code.

- [Led](Led/ReadMe.md)

  A rich text processing and editing framework (includes sample LedIt word processor application, and LedLineIt, text editor)

- [Modbus](Modbus/ReadMe.md)

  A library to interact with/exchange data over [Modbus](https://en.wikipedia.org/wiki/Modbus)

- [NetworkMonitor](NetworkMonitor/ReadMe.md)
  Framework for capturing network measurements, probes, etc.

  This is for things like ping, traceroute, but not for things like SNMP network data (which is in system monitor framework)

- [Service](Service/ReadMe.md)

  Package a program in the form of a UNIX or Windows service

- [System Performance](SystemPerformance/ReadMe.md)

  Measure CPU usage, network usage, disk activity etc for a system, in a very straightfoward and system portable way

- [UPnP](UPnP/ReadMe.md)

  Universal Plug and Play
  The only part currently implemented is SSDP

- [Web Server](WebServer/ReadMe.md)

  Provide basic web server functionality within your application, including routers, interceptors etc.

- [Web Service](WebService/ReadMe.md)

  Expose programatic APIs through web services (SOAP, JSON, REST).
  This framework depends on the Web-Server framework

### Future

- DNS-SD (DNS Service Discovery)

  [FUTURE PROJECT]

  http://www.dns-sd.org/
  http://www.ietf.org/rfc/rfc6763.txt

  [weakly-depends (not required) on mDNS framework]

  Nothing implemented so far.

- mDNS [FUTURE PROJECT]

  http://en.wikipedia.org/wiki/Multicast_DNS
  http://www.ietf.org/rfc/rfc6762.txt

  Nothing implemented so far.

- WebKits (webkit+stroika)

  [FUTURE PROJECT]

  (integrate / wrapper on webkit for building gui apps)
  (make easy to build hybrid apps)
  (integrate with HTMLViewCompiler)
  (integrate with tool to compile binary data to .o file, and include in file exe
  with directory, and use that for webkit to find files/resources)
  (Integrate with stroika webserver module for easy ajax)
  (when all done - SB able to do hybrid apps like HealthFrame - but
  portably)

  Nothing implemented so far.
