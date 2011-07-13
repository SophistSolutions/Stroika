/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__String__
#define	__String__

/*
 * $Header: /fuji/lewis/RCS/String.hh,v 1.17 1992/11/12 08:13:23 lewis Exp $
 *
 * Description:
 *
 *		Conceptually, this class should inherit from Sequence(Character).
 *		Since that would be too expensive, it doesn't, but instead,
 *		follows same protocol.  Now there are no such things as protocols, in
 *		C++, but we interpret this word loosely from SmallTalk-80.
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
 *
 * Design Notes:
 *
 *		Used to have unsigned char* ctor for Pascal Style strings, but
 *		decided against this, as it is unsafe.
 *
 * TODO:
 *
 * Changes:
 *	$Log: String.hh,v $
 *		Revision 1.17  1992/11/12  08:13:23  lewis
 *		Moved some inlines to implenmation detail section.
 *
 *		Revision 1.16  1992/10/16  22:49:34  lewis
 *		Add string compare overloads of const char* (op==, etc). This was
 *		needed cuz of ambigutiy problems with overloading.
 *
 *		Revision 1.15  1992/10/10  03:56:51  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.14  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.13  1992/10/09  05:35:31  lewis
 *		Get rid of remainging usage/support for kEmptyString.
 *
 *		Revision 1.12  1992/10/07  23:05:55  sterling
 *		Conditionanly add supporot for new GenClass instead of declare macros.
 *
 *		Revision 1.11  1992/09/15  16:54:09  lewis
 *		Include Sequnce.hh - not AbSequence.hh
 *
 *		Revision 1.10  1992/09/11  15:04:38  sterling
 *		massive changes, new structure, some new reps
 *
 *		Revision 1.9  1992/09/05  04:19:59  lewis
 *		Renamed NULL->Nil again.
 *
 *		Revision 1.8  1992/09/01  15:20:18  sterling
 *		Lots.
 *
 *		Revision 1.7  1992/07/22  02:31:25  lewis
 *		Tried to make the Character a real class, with I strong - temporary
 *		- eye toward backward compatability. Did minimal hacks here to get things
 *		working quickly. Interface is roughly right, but internally we are still
 *		totally char* based.
 *
 *		Revision 1.6  1992/07/21  05:21:42  lewis
 *		Include new file Character.hh - make real changes to this later.
 *
 *		Revision 1.5  1992/07/16  07:36:57  lewis
 *		Moved class	StringRep to head of file since BCC templates seemed
 *		to prefer having it there.
 *
 *		Revision 1.4  1992/07/16  05:14:58  lewis
 *		Added new method Contains - a shortcut for saying IndexOf () != kBadSequenceIndex.
 *
 *		Revision 1.3  1992/07/07  07:11:05  lewis
 *		Get rid of AssertNotNil (fRep) since now that we use Shared
 *		it is redundent, and it happened to make BCC barf.
 *		Also, cleaned up comments.
 *
 *		Revision 1.2  1992/07/02  03:16:43  lewis
 *		Moved all of the template (macro based) declarations for String/Real from
 *		here to Stroika-Founation-Globals.hh.
 *
 *		Revision 1.17  1992/06/09  17:53:57  lewis
 *		ReFixed bug VERY subtle bug Sterl had fixed in his last release - one with b+=b for Strings.
 *		He had just changed arg to op+= to String (from const String&). I changed hack so more common
 *		cases pay no performance hit, and fixed it totally locally to the function.
 *
 *		Revision 1.16  92/06/09  12:36:32  12:36:32  lewis (Lewis Pringle)
 *		Renamed SmartPointer back to Shared.
 *		
 *		Revision 1.15  92/06/09  01:22:22  01:22:22  lewis (Lewis Pringle)
 *		Merged in sterls changes to use new SmartPointer class instead of old Shared scheme.
 *		
 *		Revision 1.14  1992/05/23  00:12:03  lewis
 *		Use "Shared.hh" instead of "Memory.hh".
 *
 *		Revision 1.13  92/05/21  17:25:31  17:25:31  lewis (Lewis Pringle)
 *		Changed definition of substring.
 *		
 *		Revision 1.12  92/05/13  00:10:21  00:10:21  lewis (Lewis Pringle)
 *		Comment on DTOR of StringRep.
 *		
 *		Revision 1.11  92/03/30  13:46:03  13:46:03  lewis (Lewis Pringle)
 *		Added ifdefs around Declare macrso so not done with templates available.
 *		
 *		Revision 1.8  1992/03/06  05:37:56  lewis
 *		Minor cleanups, and worked on commenting things.
 *
 *		Revision 1.7  1992/01/14  05:55:31  lewis
 *		Added Set_Array (String/Real).
 *
 *		Revision 1.5  1991/12/04  19:26:08  lewis
 *		added ToC/PString w/wo Trunc.
 *
 */

