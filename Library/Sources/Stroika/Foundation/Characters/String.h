/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Characters_String_h_
#define	_Stroika_Foundation_Characters_String_h_	1

/*
 *
 * Description:
 *
 *
 *		This family of classes using something akin to the Letter/Envelope paradigm
 *		described in Copliens "Advanced C++ Programming Styles and Idioms").
 *
 *		The class String () is the main thing that users of this class will see. In
 *		fact, it would be EXTREMELY unusual to interact with the class String in any
 *		other way. However, as part of the internal implementation, and in some cases
 *		for optimizations in places where an alternate text representation is preferred,
 *		there may be some interest in the "letters" inside of Strings.
 *
 *		The class that String's own is a reference counted pointer to String::Rep.
 *		This class is abstract, and defines a protocol for getting at the string data
 *		(Character*) and for getting/setting the string length. All the rest of the
 *		String class is built in this interface, and subclasses of String::Rep only
 *		needed follow that prootcol to be used interchangably with the rest of Strings.
 *
 *		This is which NEED be used. However, Stroika provides several String SUBTYPES
 *		which act EXACTLY like a String, but offer different performance behaviors.
 *
 *				o	String_BufferedArray
 *				o	String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly		(aka String_Constant)
 *				o	String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite
 *				o	String_ExternalMemoryOwnership_StackLifetime_ReadOnly
 *				o	String_ExternalMemoryOwnership_StackLifetime_ReadWrite
 *				o	String_Common
 */

#include	"../StroikaPreComp.h"

#include	"../Memory/SharedByValue.h"
#include	"TString.h"

#include	"Character.h"



/*
 * TODO:
 *		o	At least one StringRep variant (maybe the stanrdard/common string-buffer rep
 *			which has a fixed-size buffer, and uses that INLINE, and allocates POINTER if that isn't big enuf?
 */


/*
 * TODO:
 *		o	String::CTOR protected with REP should take funky ENUM - protected - not BOOL. That reads confusingly.
 */


/*
 * TODO:
 *
 *		o	WRITEUP THREAD SAFETY:
 *		 THREAD SAFETY:
 *			 Writeup in docs STRINGS THREADING SAFETY setioN (intenral hidden stuff fully threadsafe,
 *			 but externally, envelope cannot be read/write or write/write at the same time). – document examples.
 *
 *
	(0)	Add Ranged insert public envelope API, and add APPEND (not just operaotr+) API

	(0)	Try and get rid of the Peek () API
	(0)	Fix const	Memory::SharedByValue<String::String::Rep>	String::kEmptyStringRep_ (new String_CharArray::MyRep_ (nullptr, 0), &String::Clone_);
		to properly handle cross-module startup (not safe as is - probably use ModuleInit<> stuff. OR use static intit PTR and assure its fixed
			just in CPP file
	(0)	Move DOCS in the top of this file down to the appropriate major classes - and then review the implemantion and make sure
		it is all correct for each (especially SetStorage () sutff looks quesitonable)
	(1)	Use new CopyTo() method to get rid of MOST of the casts/memcpy code in the implementation


	(4)	Migrate most of the StringUtils stuff here like:
			> Contains- with CI optin
// overload so can be string arg OR lambda!
			> StartsWtih- with CI optin
			> EndsWith- with CI optin
			> Compare () - returns < less > more =0 for equal- with CI optin
			> Equals() - with CI optin
	(5)	Add Left()/Right()/Mid() funtions - like basic (simple, vaguely useful - especially 'Right'()).



	(6)	Compare
			template	<typename TCHAR>
				basic_string<TCHAR>	RTrim (const basic_string<TCHAR>& text)
					{
						std::locale loc1;	// default locale
						const ctype<TCHAR>& ct = use_facet<ctype<TCHAR> >(loc1);
						typename basic_string<TCHAR>::const_iterator i = text.end ();
						for (; i != text.begin () and ct.is (ctype<TCHAR>::space, *(i-1)); --i)
							;
						return basic_string<TCHAR> (text.begin (), i);
					}
			with the TRIM() implementation I wrote here - in String. Not sure we want to use the local stuff? Maybe?


MEDIUM TERM TODO (AFTER WE PORT MORE CONTAINER CLASSES):
	(o)		when we get Sequence<> ported (after) - we MUST add sequence-iterator to String class (will work beatifulyl with new stdc++ foreach() stuff).


	(o)		Redo implementation of String_StackLifetime - using high-performance algorithm described in the documentation.


	(o)		Optimize bool	Stroika::Foundation::Characters::operator== (const String& lhs, const wchar_t* rhs)
			by adding virtual ‘compare()’ method on rep – taking const wchar_t* arg. Current code 
			quite slow (but only conclude this/do this AFTER we have a regression test to PROVE my theories!)
	(o)		Do String_stdwstring() – as impl optimized to return std::wstring() a lot – saving that impl internally. 
			Do make this efficient, must have pur virtual method of String:::Rep which fills in a wstring* arg
			(what about ‘into no-malloc semantics – I guess taken care of perhaps by this? Maybe not… THINKOUT – 
			but pretty sure we want some sort of String_stdwstring(). 

	(o)		Consider optimizing Sting::operator== () to check if reps ptrs are equal. Reason maybe opt is cuz of String::Common
			stuff and special case of emptyu string. If it hits seldom enuf, just a wasted effort.
*/






