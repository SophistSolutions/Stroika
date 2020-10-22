# This folder contains all the Stroika Library [Foundation](../ReadMe.md)::[DataExchange](../ReadMe.md)::Archive source code

This module provides readers and writers which read and write directory hierarchies of BLOBs.

Supported are formats like Zip files, 7z archives, tar, ar, etc (some not yet implemented).

Abstract Interfaces:

- [Reader.h](Reader.h)

Supported Concrete formats:

- [7z/Reader.h](7z/Reader.h)
- [Zip/Reader.h](Zip/Reader.h)
