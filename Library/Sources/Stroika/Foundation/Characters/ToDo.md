o	Consider adding class 'cstr'
	It would have a constructor taking (const wchar_t*) or wstring, and would have an operator const wchar_t* method.
	Nothing else. It would internally store a wstring.

	The point of this is that then things like Duration (and other such classes) could return soemthing that mostly
	acted like a const wchar_t*, but had safe lifetime semantics.