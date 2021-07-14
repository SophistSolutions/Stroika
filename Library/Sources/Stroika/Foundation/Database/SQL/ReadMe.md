# Stroika::Foundation::[Database](../ReadMe.md)::SQL

This folder contains all the Stroika Library Foundation::[Database](../ReadMe.md)::SQL source code.

## Modules

- Interfaces/Ptr classes

  - [Connection.h](Connection.h) - IRep/Ptr for generic SQL database connection
  - [Statement.h](Statement.h) - IRep/Ptr for generic SQL database statment
  - [Transaction.h](Transaction.h) - IRep/Ptr for generic SQL database transaction

- Concrete

  - [ODBCClient.h](ODBCClient.h) - Concrete ODBC client (implements abstract Connection etc interfaces)
  - [SQLite.h](SQLite.h)- Concrete ODBC client (implements abstract Connection etc interfaces)

- Other
  - [Utils.h](Utils.h)
  - [ORM/](ORM/ReadMe.md) - Map between VariantValue tree structures and relational database
