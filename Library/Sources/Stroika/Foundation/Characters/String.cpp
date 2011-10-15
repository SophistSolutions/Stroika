/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include    <algorithm>
#include	<cstdarg>
#include    <climits>
#include    <istream>
#include	<string>
#include	<regex>

#include	"../Containers/Common.h"
#include	"../Math/Common.h"
#include	"TString.h"

#include	"String.h"



using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Characters;


/*
 * INTERNAL (INCOMPLETE/PARTLY OBSOLETE) NOTES ABOUT STRING REPS:
 *
 
 *
 *		StringRep_Substring is a subclass of StringRep useful in the
 *		implementation of SubString operations. It allows for the sharing of memory
 *		of the original String::_Rep (whatever its representation) and still have
 *		the concept of a separate string.
 *		This can be a very big efficiency win when applying substring operations
 *		in a fashion that does not then modify the substrings. Then we get away with
 *		being lazy, in our lazy copy, and never really have to.
 *
 *		There are a whole raft of other potentially useful String::_Reps. For example,
 *		a String::_Rep that hooks into a backend database, or into the system
 *		TextEdit's implementations. A String::_Rep could compress its text to save
 *		space. A possibility used in GCC is to build a buffer that points to two
 *		other buffers as a way to implement operator+ in String efficiently. (NOTE:
 *		some of the above would require adding more interface to String::_Rep,
 *		in particular to make GetChar be a virtual method. This will probably be
 *		done, but it remains controversial due to the intrinsic overhead of
 *		virtual function invocations).
 *
 *		Although you can create a String from any one of these String::_Reps
 *		directly, it is far more common to do so via a convience interface. The
 *		constructor for String is overloaded to take an enum AllocMode argument
 *		which specifies how to interpret the rest of the arguments to the
 *		constructor (really those args are just passed on directly to the
 *		appropriate subclass of String::_Rep mentioned above by conditioned on
 *		the enum argument).
 *
 *		These AllocModes are:
 *			eBuffered:				which means use StringRep_CharArray.
 *			eFixedSize:				which means use StringRep_BufferedCharArray
 *
 *		At the risk of being redundent. Most of this need be of no concern. If
 *		you ignore the allocMode stuff and the StringRep stuff, it will all
 *		be handled for you with nearly optimal performance. One of the above
 *		AllocModes will be used (we dont document which)

 */




class	String_CharArray::MyRep_ : public String::_Rep {
	public:
		MyRep_ (const Character* arrayOfCharacters, size_t nBytes);
		~MyRep_ ();

		virtual		_Rep*	Clone () const override;

		virtual		size_t	GetLength () const override;
		virtual		bool	Contains (Character item) const override;
		virtual		void	RemoveAll () override;

		virtual		Character	GetAt (size_t index) const override;
		virtual		void		SetAt (Character item, size_t index) override;
		virtual		void		InsertAt (const Character* srcStart, const Character* srcEnd, size_t index) override;
		virtual		void		RemoveAt (size_t index, size_t amountToRemove) override;

		virtual		void	SetLength (size_t newLength) override;

		virtual		const Character*	Peek () const override;

		virtual	int	Compare (const _Rep& rhs, String::CompareOptions co) const override;

	protected:
		MyRep_ ();
		nonvirtual	void	SetStorage (Character* storage, size_t length);

		virtual	size_t	CalcAllocChars_ (size_t requested);

	private:
		wchar_t*	fStorage;
		size_t	    fLength;
};






/*
    * Subclasses from String_CharArray::MyRep_ instead of String::_Rep as a convenience (inheriting implementation).
    * In nearly all cases, such inheritance is a bad idea, but here it is justified because people
    * never directly manipulate the Reps, only the envelope classes, which have the conceptually
    * proper derivation. And the code savings is significant, since they differ only in
    * their buffering schemes.
    * Of course, not all subclasses of String::_Rep can do this, and if it ever posed a problem here it
    * could be modified without having any effect on peoples code.
    *
    */
class	String_BufferedCharArray::MyRep_ : public String_CharArray::MyRep_ {
    public:
        MyRep_ (const Character* arrayOfCharacters, size_t nBytes);

        virtual		_Rep*	Clone () const override;

    protected:
        virtual		size_t	CalcAllocChars_ (size_t requested) override;
};






class	String_ExternalMemoryOwnership::MyRep_ : public String_CharArray::MyRep_ {
    public:
        MyRep_ (const Character* arrayOfCharacters, size_t nBytes);
        ~MyRep_ ();

        virtual		void	RemoveAll () override;

        virtual		void	SetAt (Character item, size_t index) override;
        virtual		void	InsertAt (const Character* srcStart, const Character* srcEnd, size_t index) override;
        virtual		void	RemoveAt (size_t index, size_t amountToRemove) override;

        virtual		void	SetLength (size_t newLength) override;

    private:
        bool	fWeOwnBuffer_;

        // called before calling any method that modifies the string, to ensure that
        // we do not munge memory we did not alloc
        nonvirtual	void	AssureWeOwnBuffer_ ();
};







