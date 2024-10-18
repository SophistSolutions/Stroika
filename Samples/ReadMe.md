# Stroika Samples

* [ActiveLedIt](ActiveLedIt/)
  is a word-processor activeX control. This works only for Windows.
  ActiveLedIt is based on the Stroika Led WordProcessor framework

* [AppSettings](AppSettings/)
  is a simple example of using ObjectVariantMapper to map objects to/from JSON, 
  combined with OptionsFile utility class to store JSON content in the file system (managing version/format upgrades etc),
  along with ModuleGetterSetter to manage all this on a per-module basis (so settings can be defined/saved per module or globally)
  and accessed automatically in a threadsafe manner (automatically synchronized).

* [ArchiveUtility](ArchiveUtility/)
  is a simple command-line tool to create/update/list/etc archives, like zip files, or 7z files.
  This demonstrates use of the Stroika DataExchange/Archive classes.

* [Containers](Containers/)
  demonstrates the basic ideas of the Stroika container class library

* [HTMLUI](HTMLUI/)
  **very good place to start**

  WebService app, auto startup operating system service, installers, build from docker, run under docker.

  This sample combines all of what you need to build an installable app, which runs as a platform-native service,
  and exposes its HTML user interface (using vue3 / quasar).

* [LedIt](LedIt/)
  is a word processor application. It was originally working on Windows,
  MacOS, and Linux, but only the Windows code has been ported and tested
  successfully on Stroika (Led v3.x code it was based on worked on the
  other platforms years ago).
  LedIt is based on the Stroika Led WordProcessor framework.

* [LedLineIt](LedLineIt/)
  is a text editor application. This version only works on Windows, but it
  was ported from code (Led 3.1x) that also had implementations for MacOS, and
  Linux, and so could be ported/targetted for those OSes without too much difficulty.
  LedLineIt is based on the Stroika Led Text Editor framework.

* [Serialization](Serialization/)
  demonstrates facilities for serializing (and deserializing) objects - the basis of many Stroika
  features (e.g. webservices, OptionsFile).

* [Service](Service/)
  is a simple demonstration of how to use the Stroika Service Framework. Makes it easy to create
  services that integrate with UNIX service systems (init/systemd etc) or windows service manager. Contains source for installers in WIX (MSI), RPM, and DEB.

* [SimpleLedTest](SimpleLedTest/)
  tests the simple-to-use Led library wrapper code. This can easily
  be adapted to work for MacOS (Carbon SDK) and Linux / X-Windows, but
  this code is only tested for Windows).
  SimpleLedTest is based on the Stroika Led WordProcessor framework.

* [SQL](SQL/)
  Simple example program using the Stroika SQL wrappers to access/update an SQL database.
  Only SQLite fully supported for now.

* [SSDPClient](SSDPClient/)
  shows how to user the Stroika UPnP framework to search (SSDP) for UPnP devices.

* [SSDPServer](SSDPServer/)
  shows how to user the Stroika UPnP framework to make a device/service advertised via SSDP (UPnP)

* [SystemPerformanceClient](SystemPerformanceClient/)
  is a simple demonstration of how to use the Stroika System Performance framework. This shows how to capture
  system performance data like CPU usage, network usage, filesystem usage, process information etc, portably.

* [Traceroute](Traceroute/)
  is networking example, implementing both the 'ping' application and 'traceroute' - based on Stroika-layer networking.

* [WebServer](WebServer/)
  is an example of how to use the web server framework, including routers, interceptors, etc.

* [WebService](WebService/)
  is an example of how to use the web service framework. The sample separates the WebService model marshalling from the service API, and it separates implementation of webservice from mapping the APIs into a (typically RESTFUL) WebServer route mapping.
