# WebServices sample

This sample application shows how to integrate with the Stroika WebServer and WebServices frameworks to build web-service functionality into
your application.

This sample guides you to breakup your web-service application into serveral parts:

- Model - the objects you read/write/manipulate through your web service API (things that have to be serialized/deserialized)
- IWSAPI - the abstract C++ API defininng what methods can be called (pure C++ objects, no marshalling etc)
- WSImpl - the pure application logic part of your webservices. Here you inherit from IWSAPI, and simple return the appropriate C++ object results.
- WebServer - this ties together the abstract interface with URL rules (routes etc), and simply maps delegated route handlers to the IWSAPI


* To test this example:
  * Run the service (under the debugger if you wish)
  * curl  http://localhost:8080/ OR
  * curl  http://localhost:8080/FRED OR      (to see rror handling)
  * curl -H "Content-Type: application/json" -X POST -d '{"AppState":"Start"}' http://localhost:8080/SetAppState