namespace	Stroika {
	namespace	Foundation {
		namespace	Characters {

            const size_t    kBadStringIndex   = wstring::npos;


			/*
			 * The Stroika String class is an alternatve for the std::wstring class, which should be largely interoperable with
			 * code using wstring (there is wstring constructor and As<wstring>() methods).
			 *
			 * The Stroika String class is conceptually a sequence of (UNICODE) Characters, and so there is no obvious way
			 * to map the Stroika String to a std::string. However, if you specify a codepage for conversion, or are converting to/from
			 * TString/TChar, there is builtin support for that.
			 *
			 *
			 * EOS Handling:
			 *		The Stroika String class does support having embedded NUL-characters. It also supports returning wchar_t* strings
			 *		which are NUL-terminated. But - Stroika generally does NOT maintain strings internally as NUL-terminated (generally). It may add a performance
			 *		overhead when you call the c_str() method to force a NUL-character termination. See String::c_str ().
			 *
			 *		Also note that some subclasses of String (e.g. future String_stdwstring) may not support internal NUL-characters, if their underling
			 *		implementation doesn't allow for that.
			 *
			 */
            class	String {
                public:
					/*
					 * All the constructors are obvious, except to note that NUL-character ARE allowed in strings, except for the case of single char* argument
					 * constructors - which find the length based on the terminating NUL-character.
					 */
                    String ();
                    String (const char16_t* cString);
                    String (const wchar_t* cString);
                    String (const wchar_t* from, const wchar_t* to);
                    String (const Character* from, const Character* to);
					String (const std::wstring& r);
					String (const String& from);
                    ~String ();

                    nonvirtual	String&	operator= (const String& newString);

				public:
					static	String	FromUTF8 (const char* from);
					static	String	FromUTF8 (const std::string& from);
					static	String	FromTString (const TChar* from);
					static	String	FromTString (const TString& from);

                public:
                    nonvirtual	String&	operator+= (Character appendage);
                    nonvirtual	String&	operator+= (const String& appendage);

                public:
                    nonvirtual	size_t	GetLength () const;
					/*
					 * NOTE - when you increase the size of a string with SetLength() - the extra characters added are not initialized, and 
					 * will have random values.
					 */
                    nonvirtual	void	SetLength (size_t newLength);

                public:
					nonvirtual	bool	empty () const;
					nonvirtual	void	clear ();

                public:
                    nonvirtual	Character	operator[] (size_t i) const;
                    nonvirtual	void		SetCharAt (Character c, size_t i);

                public:
                    nonvirtual	void		InsertAt (Character c, size_t i);

                public:
					/*
					 * Remove the characters start at 'index' - removing nCharsToRemove (defaults to 1). It is an error if this implies removing
					 * characters off the end of the string.
					 */
                    nonvirtual	void		RemoveAt (size_t index, size_t nCharsToRemove = 1);
                