namespace	{
	// For now - disable this class cuz its broken - run regression tests to see.
	// I'm not clear on how this is supposed to work. While read-only, it works FINE. But if anyone ever makes ANY modifications (removall, etc), 
	// we REALLY want to switch to a differnt REP!!! Like o flength goes zero - we want to return the global kEmptyStringRep (maybe not).
	// But we more likely want to swithc the the string buffered rep or something. Maybe have virtual methods like InsertAt<>(in rep) retuirn SharedPtr<Rep> so it can be changed?
	// Or take ref-param of that guy so it can be changed???
	#define	qString_SubStringClassWorks	0

	#if		qString_SubStringClassWorks
	struct String_Substring_ : public String {
		class	MyRep_ : public String::_Rep {
			public:
				MyRep_ (const Memory::SharedByValue<_Rep>& baseString, size_t from, size_t length);

				virtual		_Rep*	Clone () const override;

				virtual		size_t	GetLength () const override;
				virtual		bool	Contains (Character item) const override;
				virtual		void	RemoveAll () override;

				virtual		Character	GetAt (size_t index) const override;
				virtual		void		SetAt (Character item, size_t index) override;
				virtual		void		InsertAt (const Character* srcStart, const Character* srcEnd, size_t index) override;
				virtual		void		RemoveAt (size_t index, size_t amountToRemove) override;

				virtual		void	SetLength (size_t newLength) override;

				virtual		const Character*	Peek () const override;
				virtual	int	Compare (const _Rep& rhs, String::CompareOptions co) const override;

			private:
				Memory::SharedByValue<_Rep>	fBase;

				size_t	fFrom;
				size_t	fLength;
		};
	};
	#endif
}







/*
 ********************************************************************************
 ************************************* String ***********************************
 ********************************************************************************
 */
namespace	{
	struct	MyEmptyString_ : String {
		static	String::_Rep*	Clone_ (const _Rep& rep)
			{
				return (rep.Clone ());
			}
		static	Memory::SharedByValue<String::_Rep>	mkEmptyStrRep_ ()
			{
				static	bool								sInited_	=	false;
				static	Memory::SharedByValue<String::_Rep>	s_;
				if (not sInited_) {
					sInited_ = true;
					s_ = Memory::SharedByValue<String::_Rep> (DEBUG_NEW String_CharArray::MyRep_ (nullptr, 0), &Clone_);
				}
				return s_;
			}
	};
}

String::String ()
	: fRep_ (MyEmptyString_::mkEmptyStrRep_ ())
{
}

String::String (const char16_t* cString)
	: fRep_ (MyEmptyString_::mkEmptyStrRep_ ())
{
	RequireNotNull (cString);
	// Horrible, but temporarily OK impl
	for (const char16_t* i = cString; *i != '\0'; ++i) {
		InsertAt (*i, (i - cString));
	}
}

String::String (const wchar_t* cString)
	: fRep_ (new String_CharArray::MyRep_ ((const Character*)cString, wcslen (cString)), &Clone_)
{
	RequireNotNull (cString);
	static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
}

String::String (const wchar_t* from, const wchar_t* to)
	: fRep_ (new String_CharArray::MyRep_ ((const Character*)from, to-from), &Clone_)
{
	Require (from <= to);
	Require (from != nullptr or from == to);
	static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
}

String::String (const Character* from, const Character* to)
	: fRep_ (new String_CharArray::MyRep_ (from, to-from), &Clone_)
{
	Require (from <= to);
	Require (from != nullptr or from == to);
}

String::String (const std::wstring& r)
    : fRep_ (new String_CharArray::MyRep_ ((const Character*)r.c_str (), r.length ()), &Clone_)
{
}

String::String (_Rep* sharedPart, bool)
	: fRep_ (sharedPart, &Clone_)
{
	RequireNotNull (sharedPart);
	Require (fRep_.unique ());
}

String	String::FromUTF8 (const char* from)
{
	return UTF8StringToWide (from);
}

String	String::FromUTF8 (const std::string& from)
{
	return UTF8StringToWide (from);
}

String	String::FromTString (const TChar* from)
{
	return TString2Wide (from);
}

String	String::FromTString (const basic_string<TChar>& from)
{
	return TString2Wide (from);
}

String&	String::operator+= (Character appendage)
{
	fRep_->InsertAt (&appendage, &appendage + 1, GetLength ());
	return (*this);
}

String&	String::operator+= (const String& appendage)
{
	if ((fRep_ == appendage.fRep_) and fRep_.unique ()) {
		/*
		 * We must be careful about this case since blowing aways "this"s memory, and then
		 * trying to copy from "appendage"s would be bad.
		 */
		String tmp = appendage;
		return (operator+= (tmp));
	}
	else {
		size_t	oldLength	=	GetLength ();
		size_t	appendLength = appendage.GetLength ();
		SetLength (oldLength + appendLength);
		Assert (appendage.GetLength () == appendLength);
		Assert (fRep_.unique ());
		Character*	dstBuf	=	const_cast<Character*>(&(fRep_->Peek ())[oldLength]);
		appendage.CopyTo (dstBuf, dstBuf + appendLength);
	}
	return (*this);
}

