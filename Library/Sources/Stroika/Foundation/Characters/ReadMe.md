# Stroika::[Foundation](../)::Characters

This Folder contains the [Foundation](../)::Characters source code.

Code to manage Strings, formatting of strings, RegularExpressions, LineEndings, and CodePage conversions.

Also, generic ToString () support - to convert objects to a string format.

By **far** the most important part would be the [String.h](String.h) **String** class

Modules

  - [Character.h](Character.h) - Unicode **Character** class
  - [CodePage.h](CodePage.h)
  - [CodeCvt.h](CodeCvt.h) - generic character encoding conversion utilities
  - [Concrete/](Concrete/)
  - [CString/](CString/)
  - [Format.h](Format.h) - function **Format()**
  - [LineEndings.h](LineEndings.h)
  - [Number.h](Number.h) - **Number::Parse()**, and the options **ToString()** takes for a number
  - [Platform/ReadMe.md](Platform/)
  - [RegularExpression.h](RegularExpression.h) - class **RegularExpression**
  - [SDKChar.h](SDKChar.h)
  - [SDKString.h](SDKString.h)
  - [String.h](String.h) - class **String**
  - [StringBuilder.h](StringBuilder.h) - class **StringBuilder** for efficient generation of new Strings
  - [TextConvert.h](TextConvert.h) - Byte Order Mark (BOM)
  - [ToString.h](ToString.h) - function **ToString()** - like std::to_string() but with Stroika String class, and various options like **FloatToStringOptions** (a **Notation** and a **Precision**), etc...
