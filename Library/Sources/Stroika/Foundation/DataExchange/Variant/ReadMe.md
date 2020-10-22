# This folder contains all the Stroika Library [Foundation](../../ReadMe.md)::[DataExchange](../ReadMe.md)::Variant source code

This module provides readers and writers which convert binary streams to binary streams
of the same data

This module provides readers and writers which map between VariantValue objects
and InputStreamPtr<> and OutputStream<> object.

### Abstract Interfaces:

- [Reader.h](Reader.h)
- [Writer.h](Writer.h)

### Supported Concrete formats:

- CharacterDelimitedLines

  - [Reader.h](CharacterDelimitedLines/Reader.h)
  - [Writer.h](CharacterDelimitedLines/Writer.h)

- INI

  - [Profile.h](INI/Profile.h)
  - [Reader.h](INI/Reader.h)
  - [Writer.h](INI/Writer.h)

- JSON

  - [Reader.h](JSON/Reader.h)
  - [Writer.h](JSON/Writer.h)

- XML

  - [Reader.h](XML/Reader.h)
  - [Writer.h](XML/Writer.h)