#include	"Character.hh"
#include	"Debug.hh"
#include	"Sequence.hh"
#include	"Shared.hh"



	//	qMPW_CFRONT_BUG__LoadDumpBreaksNestedClassDefns
	class	StringRep {
		protected:
			StringRep ();
			
		public:
			virtual	~StringRep ();
	
			virtual	StringRep*	Clone () const		= Nil;

			virtual	size_t	GetLength () const 						= Nil;
			virtual	Boolean	Contains (Character item) const					= Nil;
			virtual	void	RemoveAll () 									= Nil;
	
			virtual	Character	GetAt (size_t index) const				= Nil;
			virtual	void		SetAt (Character item, size_t index)	= Nil;
			virtual	void		InsertAt (Character item, size_t index)	= Nil;
			virtual	void		RemoveAt (size_t index, size_t amountToRemove)	= Nil;

			virtual	void	SetLength (size_t newLength) 	= Nil;

			virtual	const void*	Peek () const 						= Nil;
	};



#if		!qRealTemplatesAvailable
	#include "TFileMap.hh"
	#include SharedOfStringRep_hh
#endif

class	String {
	public:
		String ();
		String (const char* cString);
		String (const void* arrayOfBytes, size_t nBytes);	// if arrayOfBytes Nil then not copied from, and initial data in string undefined
		String (const String& from);

		~String ()	{}		// NOTE: NON-VIRTUAL!!!!!

		nonvirtual	String&	operator= (const String& newString);

		nonvirtual	String&	operator+= (Character appendage);
		nonvirtual	String&	operator+= (const String& appendage);

		nonvirtual	size_t	GetLength () const;
		nonvirtual	void			SetLength (size_t newLength);

		nonvirtual	Character	operator[] (size_t i) const;
		nonvirtual	void		SetCharAt (Character c, size_t i);
		
		nonvirtual	void		InsertAt (Character c, size_t i);
		nonvirtual	void		RemoveAt (size_t i);
		nonvirtual	void		RemoveAt (size_t i, size_t amountToRemove);
		nonvirtual	void		Remove (Character c);

		/*
		 * inherited from Sequence. Lookup the character (or string) in this string, and return
		 * its index - either starting from the front, or end of the string. Returns kBadSequenceIndex
		 * if none found.
		 */
		nonvirtual	size_t	IndexOf (Character c) const;
		nonvirtual	size_t	IndexOf (const String& subString) const;
		nonvirtual	size_t	RIndexOf (Character c) const;
		nonvirtual	size_t	RIndexOf (const String& subString) const;
		nonvirtual	Boolean			Contains (Character c) const;
		nonvirtual	Boolean			Contains (const String& subString) const;

		/*
		 * Produce a substring of this string, starting at from, and extending length characters. If length
		 * is kBadSequenceIndex (default) then return all the way to the end of the string.
		 */
		nonvirtual	String		SubString (size_t from, size_t length = kBadSequenceIndex) const;

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

#if		qMacOS
		// length byte demarcated string...
		nonvirtual	unsigned char*	ToPString () const;
		nonvirtual	unsigned char*	ToPString (unsigned char buf [], size_t bufferSize) const;
		nonvirtual	unsigned char*	ToPStringTrunc () const;
		nonvirtual	unsigned char*	ToPStringTrunc (unsigned char buf [], size_t bufferSize) const;
#endif	/*qMacOS*/

	protected:
		String (StringRep* sharedPart, Boolean ignored);	// Boolean arg to disamiguate constructors
		
		nonvirtual	void	SetRep (StringRep* rep);

		nonvirtual	const StringRep*	GetRep () const;
		nonvirtual	StringRep*			GetRep ();

	private:		
#if		qRealTemplatesAvailable
		Shared<StringRep>	fRep;
#else
		Shared(StringRep)	fRep;
#endif

		static	StringRep*	Clone (const StringRep& rep)	{ return (rep.Clone ()); }

	/*
	 * These are made friends so they can peek at the shared part, as an optimization/
	 */
	friend	Boolean	operator== (const String& lhs, const String& rhs);
	friend	Boolean	operator!= (const String& lhs, const String& rhs);
	friend	Boolean	operator< (const String& lhs, const String& rhs);
	friend	Boolean	operator<= (const String& lhs, const String& rhs);
	friend	Boolean	operator> (const String& lhs, const String& rhs);
	friend	Boolean	operator>= (const String& lhs, const String& rhs);

	// constructs a StringRep_Catenate
	friend	String	operator+ (const String& lhs, const String& rhs);
};


String	operator+ (const String& lhs, const String& rhs);