				public:
                    nonvirtual	void		Remove (Character c);

                public:
                    /*
                     * inherited from Sequence. Lookup the character (or string) in this string, and return
                     * its index - either starting from the front, or end of the string. Returns kBadStringIndex
                     * if none found.
                     */
                    nonvirtual	size_t	IndexOf (Character c) const;
					/*
					 * IndexOf (substring) returns the index of the first occurance of the given substring in this string. This function
					 * always returns a valid string index, which is followed by the given substring, or kBadStringIndex otherwise.
					 */
                    nonvirtual	size_t	IndexOf (const String& subString) const;


                    nonvirtual	size_t	RIndexOf (Character c) const;

					/*
					 * RIndexOf (substring) returns the index of the last occurance of the given substring in this string. This function
					 * always returns a valid string index, which is followed by the given substring, or kBadStringIndex otherwise.
					 */
                    nonvirtual	size_t	RIndexOf (const String& subString) const;

                    nonvirtual	bool	Contains (Character c) const;
                    nonvirtual	bool	Contains (const String& subString) const;

                public:
                    /*
                     * Produce a substring of this string, starting at from, and up to to (require from <= to unless to == kBadStingIndex). If to
                     * is kBadStringIndex (default) then return all the way to the end of the string.
                     */
                    nonvirtual	String		SubString (size_t from, size_t to = kBadStringIndex) const;

                public:
					/*
					 * Apply the given regular expression return true if it matches this string
					 */
					nonvirtual	bool	Match (const String& regEx) const;

                public:
					/*
					 * Apply the given regular expression, and return a vector of the starts of all substring matches.
					 */
					nonvirtual	vector<String>	Find (const String& regEx) const;

				public:
					/*
					 * Apply the given regular expression, with 'with' and replace each match. This doesn't modify this string, but returns the replacement string.
					 */
					nonvirtual	String	Replace (const String& regEx, const String& with) const;

				public:
					#if		!qCompilerAndStdLib_Supports_lambda_default_argument || !qCompilerAndStdLib_lamba_closureCvtToFunctionPtrSupported
					inline	static	bool	DefaultTrimArg_ (Character c)				{		return c.IsWhitespace ();		}
					#endif

					#if		qCompilerAndStdLib_Supports_lambda_default_argument && qCompilerAndStdLib_lamba_closureCvtToFunctionPtrSupported
					/*
					 * String LTrim () scans the characters form the left to right, and applies the given 'shouldBeTrimmed' function (defaults to IsWhitespace).
					 * all such characters are removed, and the resulting string is returned. This does not modify the current string its applied to - just
					 * returns the trimmed string.
					 */
					nonvirtual	String	LTrim (bool (*shouldBeTrimmmed) (Character) = [](const Character& c) -> bool { return c.IsWhitespace (); }) const;
					/*
					 * String RTrim () scans the characters form the right to left, and applies the given 'shouldBeTrimmed' function (defaults to IsWhitespace).
					 * all such characters are removed, and the resulting string is returned. This does not modify the current string its applied to - just
					 * returns the trimmed string.
					 */
					nonvirtual	String	RTrim (bool (*shouldBeTrimmmed) (Character) = [](Character c) -> bool { return c.IsWhitespace (); }) const;
					/*
					 * String Trim () is locally equivilent to RTrim (shouldBeTrimmed).LTrim (shouldBeTrimmed).
					 */
					nonvirtual	String	Trim (bool (*shouldBeTrimmmed) (Character) = [](Character c) -> bool { return c.IsWhitespace (); }) const;
					#else
					nonvirtual	String	LTrim (bool (*shouldBeTrimmmed) (Character) = DefaultTrimArg_) const;
					nonvirtual	String	RTrim (bool (*shouldBeTrimmmed) (Character) = DefaultTrimArg_) const;
					nonvirtual	String	Trim (bool (*shouldBeTrimmmed) (Character) = DefaultTrimArg_) const;
					#endif

				public:
					// Walk the entire string, and produce a new string consisting of all characters for which the predicate 'removeCharIf' returned false
					nonvirtual	String	StripAll (bool (*removeCharIf) (Character)) const;

