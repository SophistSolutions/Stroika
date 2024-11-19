
# WebServer Stroika Sample

Simple example of how to use the web server framework, including routers, interceptors, FileSystemRequestHandler (http serve content from filesystem), etc.

This can easily be integrated with the service framework (see Service framework sample, or the HTMLUI sample).

To test this example:
  - Run the service (under the debugger if you wish)
    ~~~bash
    # assumes cd to StroikaRoot directory - top level of repo
    # assumes you built CONFIGURATION=Debug

    ./Builds/Debug/Samples-WebServer/WebServer -h
    ./Builds/Debug/Samples-WebServer/WebServer --quit-after 60
    ~~~
  - Then in another window, while the above is running
    ~~~bash
    curl  http://localhost:8080/
    curl  http://localhost:8080/FRED
    curl -H "Content-Type: application/json" -X POST -d '{"AppState":"see-echoed"}' http://localhost:8080/SetAppState
    curl  http://localhost:8080/Files/index.html -v
    # OR      (to see error handling)
    curl  http://localhost:8080/Files/filenotfound.html -v

Source Code:
 - [Sources/WebServer.cpp](Sources/WebServer.cpp)