Boolean	operator== (const String& lhs, const String& rhs);
Boolean	operator== (const char* lhs, const String& rhs);
Boolean	operator== (const String& lhs, const char* rhs);
Boolean	operator!= (const String& lhs, const String& rhs);
Boolean	operator!= (const char* lhs, const String& rhs);
Boolean	operator!= (const String& lhs, const char* rhs);
Boolean	operator< (const String& lhs, const String& rhs);
Boolean	operator< (const char* lhs, const String& rhs);
Boolean	operator< (const String& lhs, const char* rhs);
Boolean	operator<= (const String& lhs, const String& rhs);
Boolean	operator<= (const char* lhs, const String& rhs);
Boolean	operator<= (const String& lhs, const char* rhs);
Boolean	operator> (const String& lhs, const String& rhs);
Boolean	operator> (const char* lhs, const String& rhs);
Boolean	operator> (const String& lhs, const char* rhs);
Boolean	operator>= (const String& lhs, const String& rhs);
Boolean	operator>= (const char* lhs, const String& rhs);
Boolean	operator>= (const String& lhs, const char* rhs);


/*
 * Stream inserters and extractors.
 */
class ostream;
class istream;
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

class	StringRep_CharArray : public StringRep {
	public:
		StringRep_CharArray (const void* arrayOfBytes, size_t nBytes);
		~StringRep_CharArray ();

		override	StringRep*	Clone () const;

		override	size_t	GetLength () const;
		override	Boolean	Contains (Character item) const;
		override	void	RemoveAll ();

		override	Character	GetAt (size_t index) const;
		override	void		SetAt (Character item, size_t index);
		override	void		InsertAt (Character item, size_t index);
		override	void		RemoveAt (size_t index, size_t amountToRemove);

		override	void	SetLength (size_t newLength);

		override	const void*	Peek () const;
	
	protected:
		StringRep_CharArray ();
		nonvirtual	void	SetStorage (char* storage, size_t length);
	
	private:			
		size_t	fLength;
		char*			fStorage;

		virtual	size_t	CalcAllocSize (size_t requested);
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


/*
 * Subclasses from StringRep_CharArray instead of StringRep as a convenience (inheriting implementation).
 * In nearly all cases, such inheritance is a bad idea, but here it is justified because people
 * never directly manipulate the Reps, only the envelope classes, which have the conceptually
 * proper derivation. And the code savings is significant, since they differ only in 
 * their buffering schemes.
 * Of course, not all subclasses of StringRep can do this, and if it ever posed a problem here it
 * could be modified without having any effect on peoples code.
 *
 */
class	StringRep_BufferedCharArray : public StringRep_CharArray {
	public:
		StringRep_BufferedCharArray (const void* arrayOfBytes, size_t nBytes);

		override	StringRep*	Clone () const;

	private:
		override	size_t	CalcAllocSize (size_t requested);
};


class	String_ReadOnlyChar : public String {
	public:
		String_ReadOnlyChar (const char* cString);
		String_ReadOnlyChar (const void* arrayOfBytes, size_t nBytes);
		String_ReadOnlyChar (const String_ReadOnlyChar& s);

		String_ReadOnlyChar& operator= (const String_ReadOnlyChar& s);
};

/*
 * Subclasses from StringRep_CharArray instead of StringRep as a convenience (inheriting implementation).
 * In nearly all cases, such inheritance is a bad idea, but here it is justified because people
 * never directly manipulate the Reps, only the envelope classes, which have the conceptually
 * proper derivation. This class differs from StringRep_CharArray only in that it normally does
 * not allocate any memory, simply pointing at the memory the user provides. However, it must
 * have all the capabilities of the full StringRep_CharArray, so that if forced to modify itself,
 * it can do so in a safe manner. This could be implemented through ownership instead of inheritance,
 * but that would be less efficient in a place where efficiency counts.
 * Of course, not all subclasses of StringRep can do this, and if it ever posed a problem here it
 * could be modified without having any effect on peoples code.
 *
 * One interesting thing to note is that we do not override the Clone method here. This is
 * justified, in that the Clone method only requires that we return a StringRep*, not an exact
 * duplicate. And it is a good idea since we only call Clone when we wish to apply modifications,
 * but StringRep_ReadOnlyChar is a class that dislikes applying modifications.
 */
class	StringRep_ReadOnlyChar : public StringRep_CharArray {
	public:
		StringRep_ReadOnlyChar (const void* arrayOfBytes, size_t nBytes);
		~StringRep_ReadOnlyChar ();

		override	void	RemoveAll ();

		override	void	SetAt (Character item, size_t index);
		override	void	InsertAt (Character item, size_t index);
		override	void	RemoveAt (size_t index, size_t amountToRemove);

		override	void	SetLength (size_t newLength);

	private:
		Boolean	fAllocedMem;
		
