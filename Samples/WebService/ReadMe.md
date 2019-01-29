# WebServices sample

This sample application shows how to integrate with the Stroika WebServer and WebServices frameworks to build web-service functionality into
your application.

This example does not show the shortest/simplest path, but is organized in a pattern I've used many times to separate implementation of webservice from mapping the APIs into a (typically RESTFUL) WebServer route mapping. For the simplest approach, see the [WebServer](../WebServer/) sample.

This sample guides you to breakup your web-service application into serveral parts:

- Model - the objects you read/write/manipulate through your web service API (things that have to be serialized/deserialized)
- IWSAPI - the abstract C++ API defininng what methods can be called (pure C++ objects, no marshalling etc)
- WSImpl - the pure application logic part of your webservices. Here you inherit from IWSAPI, and simple return the appropriate C++ object results.
- WebServer - this ties together the abstract interface with URL rules (routes etc), and simply maps delegated route handlers to the IWSAPI

- To test this example:
  - Run the service (under the debugger if you wish)
  - curl  http://localhost:8080/ -- best if viewed in web browser

  - curl -H "Content-Type: application/json" -X POST -d '{"arg1":"2 + i", "arg2": 3.2 }' http://localhost:8080/plus --output -
  - curl -H "Content-Type: application/json" -X POST -d '{"arg1":"2 + i", "arg2": 3.2 }' http://localhost:8080/minus --output -
  - curl -H "Content-Type: application/json" -X POST -d '{"arg1":"2 + i", "arg2": 3.2 }' http://localhost:8080/times --output -
  - curl -H "Content-Type: application/json" -X POST -d '{"arg1":"2 + i", "arg2": 3.2 }' http://localhost:8080/divide --output -