void	String::SetLength (size_t newLength)
{
	if (newLength == 0) {
		fRep_->RemoveAll ();
	}
	else {
		fRep_->SetLength (newLength);
	}
}

void	String::SetCharAt (Character c, size_t i)
{
	Require (i >= 0);
	Require (i < GetLength ());
	fRep_->SetAt (c, i);
}

void	String::InsertAt (Character c, size_t i)
{
	Require (i >= 0);
	Require (i <= (GetLength ()));
	fRep_->InsertAt (&c, &c + 1, i);
}

void	String::RemoveAt (size_t i, size_t amountToRemove)
{
	Require (i >= 0);
	Require ((i+amountToRemove) <= (GetLength ()));
	fRep_->RemoveAt (i, amountToRemove);
}

void	String::Remove (Character c)
{
	size_t index = IndexOf (c);
	if (index != kBadStringIndex) {
		fRep_->RemoveAt (index, 1);
	}
}

size_t	String::IndexOf (Character c) const
{
	size_t length = GetLength ();
	for (size_t i = 0; i < length; i++) {
		if (fRep_->GetAt (i) == c) {
			return (i);
		}
	}
	return (kBadStringIndex);
}

size_t	String::IndexOf (const String& subString) const
{
	if (subString.GetLength () == 0) {
		return ((GetLength () == 0) ? kBadStringIndex : 0);
	}
	if (GetLength () < subString.GetLength ()) {
		return (kBadStringIndex);	// important test cuz size_t is unsigned
	}

	size_t	subStrLen	=	subString.GetLength ();
	size_t	limit		=	GetLength () - subStrLen;
	for (size_t i = 0; i <= limit; i++) {
		for (size_t j = 0; j < subStrLen; j++) {
			if (fRep_->GetAt (i+j) != subString.fRep_->GetAt (j)) {
				goto nogood;
			}
		}
		return (i);
nogood:
		;
	}
	return (kBadStringIndex);
}

size_t	String::RIndexOf (Character c) const
{
	size_t length = GetLength ();
	for (size_t i = length; i > 0; --i) {
		if (fRep_->GetAt (i-1) == c) {
			return (i-1);
		}
	}
	return (kBadStringIndex);
}

size_t	String::RIndexOf (const String& subString) const
{
	/*
	 * Do quickie implementation, and dont worry about efficiency...
	 */
	if (subString.GetLength () == 0) {
		return ((GetLength () == 0) ? kBadStringIndex : GetLength ()-1);
	}

	size_t	subStrLen	=	subString.GetLength ();
	size_t	limit		=	GetLength () - subStrLen+1;
	for (size_t i = limit; i > 0; --i) {
		if (SubString (i-1, i - 1 + subStrLen) == subString) {
			return (i-1);
		}
	}
	return (kBadStringIndex);
}

bool	String::Contains (Character c) const
{
	return (fRep_->Contains (c));
}

bool	String::Contains (const String& subString) const
{
	return bool (IndexOf (subString) != kBadStringIndex);
}

bool	String::Match (const String& regEx) const
{
	#if		qCompilerAndStdLib_Bug_regexpr
		AssertNotImplemented ();
		return false;
	#else
		wstring	tmp	=	As<wstring> ();
		return regex_match (tmp.begin(), tmp.end(), wregex (regEx.As<wstring> ()));
	#endif
}

vector<String>	String::Find (const String& regEx) const
{
	vector<String>	result;
	#if		qCompilerAndStdLib_Bug_regexpr
		AssertNotImplemented ();
	#else
		wstring	tmp		=	As<wstring> ();
		wregex	regExp	=	wregex (regEx.As<wstring> ());
		std::wsmatch res;
		regex_search (tmp, res, regExp);
		result.reserve (res.size ());
		for (wsmatch::const_iterator i = res.begin (); i != res.end (); ++i) {
			result.push_back (String (*i));
		}
	#endif
	return result;
}

String	String::Replace (const String& regEx, const String& with) const
{
	#if		qCompilerAndStdLib_Bug_regexpr
		AssertNotImplemented ();
		return String ();
	#else
		return String (regex_replace (As<wstring> (), wregex (regEx.As<wstring> ()), with.As<wstring> ()));
	#endif
}

String	String::SubString (size_t from, size_t to) const
{
	size_t	length	=	to-from;	// akwardly written cuz arg used to be 'length' - but fix impl later...
	Require (from >= 0);
	Require ((to == kBadStringIndex) or (length <= (GetLength ()-from) and (length >= 0)));

	if (length == 0) {
		return String ();
    }
	if (length == kBadStringIndex) {
		length = GetLength () - from;
	}
	if (from == 0 and length == GetLength ()) {
		// just bump reference count
		return *this;
	}
	#if		qString_SubStringClassWorks
		return (String (new String_Substring_::MyRep_ (fRep_, from, length), false));
	#else
		return (String (fRep_->Peek () + from, fRep_->Peek () + from + length));
	#endif
}

