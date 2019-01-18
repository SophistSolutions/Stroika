
WebServer Example of how to use the web server framework, including routers, interceptors, etc.

This can easily be integrated with the service framework (see Service framework sample)

* To test this example:
  * Run the service (under the debugger if you wish)
  * curl  http://localhost:8080/ OR
  * curl  http://localhost:8080/FRED OR      (to see rror handling)
  * curl -H "Content-Type: application/json" -X POST -d '{"AppState":"Start"}' http://localhost:8080/SetAppState
