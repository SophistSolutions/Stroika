# HTML UI Big Integrated Sample

- C++ Stroika-based Backend
- Modern HTML UI frontend
- optional installers
- optionally run app in docker container

This sample application shows how to build a Stroika/C++ based backend, and a simple web UI, and pack them together
into a seemless application.

This example does not show the shortest/simplest path, but is organized in a pattern I've used successfully many times to separate implementation of webservice from the RESTUL API mapping. For the shortest and most direct approach, see the [WebServer](../WebServer/) sample and directly use routes.

This sample guides you to breakup your web-service application into serveral parts:

## Backend/C++/Stroika

- [AppConfiguration.h](Sources/AppConfiguration.h) / [AppConfiguration.cpp](Sources/AppConfiguration.cpp) - automatically managed serialization of per-app-instance configuration data
- [Model.h](Sources/Model.h) / [Model.cpp](Sources/Model.cpp) - the objects you read/write/manipulate through your web service API (things that have to be serialized/deserialized)
- [IWSAPI.h](Sources/IWSAPI.h) - the abstract C++ API defininng what methods can be called (pure C++ objects, no marshalling etc)
- [WSImpl.cpp](Sources/WSImpl.cpp) - the pure application logic part of your webservices. Here you inherit from IWSAPI, and simple return the appropriate C++ object results.
- [WebServer.cpp](Sources/WebServer.cpp) - this ties together the abstract interface with URL rules (routes etc), and simply maps delegated route handlers to the IWSAPI
- [Server.cpp](Sources/Server.cpp) - register app as operating system 'service' and manage startup of components
- [Main.cpp](Sources/Main.cpp) - Command-line processing, and basic overall app startup/flow.

- To test this example:
  - Run the service (under the debugger if you wish)
  - `curl  http://localhost:80/api` 
  

## Web UI

Simple app built using vue/quasar (has nothing todo with Stroika, can use any framework, but need something for a demo).