				public:
					/*
					 * Return a new string based on this string where each lower case characer is replaced by its upper case equivilent.
					 */
					nonvirtual	String	ToLowerCase () const;

					/*
					 * Return a new string based on this string where each lower case characer is replaced by its upper case equivilent.
					 */
					nonvirtual	String	ToUpperCase () const;

					/*
					 * Return true if the string contains zero non-whitespace characters.
					 */
					nonvirtual	bool	IsWhitespace () const;


				public:
					/*
					 * 	CopyTo () copies the contents of this string to the target buffer.
					 *	CopyTo () does NOT nul-terminate the target buffer, but DOES assert that (bufTo-bufFrom) is >= this->GetLength ()
					 */
					nonvirtual	void	CopyTo (Character* bufFrom, Character* bufTo) const;
					nonvirtual	void	CopyTo (wchar_t* bufFrom, wchar_t* bufTo) const;


				public:
					/*
					 * Convert String losslessly into a standard C++ type (right now just <wstring>,<const wchar_t*> supported)
					 *
					 * For the special case of <T=const wchar_t*>, the returned result is NOT NUL-terminated.
					 */
					template	<typename	T>
						nonvirtual	T	As () const;
					template	<typename	T>
						nonvirtual	void	As (T* into) const;

				public:
					/*
					 * Convert String losslessly into a standard C++ type (right now just <string> supported). Note - template param is optional.
					 */
					template	<typename	T>
						nonvirtual	T	AsUTF8 () const;
					template	<typename	T>
						nonvirtual	void	AsUTF8 (T* into) const;
					nonvirtual	std::string	AsUTF8 () const;
					nonvirtual	void		AsUTF8 (std::string* into) const;

				public:
					/*
					 */
					nonvirtual	TString	AsTString () const;
					nonvirtual	void	AsTString (TString* into) const;

				public:
					/*
					 * Convert String losslessly into a standard C++ type (right now just <string> supported). The source string
					 * MUST be valid ascii characters (asserted)
					 */
					template	<typename	T>
						nonvirtual	T	AsASCII () const;
					template	<typename	T>
						nonvirtual	void	AsASCII (T* into) const;
					nonvirtual	std::string	AsASCII () const;
					nonvirtual	void		AsASCII (std::string* into) const;


				public:
					enum	CompareOptions {
						eWithCase_CO,
						eCaseInsensitive_CO,
					};
					// Return < 0 if *this < rhs, return 0 if equal, and return > 0 if *this > rhs.
					nonvirtual	int	Compare (const String& rhs, CompareOptions co) const;


				// StdC++ wstring aliases [there maybe a namespace trick in new c++ to do this without inlines - like new '=' guy???
				public:
					nonvirtual	size_t			size () const;
					nonvirtual	size_t			length () const;

					// As with STL, the return value of the data () function should NOT be assumed to be NUL-terminated
					//
					// The lifetime of the pointer returned is gauranteed until the next call to this String envelope class (that is if other reps change, or are
					// acceessed this data will not be modified)
					nonvirtual	const wchar_t*	data () const;

					// This will always return a value which is NUL-terminated. Note that Stroika generally does NOT keep strings in NUL-terminated form, so
					// this could be a costly function, requiring a copy of the data.
					//
					// The lifetime of the pointer returned is gauranteed until the next call to this String envelope class (that is if other reps change, or are
					// acceessed this data will not be modified)
					nonvirtual	const wchar_t*	c_str () const;
					
					// need more overloads
					nonvirtual	size_t find (wchar_t c) const;
					// need more overloads
					nonvirtual	size_t rfind (wchar_t c) const;


				protected:
                    class	_Rep;
					struct	_Rep_Cloner {
						inline	_Rep*	Copy (const _Rep& t) const
							{
								return String::Clone_ (t);
							}
					};

				protected:
					enum _REPCTOR { _eRepCTOR };
					// _Rep MUST be not-null
                    String (_Rep* sharedPart, _REPCTOR);