String	String::LTrim (bool (*shouldBeTrimmmed) (Character)) const
{
	RequireNotNull (shouldBeTrimmmed);
	// Could be much more efficient if pushed into REP - so we avoid each character virtual call...
	size_t length = GetLength ();
	for (size_t i = 0; i < length; ++i) {
		if (not (*shouldBeTrimmmed) (fRep_->GetAt (i))) {
			if (i == 0) {
				// no change in string
				return *this;
			}
			else {
				return SubString (i, length);
			}
		}
	}
	// all trimmed
	return String ();
}

String	String::RTrim (bool (*shouldBeTrimmmed) (Character)) const
{
	RequireNotNull (shouldBeTrimmmed);
	// Could be much more efficient if pushed into REP - so we avoid each character virtual call...
	size_t length = GetLength ();
	if (length != 0) {
		for (ptrdiff_t i = length - 1; i != 0; --i) {
			if (not (*shouldBeTrimmmed) (fRep_->GetAt (i))) {
				size_t	nCharsRemoved	=	(length - 1) - i;
				if (nCharsRemoved == 0) {
					// no change in string
					return *this;
				}
				else {
					return SubString (0, length - nCharsRemoved);
				}
			}
		}
	}
	// all trimmed
	return String ();
}

String	String::Trim (bool (*shouldBeTrimmmed) (Character)) const
{
	RequireNotNull (shouldBeTrimmmed);
	/*
	 * This could be implemented more efficient, but this is simpler for now..
	 */
	return LTrim (shouldBeTrimmmed).RTrim (shouldBeTrimmmed);
}

String	String::StripAll (bool (*removeCharIf) (Character)) const
{
	RequireNotNull (removeCharIf);

	// optimize special case where removeCharIf is always false
	//
	// Walk string and find first character we need to remove
	size_t	n	=	GetLength ();
	for (size_t i = 0; i < n; ++i) {
		Character	c	=	operator[] (i);
		if ((removeCharIf) (c)) {
			// on first removal, clone part of string done so far, and start appending
			String	tmp	=	SubString (0, i);
			// Now keep iterating IN THIS LOOP appending characters and return at the end of this loop
			i++;
			for (; i < n; ++i) {
				c	=	operator[] (i);
				if (not (removeCharIf) (c)) {
					tmp += c;
				}
			}
			return tmp;
		}
	}
	return *this;	// if we NEVER get removeCharIf return false, just clone this
}

String	String::ToLowerCase () const
{
	// Copy the string first (cheap cuz just refcnt) - but be sure to check if any real change before calling SetAt cuz SetAt will do the actual copy-on-write
	String	result	=	*this;
	size_t	n	=	result.GetLength ();
	for (size_t i = 0; i < n; ++i) {
		Character	c	=	result[i];
		if (c.IsUpperCase ()) {
			result.SetCharAt (c.ToLowerCase (), i);
		}
	}
	return result;
}

String	String::ToUpperCase () const
{
	// Copy the string first (cheap cuz just refcnt) - but be sure to check if any real change before calling SetAt cuz SetAt will do the actual copy-on-write
	String	result	=	*this;
	size_t	n	=	result.GetLength ();
	for (size_t i = 0; i < n; ++i) {
		Character	c	=	result[i];
		if (c.IsLowerCase ()) {
			result.SetCharAt (c.ToUpperCase (), i);
		}
	}
	return result;
}

bool String::IsWhitespace () const
{
	size_t	n	=	GetLength ();
	for (size_t i = 0; i < n; ++i) {
		Character	c	=	operator[] (i);
		if (not c.IsWhitespace ()) {
			return false;
		}
	}
	return true;
}

template	<>
	void	String::AsUTF8 (string* into) const
		{
			RequireNotNull (into);
			*into = WideStringToUTF8 (As<wstring> ());	//tmphack impl (but shoudl work)
		}

TString	String::AsTString () const
{
	TString	result;
	AsTString (&result);
	return result;
}

void	String::AsTString (TString* into) const
{
	RequireNotNull (into);
	*into = Wide2TString (As<wstring> ());	// poor inefficient implementation
}

template	<>
	void	String::AsASCII (string* into) const
		{
			RequireNotNull (into);

			into->clear ();
			size_t	len	=	GetLength ();
			into->reserve (len);
			for (size_t i = 0; i < len; ++i) {
				into->push_back (operator[] (i).GetAsciiCode ());
			}
		}









/*
 ********************************************************************************
 ******************************** String_CharArray ******************************
 ********************************************************************************
 */
String_CharArray::String_CharArray ()
	: String (new String_CharArray::MyRep_ (nullptr, 0), false)
{
}

