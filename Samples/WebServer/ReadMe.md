
# WebServer Stroika Sample

Example of how to use the web server framework, including routers, interceptors, etc.

This can easily be integrated with the service framework (see Service framework sample).

- To test this example:
 -      o   Run the service (under the debugger if you wish)
 -      o   curl  http://localhost:8080/ OR
 -      o   curl  http://localhost:8080/FRED OR      (to see error handling)
 -      o   curl -H "Content-Type: application/json" -X POST -d '{"AppState":"Start"}' http://localhost:8080/SetAppState
 -      o   curl  http://localhost:8080/Files/foo.html -v

**NOTE** this program 

(make sure you run with 'current directory == top level directory of this sample else you wont find sample-html-folder)

ADD html query example

/*
 *  To test this example: (make sure you run with 'current directory == top level directory of this sample else you wont find sample-html-folder)
 *
 */