                protected:
                    Memory::SharedByValue<_Rep,_Rep_Cloner>	_fRep;

                protected:
                    static	_Rep*	Clone_ (const _Rep& rep);

				private:
					/*
					 * These are made friends so they can peek at the shared part, as an optimization/
					 */
					friend	bool	operator== (const String& lhs, const String& rhs);
					friend	bool	operator!= (const String& lhs, const String& rhs);
					friend	bool	operator< (const String& lhs, const String& rhs);
					friend	bool	operator<= (const String& lhs, const String& rhs);
					friend	bool	operator> (const String& lhs, const String& rhs);
					friend	bool	operator>= (const String& lhs, const String& rhs);

					// constructs a StringRep_Catenate
					friend	String	operator+ (const String& lhs, const String& rhs);
            };

			template	<>
				void	String::As (wstring* into) const;
			template	<>
				wstring	String::As () const;
			template	<>
				const wchar_t*	String::As () const;

			template	<>
				void	String::AsUTF8 (string* into) const;
			template	<>
				string	String::AsUTF8 () const;

			template	<>
				void	String::AsASCII (string* into) const;
			template	<>
				string	String::AsASCII () const;




			/*
			 * Protected helper Rep class.
			 */
			class	String::_Rep {
                protected:
                    _Rep ();

                public:
                    virtual	~_Rep ();

                    virtual	_Rep*	Clone () const						= 0;

                    virtual	size_t	GetLength () const 					= 0;
                    virtual	bool	Contains (Character item) const		= 0;
                    virtual	void	RemoveAll () 						= 0;

                    virtual	Character	GetAt (size_t index) const				= 0;
                    virtual	void		SetAt (Character item, size_t index)	= 0;
                    virtual	void		InsertAt (const Character* srcStart, const Character* srcEnd, size_t index)	= 0;
                    virtual	void		RemoveAt (size_t index, size_t nCharsToRemove)	= 0;

                    virtual	void	SetLength (size_t newLength) 				= 0;

					// return nullptr if its not already NUL-terminated
                    virtual	const wchar_t*		c_str_peek () const				= 0;
					
					// change rep so its NUL-termainted
                    virtual	const wchar_t*		c_str_change ()					= 0;

                    virtual	const Character*	Peek () const 					= 0;

					virtual	int		Compare (const _Rep& rhs, String::CompareOptions co) const	=	0;

				public:
					/*
					 * 	CopyTo () copies the contents of this string to the target buffer.
					 *	CopyTo () does NOT nul-terminate the target buffer, but DOES assert that (bufTo-bufFrom) is >= this->GetLength ()
					 */
					nonvirtual	void	CopyTo (Character* bufFrom, Character* bufTo) const;
					nonvirtual	void	CopyTo (wchar_t* bufFrom, wchar_t* bufTo) const;
            };


            String	operator+ (const String& lhs, const String& rhs);

            bool	operator== (const String& lhs, const String& rhs);
            bool	operator== (const wchar_t* lhs, const String& rhs);
            bool	operator== (const String& lhs, const wchar_t* rhs);
            bool	operator!= (const String& lhs, const String& rhs);
            bool	operator!= (const wchar_t* lhs, const String& rhs);
            bool	operator!= (const String& lhs, const wchar_t* rhs);
            bool	operator< (const String& lhs, const String& rhs);
            bool	operator< (const wchar_t* lhs, const String& rhs);
            bool	operator< (const String& lhs, const wchar_t* rhs);
            bool	operator<= (const String& lhs, const String& rhs);
            bool	operator<= (const wchar_t* lhs, const String& rhs);
            bool	operator<= (const String& lhs, const wchar_t* rhs);
            bool	operator> (const String& lhs, const String& rhs);
            bool	operator> (const wchar_t* lhs, const String& rhs);
            bool	operator> (const String& lhs, const wchar_t* rhs);
            bool	operator>= (const String& lhs, const String& rhs);
            bool	operator>= (const wchar_t* lhs, const String& rhs);
            bool	operator>= (const String& lhs, const wchar_t* rhs);