String_CharArray::String_CharArray (const wchar_t* cString)
	: String (new String_CharArray::MyRep_ ((const Character*)cString, wcslen (cString)), false)
{
    static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
}

String_CharArray::String_CharArray (const wstring& str)
    : String (new String_CharArray::MyRep_ ((const Character*)str.c_str (), str.length ()), false)
{
    static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
}


String_CharArray::String_CharArray (const String& from)
	: String (new String_CharArray::MyRep_ (from._PeekRep ()->Peek (), from.GetLength ()), false)
{
}


String_CharArray::String_CharArray (const Character* arrayOfCharacters, size_t nCharacters)
	: String (new String_CharArray::MyRep_ (arrayOfCharacters, nCharacters), false)
{
}





/*
 ********************************************************************************
 **************************** String_BufferedCharArray **************************
 ********************************************************************************
 */
String_BufferedCharArray::String_BufferedCharArray ()
	: String (new MyRep_ (nullptr, 0), false)
{
}

String_BufferedCharArray::String_BufferedCharArray (const wchar_t* cString)
	: String (new MyRep_ ((const Character*)cString, wcslen (cString)), false)
{
    static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
}

String_BufferedCharArray::String_BufferedCharArray (const wstring& str)
	: String (new MyRep_ ((const Character*) str.c_str (), str.length ()), false)
{
    static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
}

String_BufferedCharArray::String_BufferedCharArray (const String& from)
	: String (new MyRep_ (from._PeekRep ()->Peek (), from.GetLength ()), false)
{
}








/*
 ********************************************************************************
 *********************** String_ExternalMemoryOwnership *************************
 ********************************************************************************
 */
String_ExternalMemoryOwnership::String_ExternalMemoryOwnership (const wchar_t* cString)
	: String (new MyRep_ ((const Character*)cString, wcslen (cString)), false)
{
    Assert (sizeof (Character) == sizeof (wchar_t))
}






/*
 ********************************************************************************
 ****************************** String_StackLifetime ****************************
 ********************************************************************************
 */
String_StackLifetime::String_StackLifetime (const wchar_t* cString)
	: String (cString)
{
	/* TODO: FIX PERFORMANCE!!!
	 *		This implementation conforms to the requirements of the API, so that this class CAN be used safely. However, it does NOT exhibit the performance
	 *	advantages the class description promises.
	 *
	 *		TODO so - it must do its own rep (similar to String_ExternalMemoryOwnership::MyRep_) - except that it must ALSO have an extra method - FREEZE (or some such).
	 *	Then in the DTOR for this envelope, we call FREEZE on that rep - causing it to throw away its unsafe pointer. That must ONLY be done if refcount > 1 (in our DTOR).
	 */
}









/*
 ********************************************************************************
 ******************************** String::_Rep **********************************
 ********************************************************************************
 */








/*
 ********************************************************************************
 *************************** String_CharArray::MyRep_ ***************************
 ********************************************************************************
 */
String_CharArray::MyRep_::MyRep_ (const Character* arrayOfCharacters, size_t nCharacters)
	: _Rep ()
	, fStorage (nullptr)
	, fLength (nCharacters)
{
    Assert (sizeof (Character) == sizeof (wchar_t))

	fStorage = ::new wchar_t [CalcAllocChars_ (fLength)];
	if (arrayOfCharacters != nullptr) {
		memcpy (fStorage, arrayOfCharacters, fLength*sizeof (Character));
	}
}

String_CharArray::MyRep_::MyRep_ ()
	: _Rep ()
	, fStorage (nullptr)
	, fLength (0)
{
}

String_CharArray::MyRep_::~MyRep_ ()
{
	delete fStorage;
}

String::_Rep*	String_CharArray::MyRep_::Clone () const
{
	return (new String_CharArray::MyRep_ ((const Character*)fStorage, fLength));
}

size_t	String_CharArray::MyRep_::GetLength () const
{
	return (fLength);
}

bool	String_CharArray::MyRep_::Contains (Character item) const
{
	Assert (fStorage != NULL or GetLength () == 0);

	char asciiItem = item.GetAsciiCode ();
	for (size_t i = 0; i < fLength; i++) {
		if (fStorage[i] == asciiItem) {
			return (true);
		}
	}
	return (false);
}

void	String_CharArray::MyRep_::RemoveAll ()
{
	SetLength (0);
}

Character	String_CharArray::MyRep_::GetAt (size_t index) const
{
	Require (index >= 0);
	Require (index < GetLength ());
	AssertNotNull (fStorage);

	return (fStorage[index]);
}

void	String_CharArray::MyRep_::SetAt (Character item, size_t index)
{
	Require (index >= 0);
	Require (index < GetLength ());
	AssertNotNull (fStorage);

	fStorage[index] = item.As<wchar_t> ();
}

