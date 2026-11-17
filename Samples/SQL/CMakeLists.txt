add_executable (SQL Sources/ComputerNetwork.cpp Sources/DirectEmployeesDB.cpp Sources/Main.cpp Sources/ORMEmployeesDB.cpp Sources/ThreadTest.cpp)
target_link_libraries (SQL PRIVATE stroika-frameworks)
StroikaCopyFilesToExeDir (SQL)
