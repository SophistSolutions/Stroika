target_sources (stroika-foundation PRIVATE
    Connection.cpp
    ODBC.cpp
    SQLite.cpp
    Statement.cpp
    Utils.cpp
)

add_subdirectory (ORM)
