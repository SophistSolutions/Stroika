o	Consider adding template class 'cstr'
	It would have a constructor taking (const wchar_t*) or wstring, and would have an operator const wchar_t* method.
	Nothing else. It would internally store a wstring.

	The point of this is that then things like Duration (and other such classes) could return something that mostly
	acted like a const wchar_t*, but had safe lifetime semantics.

	Note that cstr could be templated on char_type, and so be used for wstring and string.

	Implementing this would be EASY, and using it EASY. But the main reason NOT to do this
	is that it may not work as desired. When you have APIs that take a char*/wchar_t* - it would work great.
	But I've found MOST of the time where I need to get a const wchar_t* rep from something is for
	elipsis params (sprintf/Format/printf/Logger::Log (), etc). And since conversion operators don't
	work with elipsis params, this could just be a boondogle.