void	String_CharArray::MyRep_::InsertAt (const Character* srcStart, const Character* srcEnd, size_t index)
{
	Require (index >= 0);
	Require (index <= GetLength ());

Assert (srcEnd - srcStart == 1);	//tmphack - just havne't implemtend more general case
Character item = *srcStart;


	SetLength (GetLength () + 1);
	if (index < (fLength-1)) {
		wchar_t*	lhs	=	&fStorage [fLength];
		wchar_t*	rhs	=	&fStorage [fLength-1];
		Assert ((fLength-index) > 0);
		size_t amt = fLength-index-1;   // minus one because fLength increased by one by SetLength above
		for (size_t i = 1; i <= amt; ++i) {
			*lhs-- = *rhs--;
		}
		Assert (lhs == &fStorage [index]);
	}
	fStorage[index] = item.As<wchar_t> ();
}

void	String_CharArray::MyRep_::RemoveAt (size_t index, size_t amountToRemove)
{
	Require (index >= 0);
	Require (index < fLength);

	if (index < fLength-1) {
		wchar_t*	lhs	=	&fStorage [index];
		wchar_t*	rhs	=	&fStorage [index+amountToRemove];
		for (size_t i = fLength - index - amountToRemove; i > 0; i--) {
			*lhs++ = *rhs++;
		}
	}
	fLength -= amountToRemove;
}

void	String_CharArray::MyRep_::SetLength (size_t newLength)
{
	size_t	oldAllocChars	=	CalcAllocChars_ (fLength);
	size_t	newAllocChars	=	CalcAllocChars_ (newLength);

	if (oldAllocChars != newAllocChars) {
		Assert (newAllocChars >= newLength);

        size_t amountToAlloc = newAllocChars*sizeof (Character);
		fStorage = (wchar_t*)realloc (fStorage, amountToAlloc);
	}
	fLength = newLength;
    Ensure (fStorage != NULL or GetLength () == 0);
	Ensure (fLength == newLength);
}

const Character*	String_CharArray::MyRep_::Peek () const
{
    Require (sizeof (Character) == sizeof (wchar_t));

	return ((const Character*)fStorage);
}

/* Deliberately does not try to free up old fStorage, as this could be other than heap memory */
void	String_CharArray::MyRep_::SetStorage (Character* storage, size_t length)
{
    Require (sizeof (Character) == sizeof (wchar_t))
	fStorage = (wchar_t*)storage;
	fLength = length;
}

size_t	String_CharArray::MyRep_::CalcAllocChars_ (size_t requested)
{
	return (requested);
}


int	String_CharArray::MyRep_::Compare (const _Rep& rhs, String::CompareOptions co) const override
{
	Require (co == eWithCase_CO or co == eCaseInsensitive_CO);
    if (this == &rhs) {
        return (0);
    }
	// Need a more efficient implementation - but this should do for starters...
	switch (co) {
		case	eWithCase_CO: {
		    size_t lLen = GetLength ();
		    size_t rLen = rhs.GetLength ();
			size_t length	=	min (lLen, rLen);
			for (size_t i = 0; i < length; i++) {
				if (fStorage[i] != rhs.GetAt (i)) {
					return (fStorage[i] - rhs.GetAt (i).GetCharacterCode ());
				}
			}
			return Containers::CompareResultNormalizeHelper<ptrdiff_t,int> (static_cast<ptrdiff_t> (lLen) - static_cast<ptrdiff_t> (rLen));
		}
										
		case	eCaseInsensitive_CO: 	{
			// Not sure wcscasecmp even helps because of convert to c-str
			//return ::wcscasecmp (l.c_str (), r.c_str ());;
		    size_t lLen = GetLength ();
		    size_t rLen = rhs.GetLength ();
			size_t length	=	min (lLen, rLen);
			for (size_t i = 0; i < length; i++) {
				Character	lc	=	Character (fStorage[i]).ToLowerCase ();
				Character	rc	=	rhs.GetAt (i).ToLowerCase ();
				if (lc.GetCharacterCode () != rc.GetCharacterCode ()) {
					return (lc.GetCharacterCode () - rc.GetCharacterCode ());
				}
			}
			return Containers::CompareResultNormalizeHelper<ptrdiff_t,int> (static_cast<ptrdiff_t> (lLen) - static_cast<ptrdiff_t> (rLen));
		}

		default:
			AssertNotReached ();
			return 0;
	}
}





/*
 ********************************************************************************
 *********************** String_BufferedCharArray::MyRep_ ***********************
 ********************************************************************************
 */
String_BufferedCharArray::MyRep_::MyRep_ (const Character* arrayOfCharacters, size_t nCharacters)
	: String_CharArray::MyRep_ ()
{
	RequireNotNull (arrayOfCharacters);
	Assert (sizeof (Character) == sizeof (wchar_t))

	wchar_t*	storage = ::new wchar_t [CalcAllocChars_ (nCharacters)];
	memcpy (storage, arrayOfCharacters, nCharacters*sizeof (Character));
	SetStorage ((Character*)storage, nCharacters);
}

String::_Rep*	String_BufferedCharArray::MyRep_::Clone () const
{
	return (new String_BufferedCharArray::MyRep_ (Peek (), GetLength ()));
}

