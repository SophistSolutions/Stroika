/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Characters_String_h_
#define	_Stroika_Foundation_Characters_String_h_	1

/*
 *
 * Description:

 <<<<REVIEW / UPDATE DOCS FOR NEW STROIKA>>>
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
 *		The class that String's own is a reference counted pointer to StringRep.
 *		This class is abstract, and defines a protocol for getting at the string data
 *		(Character*) and for getting/setting the string length. All the rest of the
 *		String class is built in this interface, and subclasses of StringRep only
 *		needed follow that prootcol to be used interchangably with the rest of Strings.
 *
 *		Although you generally don't need to know about the various subclasses of
 *		StringRep implemented here, we give a brief overview anyhow.
 *
 *		StringRep_BufferedCharArray is a subclass of StringRep designed to minimized
 *		memory usage. It uses Realloc () to keep resizing the buffer it has allocated,
 *		and whatever argument you give IT in construction, is copied into memory it
 *		allocates, and frees. The buffer is always exactly the length of the string,
 *		which minimized memory usage, but may slow down doing many operations that
 *		change the length of the String.
 *
 *		StringRep_CharArray uses buffering to make dynamically sizing String
 *		operations faster, at some cost in memory. It keeps a buffer that is at
 *		least as long as the String, but is often somewhat longer.
 *
 *		String_ReadOnlyChar::MyRep_ is a subclass of StringRep which uses the memory
 *		passed in and saves the pointer, and does not delete it later. This can be a
 *		big memory efficiency and save useless memory waste and fragmentation. It
 *		is mainly useful for constant C strings.
 *
 *		StringRep_Substring is a subclass of StringRep useful in the
 *		implementation of SubString operations. It allows for the sharing of memory
 *		of the original StringRep (whatever its representation) and still have
 *		the concept of a separate string.
 *		This can be a very big efficiency win when applying substring operations
 *		in a fashion that does not then modify the substrings. Then we get away with
 *		being lazy, in our lazy copy, and never really have to.
 *
 *		There are a whole raft of other potentially useful StringReps. For example,
 *		a StringRep that hooks into a backend database, or into the system
 *		TextEdit's implementations. A StringRep could compress its text to save
 *		space. A possibility used in GCC is to build a buffer that points to two
 *		other buffers as a way to implement operator+ in String efficiently. (NOTE:
 *		some of the above would require adding more interface to StringRep,
 *		in particular to make GetChar be a virtual method. This will probably be
 *		done, but it remains controversial due to the intrinsic overhead of
 *		virtual function invocations).
 *
 *		Although you can create a String from any one of these StringReps
 *		directly, it is far more common to do so via a convience interface. The
 *		constructor for String is overloaded to take an enum AllocMode argument
 *		which specifies how to interpret the rest of the arguments to the
 *		constructor (really those args are just passed on directly to the
 *		appropriate subclass of StringRep mentioned above by conditioned on
 *		the enum argument).
 *
 *		These AllocModes are:
 *			eBuffered:				which means use StringRep_CharArray.
 *			eFixedSize:				which means use StringRep_BufferedCharArray
 *			eReadOnly:				which means use String_ReadOnlyChar::MyRep_
 *
 *		At the risk of being redundent. Most of this need be of no concern. If
 *		you ignore the allocMode stuff and the StringRep stuff, it will all
 *		be handled for you with nearly optimal performance. One of the above
 *		AllocModes will be used (we dont document which)
 *


 *** NOTES TO WRITEUP:
 THREAD SAFETY:
	 Writeup in docs STRINGS THREADING SAFETY setioN (intenral hidden stuff fully threadsafe,
	 but externally, envelope cannot be read/write or write/write at the same time). – document examples.



 **
 */

#include	"../StroikaPreComp.h"

#include	"../Memory/CopyOnWrite.h"

#include	"Character.h"






#if		0
SHORT TERM THINGS TODO:

	(0)	Move DOCS in the top of this file down to the appropriate major classes - and then review the implemantion and make sure
		it is all correct for each (especially SetStorage () sutff looks quesitonable)
	(1)	Use new CopyTo() method to get rid of MOST of the casts/memcpy code in the implementation
	(2)	Implement the UTF8 functions
	(3)	Fix / Re-impemlement the CopyOnWrite() code to not store a POINTER to clone function. Best option is to use FUNCTOR
		whcih is stored in CopyOnWrite object - IF the compiler properly handles zero-sized object (taking no space). Otherwise use 
		some kind of overload so there is a no-cost way todo the most common case of a static (per type) Clone function.
	(4)	Migrate most of the StringUtils stuff here like:
			> Contains- with CI optin
// overload so can be string arg OR lambda!
			> StartsWtih- with CI optin
			> EndsWith- with CI optin
			> Compare () - returns < less > more =0 for equal- with CI optin
			> Equals() - with CI optin
	(5)	Add Left()/Right()/Mid() funtions - like basic (simple, vaguely useful - especially 'Right'()).



