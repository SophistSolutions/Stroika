# Stroika::[Foundation](../ReadMe.md)::Math

This folder contains all the Stroika Library [Foundation](../ReadMe.md)::Math source code.

## Modules

- [Angle.h](Angle.h)
- [Common.h](Common.h)
- [LinearAlgebra/](LinearAlgebra/ReadMe.md)
- [Optimization/](Optimization/ReadMe.md)
- [ReBin.h](ReBin.h)
- [Statistics.h](Statistics.h)

## TODO DOCS

- We could use a better top-level organization for these various math-related

  algorithms. BUt I guess it can wait til we ahve a few more

- Consider adding new "FuzzyValue" - a value type with a builtin 'error' value. You could use it as a regular value, but with a high uncertainty (infinite?) - it could be treated as an unknown, and then possibly interestingly combined with the Range<> code - to create a 'fuzzy range'?

- Consider adding STDFLOAT analog to stdint

- Fast\_... etc asLeast32, etc
