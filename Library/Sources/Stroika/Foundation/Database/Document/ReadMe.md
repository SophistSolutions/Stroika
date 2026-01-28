# Stroika::Foundation::[Database](../)::SQL

This folder contains all the Stroika Library Foundation::[Database](../)::Document (document database) source code.

Document database follow the simple patter of records (VariantValue) each associated with an ID, and items can be looked up
by ID (see https://en.wikipedia.org/wiki/Document-oriented_database).

## Modules

- Interfaces/Ptr classes

  - [Connection.h](Connection.h) - IRep/Ptr for generic database connection
  - [Collection.h](Collection.h) - IRep/Ptr for generic database collection (aka table)
  - [ObjectCollection.h](ObjectCollection.h) - subclass of Collection, but adding use of ObjectVariantMapper to operate on actual objects directly
  - [Transaction.h](Transaction.h) - IRep/Ptr for generic database transaction

- Concrete

  - [MongoDBClient.h](MongoDBClient.h) - Concrete MongoDB client (implements abstract Connection etc interfaces)
  - [SQLite.h](SQLite.h)- Concrete SQLite client (implements abstract Connection etc interfaces)
  - [LocalDocumentDB.h](LocalDocumentDB.h)- store data in RAM, or filesystem - modest performance/capabilities, but simple, and flexible for small cases (fewer dependencies)

## Sample

  - [Samples/DocumentDB](../../../../../../Samples/DocumentDB/ReadMe.md)
