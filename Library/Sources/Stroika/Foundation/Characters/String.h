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
 *				o	String_ExternalMemoryOwnership
 *				o	String_StackLifetime
 *				o	String_Common
 */

#include	"../StroikaPreComp.h"

#include	"../Memory/SharedByValue.h"
#include	"TString.h"

#include	"Character.h"




/*
 * TODO:
 *
 *		o	REWRITE DOCS ABOUT
 *		o	WRITEUP THREAD SAFETY:
 *		 THREAD SAFETY:
 *			 Writeup in docs STRINGS THREADING SAFETY setioN (intenral hidden stuff fully threadsafe,
 *			 but externally, envelope cannot be read/write or write/write at the same time). – document examples.
 *
 *		o	WRITEUP NUL-TERMIANTED STINGS:
 *			Stroika String classes do NOT - in general - NUL-terminate strings. Most implementations - in fact - will support
 *			intenral NUL characters, but not all implementations, and so you shoudln't count on this behavior.
 *
 *			Stroika Strings make it easy to be created from NUL-terminated strings (String has such a constructor), and make it
 *			easy to create NUL-terminated strings (String::As<wstring> ().c_str ()). Note - however - this can be inefficnet
 *			if the strings are large, or this conversion must be done alot. For that situation, use String_stdwstring.
 *
 *			Among the reasons to NOT promise NUL-terminated strings internally stored inside String classes are:
 *				(1)	Would cost extra byte (very minor)
 *				(2)	Would prevent 'Substring::Rep' style optimized classes, or String_Catentation::Rep either.
 *				(3)	We might wnat to do a String::Rep that stores stirngs as UTF8, and that wouldn't fit well with
 *					constraints on the represnetation of internal sequence of chars being wchar_t* with NUL-term byte. In general
 *					we want to HIDE the representation chocies as mcuh as practical.
 *				(4)	MIGHT possible cause 'lifetime of pointer' issues. Maybe not. But I'd need to think through carefully the implications - especailly
 *					with multithreading etc.
 *
 *
 *
 *
	(0)	Document if we store NUL-terminated strings internally or not. If not - how do we implement the stdC++ c_str () API safely? Perhaps temporarily store the extra char* in teh ENVOLOPE to be freed
		when the envelope next changes gets copied/etc? Sucky). Do we really need that API?
		Maybe return PROXY OBJECT which has operator char* in iT! THAT maybe safest thing!!!!
		ANMYHOW - RIGHT NOW - As<const wchar_t*> () works - but it will be confusing to people cuz NOT nul-termianted!

	(0)	Add Ranged insert public envelope API, and add APPEND (not just operaotr+) API

	(0)	Try and get rid of the Peek () API
	(0)	Fix const	Memory::SharedByValue<String::String::Rep>	String::kEmptyStringRep_ (new String_CharArray::MyRep_ (nullptr, 0), &String::Clone_);
		to properly handle cross-module startup (not safe as is - probably use ModuleInit<> stuff. OR use static intit PTR and assure its fixed
			just in CPP file
	(0)	Move DOCS in the top of this file down to the appropriate major classes - and then review the implemantion and make sure
		it is all correct for each (especially SetStorage () sutff looks quesitonable)
	(1)	Use new CopyTo() method to get rid of MOST of the casts/memcpy code in the implementation
	(2)	Implement the UTF8 functions
	(3)	Fix / Re-impemlement the SharedByValue() code to not store a POINTER to clone function. Best option is to use FUNCTOR
		whcih is stored in SharedByValue object - IF the compiler properly handles zero-sized object (taking no space). Otherwise use
		some kind of overload so there is a no-cost way todo the most common case of a static (per type) Clone function.
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

            const size_t    kBadStringIndex   = string::npos;

            class	String {
                public:
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
                    nonvirtual	void	SetLength (size_t newLength);

                public:
					nonvirtual	bool	empty () const;
					nonvirtual	void	clear ();

                public:
                    nonvirtual	Character	operator[] (size_t i) const;
                    nonvirtual	void		SetCharAt (Character c, size_t i);

                public:
                    nonvirtual	void		InsertAt (Character c, size_t i);
                    nonvirtual	void		RemoveAt (size_t i);
                    nonvirtual	void		RemoveAt (size_t i, size_t amountToRemove);
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

#if 0
				public:
                    /*
                     * Peeking is possible, but ill-advised since it is not wholly transparent when that internal
                     * pointer might become invalid. Generally, if you don't call any routines of String (even
                     * indirectly) you should be allright.
					 *
					 * FROM CODE REVIEW - IT APPEARS PEEK MUST NEVER RETURN NULL (except maybe if length = 0), and lifetime is (unclear but short)
					 * and returns NON-NUL-TERMINATED Character array. But no matter the rep, it MUST always be able to return a Character* array (so not good design constraint)
					 * We should PROBABYL try to find a way to change that or get rid of this API
					 *			--LGP 2011-09-09
                     */
                    nonvirtual	const Character* Peek () const;
