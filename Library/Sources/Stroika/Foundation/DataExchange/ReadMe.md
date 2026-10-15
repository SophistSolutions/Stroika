# Stroika::[Foundation](../)::DataExchange

This folder contains all the Stroika Library [Foundation](../)::DataExchange source code.

This module is about data structures and formats - and NOT about the IO/networking interactions of
an exchange.

This module contains code relating to serialization and deserialization of structured data.

Most of these modules use [VariantValue](./VariantValue.h) as a lingua franca to express structured data going in and out of
a module - [Why VariantValue](#why-variantvalue), below.

(e.g. JSON or XML) reader / writer <--> VariantValue <-- (via ObjectVariantMapper)--> C++ struct

## Why VariantValue

[VariantValue](VariantValue.h) is the linchpin of Stroika's data code: the one value every format, database and service layer
reads and writes, so that each converts to and from C++ only once.

Though it began as Stroika's take on COM's `VARIANT`, its role goes past that. It holds the data model nearly every system that
exchanges or scripts data has settled on - a few kinds of scalar, an ordered list, and a map from strings to values:

| System | Its values | And VariantValue |
|---|---|---|
| Python | `None`, `bool`, `int`, `float`, `str`, `bytes`, `date`, `datetime`, `list`, `dict` | nearly one to one (Python's `int` is unbounded; VariantValue's has 64 bits) |
| JSON, JavaScript | null, boolean, number, string, array, object | a superset: VariantValue keeps integer, unsigned and float apart, and has dates and BLOBs - which JSON writes as strings |
| SQL | `NULL`, `INTEGER`, `REAL`, `TEXT`, `BLOB` (SQLite's storage classes), dates | the same scalars; and a row is a map from column to value - `SQL::Statement::Row` |
| BSON, CBOR, MessagePack, YAML, TOML | JSON's, with bytes and dates | the same, near enough |
| protobuf | `google.protobuf.Value`: null, number, string, bool, struct, list | the same idea, built again |
| COM, OLE Automation | `VARIANT` | where VariantValue began |

All of these share one design, which Lisp had first, in 1958: every value is one type - a tagged union, its tag saying which
kind it is - and generic code walks any of them. Python's `PyObject`, Lua's `TValue`, a JavaScript engine's values and COM's
`VARIANT` are that design again; what differs is the kinds in the union. VariantValue is it too, kept to data: no functions,
symbols or objects with an identity of their own, and immutable - so a value is always a tree, which can be written out
anywhere and read back. (Lisp's notation for its values, S-expressions, was the first JSON.) A uniform shape is what lets one
piece of code - a writer, a comparison, a patch - work on any data at all.

### What it buys

**Each format, and each C++ type, is written once.** A format - JSON, XML, INI, CSV, a form post - reads and writes
VariantValue, and knows nothing of your types. A type is mapped to and from VariantValue once, by an
[ObjectVariantMapper](ObjectVariantMapper.h) (and, in time, by reflection), and knows nothing of formats. So N formats and M
types take N + M pieces of code, not N × M: a new format serves every type at once, and a new type every format.

**Generic tools work on any data**: comparison, `ToString` for logging and debugging, [JSON Pointer and
Patch](JSON/ReadMe.md). And data can be changed as data before it is mapped - as [OptionsFile](OptionsFile.h)'s upgrader turns
an old version's VariantValue into the current one's.

**It is cheap to pass around.** A VariantValue is immutable and shared: a copy is a pointer copy, so it moves as freely as a
String.

### What is built on it

- every [Variant](Variant/ReadMe.md) reader and writer: JSON, XML, INI, CharacterDelimitedLines, FormURLEncoded
- [ObjectVariantMapper](ObjectVariantMapper.h), and [OptionsFile](OptionsFile.h)'s configuration files
- the databases: [SQL](../Database/SQL/)'s bound parameters and rows, its ORM, and the [document databases](../Database/Document/) - a
  document is a `Mapping<String, VariantValue>` (MongoDB, SQLite)
- the [web service framework](../../Frameworks/WebService/): a request's arguments and its result, mapped to and from C++ objects;
  JSON-RPC; OpenAPI
- [SystemPerformance](../../Frameworks/SystemPerformance/)'s measurements, and [UPnP](../../Frameworks/UPnP/)'s descriptions

### Which types, and why

A type is in the model when many of the systems above carry it natively, so that it can pass through without becoming something
else. Anything else is a convention the mapper keeps - usually a string.

- **In**: null, Boolean, integer and unsigned integer (the widest of each - `As<T> ()` narrows on the way out, and most formats
  keep no width anyway), float, String, BLOB, Date, DateTime, array, and a map from String.
- **Left to the mapper**: enumerations (by name, or number), durations, times of day, UUIDs, sets, and maps whose keys are not
  strings (as arrays of pairs).

### Limits, and where they are going

- **It is a tree, built whole.** A conversion holds the entire document as a VariantValue. That is the simple and flexible
  choice - and what lookahead, patching and upgrades need - but not the fastest for very large data: an optional streaming
  path, as Rust's serde has, is [#1227](https://github.com/SophistSolutions/Stroika/issues/1227).
- **Numbers**: a float is held as a `long double`, so its text differs by platform, and the JSON writer defaults to six
  significant digits ([#1223](https://github.com/SophistSolutions/Stroika/issues/1223)); and there is no exact decimal - SQL's
  `DECIMAL`, money ([#1225](https://github.com/SophistSolutions/Stroika/issues/1225)).
- **A map keeps no order**: members come back in hash order ([#1224](https://github.com/SophistSolutions/Stroika/issues/1224)).
- **A DateTime holds whole seconds** ([#1228](https://github.com/SophistSolutions/Stroika/issues/1228)).
- **Some values look alike** through some formats: null and an empty string, say.
- **XML says more than this model** - attributes, the order of elements, namespaces, mixed content. A
  [Variant::XML::Binding](Variant/XML/Binding.h) says how a value is written in it.
- **Every scalar is an allocation of its own** ([#1226](https://github.com/SophistSolutions/Stroika/issues/1226)).

## Modules

Most Central Classes:

- [Archive](Archive/ReadMe.md)

  Algorithms that provide storage of (often folders) of data. This is for formats like 'zip' file, or .7z files

- [Compression](Compression/ReadMe.md)

  Algorithms that provide compression/decompression of streams of data

- DefaultSerializer - [DefaultSerializer.h](DefaultSerializer.h)

  This is NOT for external serialization (generally) - but useful for internally temporarily serializing an object as a BLOB, to be able to compare it and perhaps transfer it. For example, if you want to genrically be able to compute a HASH on an object, this is what is used by the Hash\<> infrastructure.

- [JSON-specific utilities](JSON/ReadMe.md)

- ObjectVariantMapper - [ObjectVariantMapper.h](ObjectVariantMapper.h)

  ObjectVariantMapper is a tool which automates conversion of C++ types from/to VariantValue objects (which then allows them to be trivially serialized/deserialized in JSON or other formats)

- VariantValue - [VariantValue.h](VariantValue.h)

  A value of one of a fixed set of types - the data model of JSON, Python, SQL and most other interchange formats - and so the
  lingua franca between strongly typed C++ structures and those formats. [Why VariantValue](#why-variantvalue), above.

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