			/*
			 *	String_BufferedArray is a kind of string which maintains extra buffer space, and
			 *	is more efficient if you are going to resize your string.
			 */
            class	String_BufferedArray  : public String {
                public:
                    String_BufferedArray ();
                    String_BufferedArray (size_t reserve);
                    explicit String_BufferedArray (const wchar_t* cString);
                    explicit String_BufferedArray (const wchar_t* cString, size_t reserve);
                    explicit String_BufferedArray (const wstring& str);
                    explicit String_BufferedArray (const wstring& str, size_t reserve);
                    explicit String_BufferedArray (const String& from);
                    explicit String_BufferedArray (const String& from, size_t reserve);
                    String_BufferedArray (const String_BufferedArray& s);

                    String_BufferedArray& operator= (const String_BufferedArray& s);

				public:
					// This returns the number of characters of space available in the buffer (without doing memory allocations)
					nonvirtual	size_t	capacity () const;

				public:
					// Reserve the given number of characters of space. N must be >= to the length of the string.
					nonvirtual	void	reserve (size_t n);
            };







            /*
			 *	String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly is a subtype of string you can use to construct a String object, so long as the memory pointed to
			 * in the argument has a
			 *		o	FULL APPLICATION LIFETIME,
			 *		o	the member referenced never changes - is READONLY.
			 *
			 *	String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly will NOT change the memory referenced in the CTOR.
			 *
			 *	Strings constructed with this String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly maybe treated like normal strings - passed anywhere, and even modified via the
			 *	String APIs.
			 *
			 *	For example
			 *		String	tmp1	=	L"FRED";
			 *		String	tmp2	=	String (L"FRED");
			 *		String	tmp3	=	String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly (L"FRED");
			 *
			 *		extern String saved;
			 *		inline	String	F(String x)			{ saved = x; x.InsertAt ('X', 1); saved = x.ToUpperCase () + "fred";  return saved; }
			 *		F(tmp1);
			 *		F(tmp2);
			 *		F(tmp3);
			 *
			 *	These ALL do essentially the same thing, and are all equally safe. The 'tmp3' implementation maybe slightly more efficent, but all are equally safe.
			 *
			 */
            class	String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly : public String {
                public:
                    explicit String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly (const wchar_t* cString);
                    String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly (const String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly& s);

                    String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly& operator= (const String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly& s);

				private:
					class	MyRep_;
            };


			/*
			 *		String_Constant can safely be used to initilaize constant C-strings as Stroika strings, with a minimum of cost.
			 */
			typedef	String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly	String_Constant;





            /*
			 *	String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite is a subtype of string you can use to construct a String object, so long as the memory pointed to
			 * in the argument has a
			 *		o	FULL APPLICATION LIFETIME,
			 *		o	and never changes value through that pointer
			 *
			 *	Note that the memory passed in MUST BE MODIFIABLE (READ/WRITE) - as String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite MAY modify the data in the pointer during the
			 *	objects lifetime.
			 *
			 *	Strings constructed with this String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite maybe treated like normal strings - passed anywhere, and even modified via the
			 *	String APIs. However, the underlying implemenation may cache the argument const wchar_t* cString indefinitely, and re-use it as needed, so
			 *	only call this String constructor with a block of read-only, never changing memory, and then - only as a performance optimization.
			 *
			 *	For example
			 *		String	tmp1	=	L"FRED";
			 *		String	tmp2	=	String (L"FRED");
			 *		String	tmp3	=	String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite (L"FRED");
			 *
			 *		extern String saved;
			 *		inline	String	F(String x)			{ saved = x; x.InsertAt ('X', 1); saved = x.ToUpperCase () + "fred";  return saved; }
			 *		F(tmp1);
			 *		F(tmp2);
			 *		F(tmp3);
			 *
			 *	These ALL do essentially the same thing, and are all equally safe. The 'tmp3' implementation maybe slightly more efficent, but all are equally safe.
			 *
			 */
            class	String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite : public String {
                public:
                    explicit String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite (wchar_t* cString);
                    String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite (const String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite& s);

                    String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite& operator= (const String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite& s);

				private:
					class	MyRep_;
            };






