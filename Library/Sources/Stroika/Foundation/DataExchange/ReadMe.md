# Stroika::[Foundation](../ReadMe.md)::DataExchange

This folder contains all the Stroika Library [Foundation](../ReadMe.md)::DataExchange source code.

This module is about data structures and formats - and NOT about the IO/networking interactions of
an exchagne.

This module contains code relating to serialization and deserializaiton of structured data.

Most of these modules will use [VariantValue](./VariantValue.h) as a linga-franca to express structured data
going in/out of a module.

(e.g. JSON or XML) reader / writer <--> VariantValue <-- (via ObjectVariantMapper)--> C++ struct

Most Central Classes:

- [Archive](Archive/ReadMe.md)

  Algorithms that provide storage of (often folders) of data. This is for formats like 'zip' file, or .7z files

- [Compression](Compression/ReadMe.md)

  Algorithms that provide compression/decompression of streams of data

- DefaultSerializer - [DefaultSerializer.h](DefaultSerializer.h)

  This is NOT for external serialization (generally) - but useful for internally temporarily serializing an object as a BLOB, to be able to compare it and perhaps transfer it. For example, if you want to genrically be able to compute a HASH on an object, this is what is used by the Hash\<> infrastructure.

- ObjectVariantMapper - [ObjectVariantMapper.h](ObjectVariantMapper.h)

  ObjectVariantMapper is a tool which automates conversion of C++ types from/to VariantValue objects (which then allows them to be trivially serialized/deserialized in JSON or other formats)

- VariantValue - [VariantValue.h](VariantValue.h)

  This is only moderately similar to std::variant\<>. Its more similar to MSFT COM VARIANT_VALUE. Its an object with a fixed enumeration of possible types (closely fitting the data model used by python, JSON, and other weakly typed languages). It's ideal as a lingua-franca for exchanging data between strongly typed C++ structures and external formats like JSON.

- [Variant](Variant/ReadMe.md)

  Algorithms that convert between VariantValues and forms like XML, JSON, CSV, etc..

Others:

- Atom - [Atom.h](Atom.h)
- BadFormatException - [BadFormatException.h](BadFormatException.h)
- CheckedConverter - [CheckedConverter.h](CheckedConverter.h)
- InternetMediaType - [InternetMediaType.h](InternetMediaType.h)
- InternetMediaTypeRegistry - [InternetMediaTypeRegistry.h](InternetMediaTypeRegistry.h)
- OptionsFile - [OptionsFile.h](OptionsFile.h)

  Utility class for storing application options/configuration

- StructFieldMetaInfo - [StructFieldMetaInfo.h](StructFieldMetaInfo.h)

- [StructFieldMetaInfo/](StructFieldMetaInfo/ReadMe.md)
- [XML/](XML/ReadMe.md)
