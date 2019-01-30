# WebService Sample

This sample application shows how to integrate with the Stroika WebServer and WebServices frameworks to build web-service functionality into
your application.

This example does not show the shortest/simplest path, but is organized in a pattern I've used successfully many times to separate implementation of webservice from the RESTUL API mapping. For the shortest and most direct approach, see the [WebServer](../WebServer/) sample and directly use routes.

This sample guides you to breakup your web-service application into serveral parts:

- [Model](Sources/Model.cpp) - the objects you read/write/manipulate through your web service API (things that have to be serialized/deserialized)
- [IWSAPI](Sources/IWSAPI.h) - the abstract C++ API defininng what methods can be called (pure C++ objects, no marshalling etc)
- [WSImpl](Sources/WSImpl.cpp) - the pure application logic part of your webservices. Here you inherit from IWSAPI, and simple return the appropriate C++ object results.
- [WebServer](Sources/WebServer.cpp) - this ties together the abstract interface with URL rules (routes etc), and simply maps delegated route handlers to the IWSAPI

- To test this example:
  - Run the service (under the debugger if you wish)
  - `curl  http://localhost:8080/` -- best if viewed in web browser

  - `curl -H "Content-Type: application/json" -X POST -d '{"arg1": 3, "arg2": 5 }' http://localhost:8080/plus --output -`
  - `curl -H "Content-Type: application/json" -X POST -d '{"arg1": 4.5, "arg2": -3.23 }' http://localhost:8080/minus --output -`
  - `curl -H "Content-Type: application/json" -X POST -d '{"arg1":"2 + 4i", "arg2": 3.2 }' http://localhost:8080/times --output -`
  - `curl -H "Content-Type: application/json" -X POST -d '{"arg1":"2 + i", "arg2": "2 - i' }' http://localhost:8080/times --output -`
  - `curl -H "Content-Type: application/json" -X POST -d '{"arg1": "2 + i", "arg2": 0 }' http://localhost:8080/divide --output -`