            /*
			 *	String_ExternalMemoryOwnership_StackLifetime_ReadOnly is a subtype of string you can use to construct a String object, so long as the memory pointed to
			 * in the argument has a
			 *		o	Greater lifetime than the String_ExternalMemoryOwnership_StackLifetime_ReadOnly envelope class
			 *		o	and buffer data never changes value externally to this String represenation
			 *
			 *	Note that the memory passed in must be READ/WRITE - and may be modified by the String_ExternalMemoryOwnership_StackLifetime_ReadOnly ()!
			 *
			 *	Strings constructed with this String_ExternalMemoryOwnership_StackLifetime_ReadOnly maybe treated like normal strings - passed anywhere, and even modified via the
			 *	String APIs. However, the underlying implemenation may cache the argument const wchar_t* cString for as long as the lifetime of the envelope class,
			 *	and re-use it as needed during this time, so only call this String constructor with great care, and then - only as a performance optimization.
			 *
			 *	This particular form of String wrapper CAN be a great performance optimization when a C-string buffer is presented and one must
			 *	call a 'String' based API. The argument C-string will be used to handle all the Stroika-String operations, and never modified, and the
			 *	association will be broken when the String_ExternalMemoryOwnership_StackLifetime_ReadOnly goes out of scope.
			 *
			 *	This means its EVEN safe to use in cases where the String object might get assigned to long-lived String variables (the internal data will be
			 *	copied in that case).
			 *
			 *	For example
			 *
			 *		extern String saved;
			 *		inline	String	F(String x)			{ saved = x; x.InsertAt ('X', 1); saved = x.ToUpperCase () + "fred";  return saved; }
			 *
			 *
			 *		void f (const wchar_t* cs)
			 *			{
			 *				F(L"FRED";);
			 *				F(String (L"FRED"));
			 *				F(String_ExternalMemoryOwnership_StackLifetime_ReadOnly (cs));
			 *			}
			 *
			 *	These ALL do essentially the same thing, and are all equally safe. The third call to F () with String_ExternalMemoryOwnership_StackLifetime_ReadOnly() based memory maybe more efficient than the
			 *	previous two, because the string pointed to be 'cs' never needs to be copied (now malloc/copy needed).
			 *
			 *		<<TODO: not sure we have all the CTOR/op= stuff done correctly for this class - must rethink - but only needed to rethink when we do
			 *			real optimized implemenation >>
			 */
            class	String_ExternalMemoryOwnership_StackLifetime_ReadOnly : public String {
                public:
                    explicit String_ExternalMemoryOwnership_StackLifetime_ReadOnly (const wchar_t* cString);
// DOCUMENT THESE NEW EXTRA CTORS!!! NYI
                    explicit String_ExternalMemoryOwnership_StackLifetime_ReadOnly (const wchar_t* start, const wchar_t* end);
            };




