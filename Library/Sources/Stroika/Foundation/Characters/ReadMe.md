# Stroika::[Foundation](../)::Characters

This Folder contains the [Foundation](../)::Characters source code.

Code to manage Strings, formatting of strings, RegularExpressions, LineEndings, and CodePage conversions.

Also, generic ToString () support - to convert objects to a string format.

Numbers and text, in both directions:

- number -> text: **ToString()** ([ToString.h](ToString.h)) - for a floating point number with **FloatToStringOptions**
- text -> number: **ParseInteger()** and **ParseFloat()** ([ParseNumber.h](ParseNumber.h)), each with its own options
- the types both directions share - a locale, significant figures, float formats: [NumberFormat.h](NumberFormat.h)

- Modules

  - [Character.h](Character.h)
  - [CodePage.h](CodePage.h)
  - [CodeCvt.h](CodeCvt.h) - generic character encoding conversion utilities
  - [Concrete/](Concrete/)
  - [CString/](CString/)
  - [FloatConversion.h](FloatConversion.h)
  - [Format.h](Format.h) - function **Format()**
  - [LineEndings.h](LineEndings.h)
  - [NumberFormat.h](NumberFormat.h) - the types describing numbers as text, shared by ToString () and ParseNumber.h
  - [ParseNumber.h](ParseNumber.h) - functions **ParseInteger()**, **ParseFloat()** - strict text-to-number conversion
  - [Platform/ReadMe.md](Platform/)
  - [RegularExpression.h](RegularExpression.h) - class **RegularExpression**
  - [SDKChar.h](SDKChar.h)
  - [SDKString.h](SDKString.h)
  - [String.h](String.h) - class **String**
  - [String2Int.h](String2Int.h) - DEPRECATED: use ParseNumber.h
  - [StringBuilder.h](StringBuilder.h) - class **StringBuilder**
  - [TextConvert.h](TextConvert.h) - Byte Order Mark (BOM)
  - [ToString.h](ToString.h) - function **ToString()**
