# Stroika::Foundation::[Database](../)::SQL

This folder contains all the Stroika Library Foundation::[Database](../)::Document (document database) source code.

## Modules

- Interfaces/Ptr classes

  - [Connection.h](Connection.h) - IRep/Ptr for generic SQL database connection
  - [Transaction.h](Transaction.h) - IRep/Ptr for generic SQL database transaction

- Concrete

  - [MongoDBClient.h](MongoDBClient.h) - Concrete MongoDB client (implements abstract Connection etc interfaces)
  - [SQLite.h](SQLite.h)- Concrete SQLite client (implements abstract Connection etc interfaces)
