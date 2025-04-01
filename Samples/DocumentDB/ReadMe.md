# Stroika Samples

## DocumentDB

Demonstrates facilities for creating/updating simple database using DocumentDB support

- This sample is EXTREMELY similar to the SQL sample, and except uses SQLite or MongoDB
  and is simpler, due to there being less of a cognitive mismatch between objects and
  the DB.

### Details

#### Source
  - [ComputerNetwork.cpp](./Sources/ComputerNetwork.cpp)
  - [EmployeesDB.cpp](./Sources/EmployeesDB.cpp)
  - [Main.cpp](./Sources/Main.cpp)

#### Running a mongodb instance

One simple way to run mongo (works on windows or UNIX, if you have docker installed)

~~~
docker run --rm --name mongodb -d -p 27017:27017 -e MONGO_INITDB_ROOT_USERNAME=admin -e MONGO_INITDB_ROOT_PASSWORD=pass mongo:latest
~~~

#### Examining the mongodb

Either use 
 - Compoass
 - Mongo Shell https://www.mongodb.com/docs/v4.4/mongo/#the-mongo-shell


#### Run the sample
~~~
MONGO_CONNECTION_STRING=mongodb://admin:pass@hercules:27017  ./Builds/Debug/Samples-DocumentDB/DocumentDB.exe
~~~
OR
~~~
./Builds/Debug/Samples-DocumentDB/DocumentDB.exe --mongoConnectionString=mongodb://admin:pass@hercules:27017
~~~
