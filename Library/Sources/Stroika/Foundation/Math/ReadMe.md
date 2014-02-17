This Folder contains the Foundation::Math source code.

	TODO:
		o	We could use a better top-level organization for these various math-related
 			algorithms. BUt I guess it can wait til we ahve a few more

		o	Consider adding new "FuzzyValue" - a value type with a builtin 'error' value. You could use it
			as a regular value, but with a high uncertainty (infinite?) - it could be treated as an unknown, and then
			possibly interestingly combined with the Range<> code - to create a 'fuzzy range'?

		o	Consider adding STDFLOAT analog to stdint 

			Fast_... etc asLeast32, etc