size_t	String_BufferedCharArray::MyRep_::CalcAllocChars_ (size_t requested)
{
	// round up to buffer block size
	return (Stroika::Foundation::Math::RoundUpTo (requested, static_cast<size_t> (32)));
}








/*
 ********************************************************************************
 ****************** String_ExternalMemoryOwnership::MyRep_ **********************
 ********************************************************************************
 */
String_ExternalMemoryOwnership::MyRep_::MyRep_ (const Character* arrayOfCharacters, size_t nCharacters)
	: String_CharArray::MyRep_ ()
	, fWeOwnBuffer_ (false)
{
	RequireNotNull (arrayOfCharacters);
	SetStorage (const_cast<Character*>(arrayOfCharacters), nCharacters);
}

String_ExternalMemoryOwnership::MyRep_::~MyRep_ ()
{
	if (not fWeOwnBuffer_) {
		SetStorage (nullptr, 0);	// make sure that memory is not deleted, as we never alloced
	}
}

void	String_ExternalMemoryOwnership::MyRep_::RemoveAll ()
{
	AssureWeOwnBuffer_ ();
	String_CharArray::MyRep_::RemoveAll ();
}

void	String_ExternalMemoryOwnership::MyRep_::SetAt (Character item, size_t index)
{
	AssureWeOwnBuffer_ ();
	String_CharArray::MyRep_::SetAt (item, index);
}

void	String_ExternalMemoryOwnership::MyRep_::InsertAt (const Character* srcStart, const Character* srcEnd, size_t index)
{
	AssureWeOwnBuffer_ ();
	String_CharArray::MyRep_::InsertAt (srcStart, srcEnd, index);
}

void	String_ExternalMemoryOwnership::MyRep_::RemoveAt (size_t index, size_t amountToRemove)
{
	AssureWeOwnBuffer_ ();
	String_CharArray::MyRep_::RemoveAt (index, amountToRemove);
}

void	String_ExternalMemoryOwnership::MyRep_::SetLength (size_t newLength)
{
	AssureWeOwnBuffer_ ();
	String_CharArray::MyRep_::SetLength (newLength);
}

void	String_ExternalMemoryOwnership::MyRep_::AssureWeOwnBuffer_ ()
{
	if (not fWeOwnBuffer_) {
	    Assert (sizeof (Character) == sizeof (wchar_t));

		size_t	len	=	GetLength ();
		wchar_t* storage = ::new wchar_t [len];
		CopyTo (storage, storage + len);
		SetStorage (reinterpret_cast<Character*> (storage), len);
		fWeOwnBuffer_ = true;
	}

	Ensure (fWeOwnBuffer_);
}








#if		qString_SubStringClassWorks
/*
 ********************************************************************************
 ************************** String_Substring_::MyRep_ ***************************
 ********************************************************************************
 */
String_Substring_::MyRep_::MyRep_ (const Memory::SharedByValue<_Rep>& baseString, size_t from, size_t length)
	: fBase (baseString)
	, fFrom (from)
	, fLength (length)
{
	Require (from >= 0);
	Require (length >= 0);
	Require ((from + length) <= fBase->GetLength ());
}

String::_Rep*	String_Substring_::MyRep_::Clone () const
{
	return (new String_CharArray::MyRep_ (Peek (), GetLength ()));
}

size_t	String_Substring_::MyRep_::GetLength () const
{
	return (fLength);
}

bool	String_Substring_::MyRep_::Contains (Character item) const
{
	for (size_t i = 0; i < fLength; i++) {
		if (GetAt (i) == item) {
			return (true);
		}
	}
	return (false);
}

void	String_Substring_::MyRep_::RemoveAll ()
{
	fBase->RemoveAll ();
	fFrom = 0;
	fLength = 0;
}

Character	String_Substring_::MyRep_::GetAt (size_t index) const
{
	Require (index < GetLength ());
	Assert ((fFrom+index) < fBase->GetLength ());
	return (fBase->GetAt (fFrom+index));
}

void	String_Substring_::MyRep_::SetAt (Character item, size_t index)
{
	Require (index < GetLength ());
	Assert ((fFrom+index) < fBase->GetLength ());
	fBase->SetAt (item, (fFrom+index));
}

void	String_Substring_::MyRep_::InsertAt (const Character* srcStart, const Character* srcEnd, size_t index)
{
	Require (index <= GetLength ());
	Assert ((fFrom+index) <= fBase->GetLength ());

	size_t	lenBeforeInsert	=	GetLength();


	fBase->InsertAt (srcStart, srcEnd, (fFrom+index));
	fLength++;
}

void	String_Substring_::MyRep_::RemoveAt (size_t index, size_t amountToRemove)
{
	Require (index < GetLength ());
	Require ((index+amountToRemove) < GetLength ());

	Assert ((fFrom+index+amountToRemove) < fBase->GetLength ());
	fBase->RemoveAt (index, amountToRemove);
	fLength -= amountToRemove;
}

