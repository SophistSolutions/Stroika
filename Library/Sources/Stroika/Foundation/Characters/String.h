/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__String__
#define	__String__

/*
 *
 * Description:
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
 *		StringRep_ReadOnlyChar is a subclass of StringRep which uses the memory
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
 *			eReadOnly:				which means use StringRep_ReadOnlyChar
 *
 *		At the risk of being redundent. Most of this need be of no concern. If
 *		you ignore the allocMode stuff and the StringRep stuff, it will all
 *		be handled for you with nearly optimal performance. One of the above
 *		AllocModes will be used (we dont document which)
 *
 *
 *
 *		<< USE TEMPLATES TO DO THIS>>
 *		Not too sure how important this is, but I think that there is no
 *		assumption that Character==char.  It could probably be used for
 *		multi-byte characters.  Though, practically, it would need to be
 *		parameterized, since most of the time when you have multibyte
 *		characters, you need single ones too?
 **
 */

#include	"../StroikaPreComp.h"

#include <ostream>

#include	"Character.h"

#include	"../Memory/Shared.h"

using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Characters;



namespace	Stroika {
	namespace	Foundation {
		namespace	Characters {

            const size_t    kBadStringIndex   = -1;

            class	String {
                public:
                    String ();
                    String (const char* cString);
                    String (const void* arrayOfBytes, size_t nBytes);	// if arrayOfBytes 0 then not copied from, and initial data in string undefined
                    String (const String& from);

                    ~String ()	{}		// NOTE: NON-VIRTUAL!!!!!

                    nonvirtual	String&	operator= (const String& newString);

                    nonvirtual	String&	operator+= (Character appendage);
                    nonvirtual	String&	operator+= (const String& appendage);

                    nonvirtual	size_t	GetLength () const;
                    nonvirtual	void	SetLength (size_t newLength);

                    nonvirtual	Character	operator[] (size_t i) const;
                    nonvirtual	void		SetCharAt (Character c, size_t i);

                    nonvirtual	void		InsertAt (Character c, size_t i);
                    nonvirtual	void		RemoveAt (size_t i);
                    nonvirtual	void		RemoveAt (size_t i, size_t amountToRemove);
                    nonvirtual	void		Remove (Character c);

                    /*
                     * inherited from Sequence. Lookup the character (or string) in this string, and return
                     * its index - either starting from the front, or end of the string. Returns kBadStringIndex
                     * if none found.
                     */
                    nonvirtual	size_t	IndexOf (Character c) const;
                    nonvirtual	size_t	IndexOf (const String& subString) const;
                    nonvirtual	size_t	RIndexOf (Character c) const;
                    nonvirtual	size_t	RIndexOf (const String& subString) const;
                    nonvirtual	bool	Contains (Character c) const;
                    nonvirtual	bool	Contains (const String& subString) const;

                    /*
                     * Produce a substring of this string, starting at from, and extending length characters. If length
                     * is kBadStringIndex (default) then return all the way to the end of the string.
                     */
                    nonvirtual	String		SubString (size_t from, size_t length = kBadStringIndex) const;

                    /*
                     * Peeking is possible, but ill-advised since it is not wholly transparent when that internal
                     * pointer might become invalid. Generally, if you don't call any routines of String (even
                     * indirectly) you should be allright.
                     */
                    nonvirtual	const void* Peek () const;

