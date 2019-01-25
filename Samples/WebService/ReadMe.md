# WebServices sample

- totally NYI

* To test this example:
  * Run the service (under the debugger if you wish)
  * curl  http://localhost:8080/ OR
  * curl  http://localhost:8080/FRED OR      (to see rror handling)
  * curl -H "Content-Type: application/json" -X POST -d '{"AppState":"Start"}' http://localhost:8080/SetAppState