#endif


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
					 */
					template	<typename	T>
						nonvirtual	T	As () const;
					template	<typename	T>
						nonvirtual	void	As (T* into) const;

				public:
					/*
					 * Convert String losslessly into a standard C++ type (right now just <string> supported)
					 */
					template	<typename	T>
						nonvirtual	T	AsUTF8 () const;
					template	<typename	T>
						nonvirtual	void	AsUTF8 (T* into) const;

				public:
					/*
					 */
					nonvirtual	TString	AsTString () const;
					nonvirtual	void	AsTString (TString* into) const;

				public:
					/*
					 * Convert String losslessly into a standard C++ type (right now just <string> supported). The source string MUST be valid ascii characters (asserted)
					 */
					template	<typename	T>
						nonvirtual	T	AsASCII () const;
					template	<typename	T>
						nonvirtual	void	AsASCII (T* into) const;



				// StdC++ wstring aliases [there maybe a namespace trick in new c++ to do this without inlines - like new '=' guy???
				public:
					nonvirtual	size_t			size () const;
					nonvirtual	size_t			length () const;
					nonvirtual	const wchar_t*	data () const;
					nonvirtual	const wchar_t*	c_str () const;
					// need more overloads
					nonvirtual	size_t find (wchar_t c) const;
					// need more overloads
					nonvirtual	size_t rfind (wchar_t c) const;


				protected:
                    class	_Rep;

				protected:
					// _Rep MUST be not-null
                    String (_Rep* sharedPart, bool ignored);	// bool arg to disamiguate constructors

                private:
                    Memory::SharedByValue<_Rep>	fRep_;

                    static	_Rep*	Clone_ (const _Rep& rep);

				// Try to get rid of _PeekRep () Or at least make protected
				protected:
				public:
					nonvirtual	Memory::SharedByValue<_Rep>	_PeekRep () const;

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




            class	String::_Rep {
                protected:
                    _Rep ();

                public:
                    virtual	~_Rep ();

                    virtual	_Rep*	Clone () const		= 0;

                    virtual	size_t	GetLength () const 					= 0;
                    virtual	bool	Contains (Character item) const		= 0;
                    virtual	void	RemoveAll () 						= 0;

                    virtual	Character	GetAt (size_t index) const				= 0;
                    virtual	void		SetAt (Character item, size_t index)	= 0;
                    virtual	void		InsertAt (const Character* srcStart, const Character* srcEnd, size_t index)	= 0;
                    virtual	void		RemoveAt (size_t index, size_t amountToRemove)	= 0;

                    virtual	void	SetLength (size_t newLength) 	= 0;

                    virtual	const Character*	Peek () const 				= 0;

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
             * EXPLAIN WHAT THIS MEANS - DEFINE EXEACT SEMANTICS CAREFULLY
 *<<<NOT SURE THE BELOW IS RIGHT>>>
 *
 *		StringRep_CharArray uses buffering to make dynamically sizing String
 *		operations faster, at some cost in memory. It keeps a buffer that is at
 *		least as long as the String, but is often somewhat longer.
			 *
			 *
			 * TODO:
			 *		Consider possibly adding a const wchar_t* Peek () const method to this type. We don't want that for String in general,
			 *		but its probably OK here (if we carefully document the thread / lifetime issues)
             */
            class	String_CharArray : public String {
                public:
                    String_CharArray ();
                    explicit String_CharArray (const wchar_t* str);
                    explicit String_CharArray (const Character* arrayOfCharacters, size_t nCharacters);
                    explicit String_CharArray (const wstring& str);
                    explicit String_CharArray (const String& from);
                    String_CharArray (const String_CharArray& s);

                   String_CharArray& operator= (const String_CharArray& s);


				public:
					class	MyRep_;
            };



			/*
             * EXPLAIN WHAT THIS MEANS - DEFINE EXEACT SEMANTICS CAREFULLY
 *<<<NOT SURE THE BELOW IS RIGHT>>>
 *		StringRep_BufferedCharArray is a subclass of StringRep designed to minimized
 *		memory usage. It uses Realloc () to keep resizing the buffer it has allocated,
 *		and whatever argument you give IT in construction, is copied into memory it
 *		allocates, and frees. The buffer is always exactly the length of the string,
 *		which minimized memory usage, but may slow down doing many operations that
 *		change the length of the String.
             */
            class	String_BufferedCharArray  : public String {
                public:
                    String_BufferedCharArray ();
                    explicit String_BufferedCharArray (const wchar_t* cString);
                    explicit String_BufferedCharArray (const wstring& str);
                    explicit String_BufferedCharArray (const String& from);
                    String_BufferedCharArray (const String_BufferedCharArray& s);

                    String_BufferedCharArray& operator= (const String_BufferedCharArray& s);

				private:
					class	MyRep_;
            };




            /*
			 *	String_ExternalMemoryOwnership is a subtype of string you can use to construct a String object, so long as the memory pointed to
			 * in the argument has a
			 *		o	FULL APPLICATION LIFETIME,
			 *		o	and never changes value
			 *
			 *	Strings constructed with this String_ExternalMemoryOwnership maybe treated like normal strings - passed anywhere, and even modified via the
			 *	String APIs. However, the underlying implemenation may cache the argument const wchar_t* cString indefinitely, and re-use it as needed, so
			 *	only call this String constructor with a block of read-only, never changeing memory, and then - only as a performance optimization.
			 *
			 *	For example
			 *		String	tmp1	=	L"FRED";
			 *		String	tmp2	=	String (L"FRED");
			 *		String	tmp3	=	String_ExternalMemoryOwnership (L"FRED");
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
            class	String_ExternalMemoryOwnership : public String {
                public:
                    explicit String_ExternalMemoryOwnership (const wchar_t* cString);
                    String_ExternalMemoryOwnership (const String_ExternalMemoryOwnership& s);

                    String_ExternalMemoryOwnership& operator= (const String_ExternalMemoryOwnership& s);

				private:
					class	MyRep_;
            };



            /*
			 *	String_StackLifetime is a subtype of string you can use to construct a String object, so long as the memory pointed to
			 * in the argument has a
			 *		o	Greater lifetime than the String_StackLifetime envelope class
			 *		o	and buffer data never changes value
			 *
			 *	Strings constructed with this String_StackLifetime maybe treated like normal strings - passed anywhere, and even modified via the
			 *	String APIs. However, the underlying implemenation may cache the argument const wchar_t* cString for as long as the lifetime of the envelope class,
			 *	and re-use it as needed during this time, so only call this String constructor with great care, and then - only as a performance optimization.
			 *
			 *	This particular form of String wrapper CAN be a great performance optimization when a C-string buffer is presented and one must
			 *	call a 'String' based API. The argument C-string will be used to handle all the Stroika-String operations, and never modified, and the
			 *	association will be broken when the String_StackLifetime goes out of scope.
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
			 *				F(String_StackLifetime (cs));
			 *			}
			 *
			 *	These ALL do essentially the same thing, and are all equally safe. The third call to F () with String_StackLifetime() based memory maybe more efficient than the
			 *	previous two, because the string pointed to be 'cs' never needs to be copied (now malloc/copy needed).
			 *
			 *		<<TODO: not sure we have all the CTOR/op= stuff done correctly for this class - must rethink - but only needed to rethink when we do
			 *			real optimized implemenation >>
			 */
            class	String_StackLifetime : public String {
                public:
                    explicit String_StackLifetime (const wchar_t* cString);
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