            /*
			 *	String_ExternalMemoryOwnership_StackLifetime_ReadWrite is a subtype of string you can use to construct a String object, so long as the memory pointed to
			 * in the argument has a
			 *		o	Greater lifetime than the String_ExternalMemoryOwnership_StackLifetime_ReadWrite envelope class
			 *		o	and buffer data never changes value externally to this String represenation
			 *
			 *	Note that the memory passed in must be READ/WRITE - and may be modified by the String_ExternalMemoryOwnership_StackLifetime_ReadWrite ()!
			 *
			 *	Strings constructed with this String_ExternalMemoryOwnership_StackLifetime_ReadWrite maybe treated like normal strings - passed anywhere, and even modified via the
			 *	String APIs. However, the underlying implemenation may cache the argument const wchar_t* cString for as long as the lifetime of the envelope class,
			 *	and re-use it as needed during this time, so only call this String constructor with great care, and then - only as a performance optimization.
			 *
			 *	This particular form of String wrapper CAN be a great performance optimization when a C-string buffer is presented and one must
			 *	call a 'String' based API. The argument C-string will be used to handle all the Stroika-String operations, and never modified, and the
			 *	association will be broken when the String_ExternalMemoryOwnership_StackLifetime_ReadWrite goes out of scope.
			 *
			 *	This means its EVEN safe to use in cases where the String object might get assigned to long-lived String variables (the internal data will be
			 *	copied in that case).
			 *
			 *	For example
			 *
			 *		extern String saved;
			 *		inline	String	F(String x)			{ saved = x; x.InsertAt ('X', 1); saved = x.ToUpperCase () + "fred";  return saved; }
			 *
			 *
			 *		void f (const wchar_t* cs)
			 *			{
			 *				F(L"FRED";);
			 *				F(String (L"FRED"));
			 *				F(String_ExternalMemoryOwnership_StackLifetime_ReadWrite (cs));
			 *			}
			 *
			 *	These ALL do essentially the same thing, and are all equally safe. The third call to F () with String_ExternalMemoryOwnership_StackLifetime_ReadWrite() based memory maybe more efficient than the
			 *	previous two, because the string pointed to be 'cs' never needs to be copied (now malloc/copy needed).
			 *
			 *		<<TODO: not sure we have all the CTOR/op= stuff done correctly for this class - must rethink - but only needed to rethink when we do
			 *			real optimized implemenation >>
			 */
            class	String_ExternalMemoryOwnership_StackLifetime_ReadWrite : public String {
                public:
                    explicit String_ExternalMemoryOwnership_StackLifetime_ReadWrite (wchar_t* cString);
// DOCUMENT THESE NEW EXTRA CTORS!!! NYI
                    explicit String_ExternalMemoryOwnership_StackLifetime_ReadWrite (wchar_t* start, wchar_t* end);
//TODO: start/end defines range of string, and bufend if 'extra bytes' usable after end - enen though not orignally part of string
                    explicit String_ExternalMemoryOwnership_StackLifetime_ReadWrite (wchar_t* start, wchar_t* end, wchar_t* bufEnd);
            };


			#if		0
            /*
			 *
			 *	NOT YET IMPLEMETNED - EVEN IN FAKE FORM - BECAUSE I"M NOT SURE OF SEMANTICS YET!
			 *
			 *	String_Common is a subtype of string you can use to construct a String object freely. It has no semantics requirements. However, it SHOULD only
			 *	be used for strings which are commonly used, and where you wish to save space. The implementation will keep the memory for String_Common strings
			 *	allocated permanently - for the lifetime of the application, and will take potentially extra time looking for the given string.
			 *
			 *	We MAY handle this like the HealthFrame RFLLib ATOM class - where we store the string in a hashtable (or map), and do quick lookup of associated index, and
			 *	also store in a table (intead of vector of strings, use a big buffer we APPEND to, and whose index is the value of the stored rep. Then doing a PEEK()
			 *	is trivial and efficient.
			 *
			 *		<<TODO: OPTIMIZATION not really implemented yet. But it can still freeely be used safely.>>
			 */
            class	String_Common : public String {
                public:
                    explicit String_Common (const String& from);
            };
			#endif


			#if		0
            /*
			 *
			 *	NOT YET IMPLEMETNED
			 *
			 *	String_stdwstring is completely compatible with any other String implementation, except that it represents things
			 *	internally using the stdC++ wstring class. The principle advantage of this is that converting TO wstrings
			 *	is much more efficient.
			 *
			 *		(AS OF YET UNCLEAR IF/HOW WE CAN SUPPORT MANIPULATIONS OF A wstring* or wstring& alias to the String rep's owned copy.
			 *		probably won't be allowed, but it would be helpful to some applicaitons if we could)
			 */
            class	String_stdwstring : public String {
                public:
                    explicit String_stdwstring (const String& from);
            };
			#endif



			/*
			 * Legacy "C_String" string utilities.
			 */
			namespace	C_String	{

				// Only implemented for char/wchar_t. Reason for this is so code using old-style C++ strings can leverage overloading!
				template	<typename T>
					size_t	Length (const T* p);
				template	<>
					size_t	Length (const char* p);
				template	<>
					size_t	Length (const wchar_t* p);


			}

		}
	}
}



#endif	/*_Stroika_Foundation_Characters_String_h_*/


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"String.inl"