void	String_Substring_::MyRep_::SetLength (size_t newLength)
{
	size_t delta = newLength - fLength;
	if (delta != 0) {
		fBase->SetLength (fBase->GetLength () + delta);
		fLength += delta;
	}
}

const Character*	String_Substring_::MyRep_::Peek () const
{
	Assert (fFrom <= fBase->GetLength ());

	const Character* buffer = fBase->Peek ();
	return (&buffer[fFrom]);
}
#endif









/*
 ********************************************************************************
 ************************************* operator+ ********************************
 ********************************************************************************
 */
String	Stroika::Foundation::Characters::operator+ (const String& lhs, const String& rhs)
{
#if 1
	String_BufferedCharArray	tmp	=	String_BufferedCharArray (lhs);

	// Very cruddy implementation - but should be functional -- LGP 2011-09-08
	for (size_t i = 0; i < rhs.GetLength (); ++i) {
		tmp.InsertAt (rhs[i], tmp.GetLength ());
	}
	return tmp;
#elif 0
    size_t	lhsLen	=	lhs.GetLength ();
    size_t	rhsLen	=	rhs.GetLength ();
    String	s	=	String (nullptr, lhsLen + rhsLen);			// garbage string of right size
    memcpy (&((char*)s.Peek ())[0], lhs.Peek (), size_t (lhsLen));
    memcpy (&((char*)s.Peek ())[lhsLen], rhs.Peek (), size_t (rhsLen));
    return (s);
#endif
}





/*
 ********************************************************************************
 ************************************* operator== *******************************
 ********************************************************************************
 */
bool	Stroika::Foundation::Characters::operator== (const String& lhs, const String& rhs)
{
    if (lhs.fRep_ == rhs.fRep_) {
        return (true);
    }
    size_t length = lhs.GetLength ();

    if (length != rhs.GetLength ()) {
        return (false);
    }

    for (size_t i = 0; i < length; i++) {
        if (lhs[i] != rhs[i]) {
            return (false);
        }
    }
    return (true);
}

bool	Stroika::Foundation::Characters::operator== (const wchar_t* lhs, const String& rhs)
{
    RequireNotNull (lhs);
    size_t length = ::wcslen (lhs);

    if (length != rhs.GetLength ()) {
        return (false);
    }

    for (size_t i = 0; i < length; i++) {
        if (Character(lhs[i]) != rhs[i]) {
            return (false);
        }
    }
    return (true);
}

bool	Stroika::Foundation::Characters::operator== (const String& lhs, const wchar_t* rhs)
{
    RequireNotNull (rhs);
    size_t length = lhs.GetLength ();

    if (length != ::wcslen (rhs)) {
        return (false);
    }

    for (size_t i = 0; i < length; i++) {
        if (lhs[i] != Character (rhs[i])) {
            return (false);
        }
    }
    return (true);
}






/*
 ********************************************************************************
 ************************************* operator< ********************************
 ********************************************************************************
 */
bool	Stroika::Foundation::Characters::operator< (const String& lhs, const String& rhs)
{
    if (lhs.fRep_ == rhs.fRep_) {
        return (false);
    }
    size_t length = min (lhs.GetLength (), rhs.GetLength ());
    for (size_t i = 0; i < length; i++) {
        if (lhs[i] != rhs[i]) {
            return (bool (lhs[i] < rhs[i]));
        }
    }
    return (bool (rhs.GetLength () != length));	// implies lhs.GetLength () < rhs.GetLength
}

bool	Stroika::Foundation::Characters::operator< (const wchar_t* lhs, const String& rhs)
{
    RequireNotNull (lhs);
    return (String_ExternalMemoryOwnership (lhs) < rhs);
}

bool	Stroika::Foundation::Characters::operator< (const String& lhs, const wchar_t* rhs)
{
    RequireNotNull (rhs);
    return (lhs < String_ExternalMemoryOwnership (rhs));
}




/*
 ********************************************************************************
 ************************************* operator<= *******************************
 ********************************************************************************
 */
bool	Stroika::Foundation::Characters::operator<= (const String& lhs, const String& rhs)
{
    if (lhs.fRep_ == rhs.fRep_) {
        return (true);
    }
    size_t length = min (lhs.GetLength (), rhs.GetLength ());
    for (size_t i = 0; i < length; i++) {
        if (lhs[i] != rhs[i]) {
            return (bool (lhs[i] < rhs[i]));
        }
    }
    return (bool (lhs.GetLength () == length)); 	// implies lhs.GetLength () <= rhs.GetLength
}

bool	Stroika::Foundation::Characters::operator<= (const wchar_t* lhs, const String& rhs)
{
    RequireNotNull (lhs);
    return (String_StackLifetime (lhs) <= rhs);
}

bool	Stroika::Foundation::Characters::operator<= (const String& lhs, const wchar_t* rhs)
{
    RequireNotNull (rhs);
    RequireNotNull (rhs);
    return (lhs <= String_StackLifetime (rhs));
}