		// called before calling any method that modifies the string, to ensure that
		// we do not munge memory we did not alloc
		nonvirtual	void	AssureMemAllocated ();
};


class	StringRep_Substring : public StringRep {
	public:
#if		qRealTemplatesAvailable
		StringRep_Substring (const Shared<StringRep>& baseString, size_t from, size_t length);
#else
		StringRep_Substring (const Shared(StringRep)& baseString, size_t from, size_t length);
#endif

		override	StringRep*	Clone () const;

		override	size_t	GetLength () const;
		override	Boolean	Contains (Character item) const;
		override	void	RemoveAll ();

		override	Character	GetAt (size_t index) const;
		override	void		SetAt (Character item, size_t index);
		override	void		InsertAt (Character item, size_t index);
		override	void		RemoveAt (size_t index, size_t amountToRemove);

		override	void	SetLength (size_t newLength);

		override	const void*	Peek () const;

	private:
#if		qRealTemplatesAvailable
		Shared<StringRep>	fBase;
#else
		Shared(StringRep)	fBase;
#endif
		size_t	fFrom;
		size_t	fLength;
};


class	StringRep_Catenate : public StringRep {
	public:
		StringRep_Catenate (const String& lhs, const String& rhs);

		override	StringRep*	Clone () const;

		override	size_t	GetLength () const;
		override	Boolean	Contains (Character item) const;
		override	void	RemoveAll ();

		override	Character	GetAt (size_t index) const;
		override	void		SetAt (Character item, size_t index);
		override	void		InsertAt (Character item, size_t index);
		override	void		RemoveAt (size_t index, size_t amountToRemove);

		override	void	SetLength (size_t newLength);

		override	const void*	Peek () const;

	private:
		String			fLeft;
		String			fRight;
		size_t	fLength;
			
		nonvirtual	void	Normalize ();
};








/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */
inline	StringRep::StringRep ()	{}

inline	String::String (const String& from):	fRep (from.fRep)	{}
	
inline	String::String (const void* arrayOfBytes, size_t nBytes):
	fRep (&Clone, Nil)
{
	fRep = new StringRep_CharArray (arrayOfBytes, nBytes);
}
	
inline	String&	String::operator= (const String& newString)
{
	fRep = newString.fRep;
	return (*this);
}

inline	void	String::RemoveAt (size_t i)	{ RemoveAt (i, 1); }

inline	const void*	String::Peek () const						{	return (fRep->Peek ());	}
inline	size_t	String::GetLength () const				{	return (fRep->GetLength ());	}
inline	Character	String::operator[] (size_t i) const
{
	Require (i >= 1);
	Require (i <= GetLength ());
	return (fRep->GetAt (i));
}

inline	void	String::SetRep (StringRep* rep)	{ fRep = rep; }

inline	const StringRep*	String::GetRep () const	{ return (fRep.GetPointer ()); }
inline	StringRep*			String::GetRep ()		{ return (fRep.GetPointer ()); }

inline	Boolean	operator!= (const String& lhs, const String& rhs)
{
	return (Boolean (not (lhs == rhs)));
}

inline	Boolean	operator!= (const char* lhs, const String& rhs)
{
	return (Boolean (not (lhs == rhs)));
}

inline	Boolean	operator!= (const String& lhs, const char* rhs)
{
	return (Boolean (not (lhs == rhs)));
}

inline	Boolean	operator> (const String& lhs, const String& rhs)
{
	return (Boolean (not (lhs <= rhs)));
}

inline	Boolean	operator> (const char* lhs, const String& rhs)
{
	return (Boolean (not (lhs <= rhs)));
}

inline	Boolean	operator> (const String& lhs, const char* rhs)
{
	return (Boolean (not (lhs <= rhs)));
}

inline	Boolean	operator>= (const String& lhs, const String& rhs)
{
	return (Boolean (not (lhs < rhs)));
}

inline	Boolean	operator>= (const char* lhs, const String& rhs)
{
	return (Boolean (not (lhs < rhs)));
}

inline	Boolean	operator>= (const String& lhs, const char* rhs)
{
	return (Boolean (not (lhs < rhs)));
}

inline	String_CharArray::String_CharArray (const String_CharArray& s)	: String (s)	{}
inline	String_CharArray& String_CharArray::operator= (const String_CharArray& s)	{	String::operator= (s);	return (*this); }

inline	String_BufferedCharArray::String_BufferedCharArray (const String_BufferedCharArray& s)	: String (s)	{}
inline	String_BufferedCharArray& String_BufferedCharArray::operator= (const String_BufferedCharArray& s)	{	String::operator= (s);	return (*this); }

inline	String_ReadOnlyChar::String_ReadOnlyChar (const String_ReadOnlyChar& s)	: String (s)	{}
inline	String_ReadOnlyChar& String_ReadOnlyChar::operator= (const String_ReadOnlyChar& s)	{	String::operator= (s);	return (*this); }



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__String__*/

