
# WebServer Stroika Sample

Example of how to use the web server framework, including routers, interceptors, etc.

This can easily be integrated with the service framework (see Service framework sample).

* To test this example:
  * Run the webserver application (under the debugger if you wish)
  * curl  http://localhost:8080/ OR
  * curl  http://localhost:8080/FRED OR      (to see rror handling)
  * curl -H "Content-Type: application/json" -X POST -d '{"AppState":"Start"}' http://localhost:8080/SetAppState

**NOTE** this program 

(make sure you run with 'current directory == top level directory of this sample else you wont find sample-html-folder)

ADD html query example