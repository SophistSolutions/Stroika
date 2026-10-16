# This folder contains all the Stroika Library [Foundation](../../)::[DataExchange](../)::Variant source code

This module provides readers and writers which map between VariantValue objects and a format - JSON, XML, INI, ... - read
from, or written to, streams (InputStream::Ptr, OutputStream::Ptr, std streams), BLOBs and Strings.

A Reader or Writer is configured when it is constructed - its format's constructor arguments, often an Options - and
immutable after: every method is const. To read or write differently, construct another - that is cheap. Copies share one
representation, never modified, so a Reader or Writer is cheap to copy - to store, to pass by value, to hand out from a
factory.

### Abstract Interfaces:

- [Reader.h](Reader.h)
- [Writer.h](Writer.h)

### Supported Concrete formats:

- CharacterDelimitedLines

  - [Reader.h](CharacterDelimitedLines/Reader.h)
  - [Writer.h](CharacterDelimitedLines/Writer.h)

- FormURLEncoded

  - [Reader.h](FormURLEncoded/Reader.h)
  - [Writer.h](FormURLEncoded/Writer.h)

- INI

  - [Profile.h](INI/Profile.h)
  - [Reader.h](INI/Reader.h)
  - [Writer.h](INI/Writer.h)

- JSON

  - [Reader.h](JSON/Reader.h)
  - [Writer.h](JSON/Writer.h)

- [XML](XML/ReadMe.md)

  - [Reader.h](XML/Reader.h)
  - [Writer.h](XML/Writer.h)
