add_executable (DocumentDB Sources/ComputerNetwork.cpp Sources/EmployeesDB.cpp Sources/Main.cpp)
target_link_libraries (DocumentDB PRIVATE Stroika::Foundation)
StroikaCopyFilesToExeDir (DocumentDB)