                    /*
                     * Some useful conversion routines to C/Pascal strings. Note that
                     * if you specify a buffer, we fill in no more than that amount, and
                     * otherwise we alloc a string for you, and you must then dispose of
                     * it when you are thru (with delete).
                     *
                     * Also, note that these return char*, rather than Character*. The reason
                     * for this is that even for multi-byte characters, you may want to get back
                     * a 'char*' representaiotn for talking to some other library routine.
                     * The actuall length will be big enuf for the whole wide string (reword).
                     *
                     * Note: if the buffer size is too small for the given string, we assert-out, rather
                     * than silently truncating. This may be less convientient in some cases, but I cannot stand
                     * things like magic truncation, and silent failures that are hard to track down. An important
                     * feature of Stroika is that we try not to quietly "do the right thing" but detect logic/
                     * programming errors as soon as possible, and notify the programmer.
                     *
                     * As it is often convientent, and sometimes safe and proper to do truncation, we also
                     * provide routines to do that. We choose to make the safer choice (the one with assertions)
                     * the default, and force you to explicitly embed the word Trunc in the conversion to
                     * get it to happen. This is not much extra typing, and I think its important that
                     * there is at least a subliminal hint that you really meant to do truncation.
                     */
                    nonvirtual	char*	ToCString () const;
                    nonvirtual	char*	ToCString (char buf [], size_t bufferSize) const;
                    nonvirtual	char*	ToCStringTrunc (char buf [], size_t bufferSize) const;

                    class	StringRep {
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

                            virtual	const void*	Peek () const 				= 0;
                    };

                protected:
                    String (StringRep* sharedPart, bool ignored);	// bool arg to disamiguate constructors

                    nonvirtual	void	SetRep (StringRep* rep);

                    nonvirtual	const StringRep*	GetRep () const;
                    nonvirtual	StringRep*			GetRep ();

                private:
                    Memory::Shared<StringRep>	fRep;

                    static	StringRep*	Clone (const StringRep& rep)	{ return (rep.Clone ()); }

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


            String	operator+ (const String& lhs, const String& rhs);

            bool	operator== (const String& lhs, const String& rhs);
            bool	operator== (const char* lhs, const String& rhs);
            bool	operator== (const String& lhs, const char* rhs);
            bool	operator!= (const String& lhs, const String& rhs);
            bool	operator!= (const char* lhs, const String& rhs);
            bool	operator!= (const String& lhs, const char* rhs);
            bool	operator< (const String& lhs, const String& rhs);
            bool	operator< (const char* lhs, const String& rhs);
            bool	operator< (const String& lhs, const char* rhs);
            bool	operator<= (const String& lhs, const String& rhs);
            bool	operator<= (const char* lhs, const String& rhs);
            bool	operator<= (const String& lhs, const char* rhs);
            bool	operator> (const String& lhs, const String& rhs);
            bool	operator> (const char* lhs, const String& rhs);
            bool	operator> (const String& lhs, const char* rhs);
            bool	operator>= (const String& lhs, const String& rhs);
            bool	operator>= (const char* lhs, const String& rhs);
            bool	operator>= (const String& lhs, const char* rhs);

            /*
             * Stream inserters and extractors.
             */
            ostream&	operator<< (ostream& out, const String& s);
            istream&	operator>> (istream& in, String& s);


            /*
             * Case conversion.
             */
            String	toupper (const String& s);
            String	tolower (const String& s);



            class	String_CharArray : public String {
                public:
                    String_CharArray ();
                    String_CharArray (const char* cString);
                    String_CharArray (const void* arrayOfBytes, size_t nBytes);
                    String_CharArray (const String& from);
                    String_CharArray (const String_CharArray& s);

                    String_CharArray& operator= (const String_CharArray& s);
            };



            class	String_BufferedCharArray  : public String {
                public:
                    String_BufferedCharArray ();
                    String_BufferedCharArray (const char* cString);
                    String_BufferedCharArray (const void* arrayOfBytes, size_t nBytes);
                    String_BufferedCharArray (const String& from);
                    String_BufferedCharArray (const String_BufferedCharArray& s);

                    String_BufferedCharArray& operator= (const String_BufferedCharArray& s);
            };




            class	String_ReadOnlyChar : public String {
                public:
                    String_ReadOnlyChar (const char* cString);
                    String_ReadOnlyChar (const void* arrayOfBytes, size_t nBytes);
                    String_ReadOnlyChar (const String_ReadOnlyChar& s);

                    String_ReadOnlyChar& operator= (const String_ReadOnlyChar& s);
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




#endif	/*__String__*/