MEDIUM TERM TODO (AFTER WE PORT MORE CONTAINER CLASSES):
	(1)	when we get Sequence<> ported (after) - we MUST add sequence-iterator to String class (will work beatifulyl with new stdc++ foreach() stuff).

#endif




namespace	Stroika {
	namespace	Foundation {
		namespace	Characters {

            const size_t    kBadStringIndex   = string::npos;

            class	String {
                public:
                    String ();
                    String (const wchar_t* cString);
					String (const std::wstring& r);
					String (const String& from);
                    ~String ();

                    nonvirtual	String&	operator= (const String& newString);

				public:
					static	String	FromUTF8 (const char* from);
					static	String	FromUTF8 (const std::string& from);

                public:
                    nonvirtual	String&	operator+= (Character appendage);
                    nonvirtual	String&	operator+= (const String& appendage);

                public:
                    nonvirtual	size_t	GetLength () const;
                    nonvirtual	void	SetLength (size_t newLength);

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
                     * Produce a substring of this string, starting at from, and extending length characters. If length
                     * is kBadStringIndex (default) then return all the way to the end of the string.
                     */
                    nonvirtual	String		SubString (size_t from, size_t length = kBadStringIndex) const;


//unimplemented draft API
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
                    /*
                     * Peeking is possible, but ill-advised since it is not wholly transparent when that internal
                     * pointer might become invalid. Generally, if you don't call any routines of String (even
                     * indirectly) you should be allright.
                     */
                    nonvirtual	const Character* Peek () const;


				public:
					/*
					 * 	CopyTo () copies the contents of this string to the target buffer.
					 *	It always nul-terminates, and asserts buffer is large enuf.
					 *	
				<<<< USE THIS IN PREFERENCE TO PEEK- EXCEPT WHERE PERFORMANCE DICTATES OTHERWISE >>>
					 */
					nonvirtual	void	CopyTo (Character* buf, size_t nCharsInBuf);
					nonvirtual	void	CopyTo (wchar_t* buf, size_t nCharsInBuf);


				public:
					/*
					 * Convert String losslessly into a standard C++ type (right now just <wstring>,<const wchar_t*> supported)
					 */
					template	<typename	T>
						nonvirtual	T	As () const;
					template	<typename	T>
						nonvirtual	void	As (T* into) const;

					/*
					 * Convert String losslessly into a standard C++ type (right now just <string> supported)
					 */
					template	<typename	T>
						nonvirtual	T	AsUTF8 () const;
					template	<typename	T>
						nonvirtual	void	AsUTF8 (T* into) const;

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
                    class	StringRep;

				protected:
					// StringRep MUST be not-null
                    String (StringRep* sharedPart, bool ignored);	// bool arg to disamiguate constructors

                private:
                    Memory::CopyOnWrite<StringRep>	fRep;

                    static	StringRep*	Clone_ (const StringRep& rep);

				private:
					static	const	Memory::CopyOnWrite<String::StringRep>	kEmptyStringRep_;

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




            class	String::StringRep {
                protected:
                    StringRep ();

                public:
                    virtual	~StringRep ();

                    virtual	StringRep*	Clone () const		= 0;

                    virtual	size_t	GetLength () const 					= 0;
                    virtual	bool	Contains (Character item) const		= 0;
                    virtual	void	RemoveAll () 						= 0;

                    virtual	Character	GetAt (size_t index) const				= 0;
                    virtual	void		SetAt (Character item, size_t index)	= 0;
                    virtual	void		InsertAt (Character item, size_t index)	= 0;
                    virtual	void		RemoveAt (size_t index, size_t amountToRemove)	= 0;

                    virtual	void	SetLength (size_t newLength) 	= 0;

                    virtual	const Character*	Peek () const 				= 0;
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


#if 0
			// DISCUSS WITH STERL IF we should supprot this sort of thing, and if so - where. Its really cruddy stream support (structured or not!)
            /*
             * Stream inserters and extractors.
             */
// PROBABLY EITHER GET RID OF THESE - OR use namespace std {typedef ostream} and then ??? somehow avoid forcing #include of <iostream> class in string code
            ostream&	operator<< (ostream& out, const String& s);
            wistream&	operator>> (wistream& in, String& s);
#endif


            /*
             * Case conversion.

// REDO AS MEMBER FUNCTIONS!!!
             */
            String	toupper (const String& s);
            String	tolower (const String& s);



			/*
             * EXPLAIN WHAT THIS MEANS - DEFINE EXEACT SEMANTICS CAREFULLY
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
             * EXPLAIN WHAT THIS MEANS - DEFINE EXEACT SEMANTICS CAREFULLY
             */
            class	String_ReadOnlyChar : public String {
                public:
                    explicit String_ReadOnlyChar (const wchar_t* cString);
                    String_ReadOnlyChar (const String_ReadOnlyChar& s);

                    String_ReadOnlyChar& operator= (const String_ReadOnlyChar& s);

				private:
					class	MyRep_;
            };




		}
	}
}




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"String.inl"



#endif	/*_Stroika_Foundation_Characters_String_h_*/
