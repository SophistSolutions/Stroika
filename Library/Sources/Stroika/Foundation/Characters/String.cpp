/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include    <algorithm>
#include	<cstdarg>
#include    <climits>
#include    <istream>
#include	<string>

#include	"../Math/Common.h"

#include	"String.h"



using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Characters;



namespace	{



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
            StringRep_BufferedCharArray (const Character* arrayOfCharacters, size_t nBytes);

            virtual		StringRep*	Clone () const override;

        private:
            virtual		size_t	CalcAllocSize (size_t requested) override;
    };


}



class	String_ReadOnlyChar::MyRep_ : public StringRep_CharArray {
    public:
        MyRep_ (const Character* arrayOfCharacters, size_t nBytes);
        ~MyRep_ ();

        virtual		void	RemoveAll () override;

        virtual		void	SetAt (Character item, size_t index) override;
        virtual		void	InsertAt (Character item, size_t index) override;
        virtual		void	RemoveAt (size_t index, size_t amountToRemove) override;

        virtual		void	SetLength (size_t newLength) override;

    private:
        bool	fAllocedMem;

        // called before calling any method that modifies the string, to ensure that
        // we do not munge memory we did not alloc
        nonvirtual	void	AssureMemAllocated ();
};


namespace	{

    class	StringRep_Substring : public String::StringRep {
        public:
            StringRep_Substring (const Memory::Shared<StringRep>& baseString, size_t from, size_t length);

            virtual		StringRep*	Clone () const override;

            virtual		size_t	GetLength () const override;
            virtual		bool	Contains (Character item) const override;
            virtual		void	RemoveAll () override;

            virtual		Character	GetAt (size_t index) const override;
            virtual		void		SetAt (Character item, size_t index) override;
            virtual		void		InsertAt (Character item, size_t index) override;
            virtual		void		RemoveAt (size_t index, size_t amountToRemove) override;

            virtual		void	SetLength (size_t newLength) override;

            virtual		const Character*	Peek () const override;

        private:
            Memory::Shared<StringRep>	fBase;

            size_t	fFrom;
            size_t	fLength;
    };

}




namespace	{

    class	StringRep_Catenate : public String::StringRep {
        public:
            StringRep_Catenate (const String& lhs, const String& rhs);

            virtual		StringRep*	Clone () const override;

            virtual		size_t	GetLength () const override;
            virtual		bool	Contains (Character item) const override;
            virtual		void	RemoveAll () override;

            virtual		Character	GetAt (size_t index) const override;
            virtual		void		SetAt (Character item, size_t index) override;
            virtual		void		InsertAt (Character item, size_t index) override;
            virtual		void		RemoveAt (size_t index, size_t amountToRemove) override;

            virtual		void	SetLength (size_t newLength) override;

            virtual		const Character*	Peek () const override;

        private:
            String			fLeft;
            String			fRight;
            size_t			fLength;

            nonvirtual	void	Normalize ();
    };

}





/*
 ********************************************************************************
 ************************************* String ***********************************
 ********************************************************************************
 */
String::String ()
	: fRep (&Clone_, nullptr)
{
	static	Memory::Shared<StringRep>	sSharedBuffer (&Clone_, new StringRep_CharArray (nullptr, 0));

	fRep = sSharedBuffer;
	Assert (fRep.CountReferences () > 1);
}

String::String (const wchar_t* cString)
	: fRep (&Clone_, nullptr)
{
	RequireNotNull (cString);
	Assert (sizeof (Character) == sizeof (wchar_t));
	fRep = new StringRep_CharArray ((const Character*)cString, wcslen (cString));
}

String::String (StringRep* sharedPart, bool)
	: fRep (&Clone_, sharedPart)
{
	Require (fRep.CountReferences () == 1);
}

String&	String::operator+= (Character appendage)
{
	fRep->InsertAt (appendage, GetLength ()+1);
	return (*this);
}

String&	String::operator+= (const String& appendage)
{
	if ((fRep == appendage.fRep) and (fRep.CountReferences () == 1)) {
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
		memcpy (const_cast<Character*>(&(fRep->Peek ())[oldLength]), appendage.fRep->Peek (), appendLength*sizeof (Character));
	}
	return (*this);
}

void	String::SetLength (size_t newLength)
{
	if (newLength == 0) {
		fRep->RemoveAll ();
	}
	else {
		fRep->SetLength (newLength);
	}
}

void	String::SetCharAt (Character c, size_t i)
{
	Require (i >= 0);
	Require (i < GetLength ());
	fRep->SetAt (c, i);
}

void	String::InsertAt (Character c, size_t i)
{
	Require (i >= 0);
	Require (i <= (GetLength ()));
	fRep->InsertAt (c, i);
}

void	String::RemoveAt (size_t i, size_t amountToRemove)
{
	Require (i >= 0);
	Require ((i+amountToRemove) <= (GetLength ()));
	fRep->RemoveAt (i, amountToRemove);
}

void	String::Remove (Character c)
{
	size_t index = IndexOf (c);
	if (index != kBadStringIndex) {
		fRep->RemoveAt (index, 1);
	}
}

size_t	String::IndexOf (Character c) const
{
	size_t length = GetLength ();
	for (size_t i = 0; i < length; i++) {
		if (fRep->GetAt (i) == c) {
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
		for (size_t j = 1; j <= subStrLen; j++) {
			if (fRep->GetAt (i+j) != subString.fRep->GetAt (j)) {
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
		if (fRep->GetAt (i-1) == c) {
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
	size_t	limit		=	GetLength () - subStrLen;
	for (size_t i = limit; i > 0; --i) {
		if (SubString (i-1, subStrLen) == subString) {
			return (i-1);
		}
	}
	return (kBadStringIndex);
}

bool	String::Contains (Character c) const
{
	return (fRep->Contains (c));
}

bool	String::Contains (const String& subString) const
{
	return bool (IndexOf (subString) != kBadStringIndex);
}

String	String::SubString (size_t from, size_t length) const
{
	Require (from >= 0);
	Require ((length == kBadStringIndex) or (length <= (GetLength ()-from) and (length >= 0)));

	if (length == 0) {
		return (L"");   // SSW: replacement for kEmptyString doesn't catch this
    }
	if (length == kBadStringIndex) {
		length = GetLength () - from;
	}
	return (String (new StringRep_Substring (fRep, from, length), false));
}



/*
 ********************************************************************************
 ******************************** String_CharArray ******************************
 ********************************************************************************
 */
String_CharArray::String_CharArray ()
	: String (nullptr, false)
{
	SetRep (new StringRep_CharArray (nullptr, 0));
}

String_CharArray::String_CharArray (const wchar_t* cString)
	: String (nullptr, false)
{
	size_t length = wcslen (cString);
    Assert (sizeof (Character) == sizeof (wchar_t));
	SetRep (new StringRep_CharArray ((const Character*)cString, length));
}

String_CharArray::String_CharArray (const wstring& str)
    : String (nullptr, false)
{
    Assert (sizeof (Character) == sizeof (wchar_t));
	SetRep (new StringRep_CharArray ((const Character*)str.c_str (), str.length ()));
}


String_CharArray::String_CharArray (const String& from)
	: String (nullptr, false)
{
	SetRep (new StringRep_CharArray (from.Peek (), from.GetLength ()));
}


 String_CharArray::String_CharArray (const Character* arrayOfCharacters, size_t nCharacters)
 {
      SetRep (new StringRep_CharArray (arrayOfCharacters, nCharacters));
 }



/*
 ********************************************************************************
 **************************** String_BufferedCharArray **************************
 ********************************************************************************
 */
String_BufferedCharArray::String_BufferedCharArray ()
	: String (nullptr, false)
{
	SetRep (new StringRep_BufferedCharArray (nullptr, 0));
}

String_BufferedCharArray::String_BufferedCharArray (const wchar_t* cString)
	: String (nullptr, false)
{
    Assert (sizeof (Character) == sizeof (wchar_t));
	size_t length = wcslen (cString);
	SetRep (new StringRep_BufferedCharArray ((const Character*)cString, length));
}

String_BufferedCharArray::String_BufferedCharArray (const wstring& str)
	: String (nullptr, false)
{
    Assert (sizeof (Character) == sizeof (wchar_t));
	SetRep (new StringRep_BufferedCharArray ((const Character*) str.c_str (), str.length ()));
}

String_BufferedCharArray::String_BufferedCharArray (const String& from)
	: String (nullptr, false)
{
	SetRep (new StringRep_BufferedCharArray (from.Peek (), from.GetLength ()));
}





/*
 ********************************************************************************
 ****************************** String_ReadOnlyChar *****************************
 ********************************************************************************
 */
String_ReadOnlyChar::String_ReadOnlyChar (const wchar_t* cString)
	: String (nullptr, false)
{
    Assert (sizeof (Character) == sizeof (wchar_t))
	size_t length = wcslen (cString);
	SetRep (new MyRep_ ((const Character*)cString, length));
}





/*
 ********************************************************************************
 *********************************** StringRep **********************************
 ********************************************************************************
 */
String::StringRep::~StringRep ()
{
}






/*
 ********************************************************************************
 ******************************** StringRep_CharArray ***************************
 ********************************************************************************
 */
StringRep_CharArray::StringRep_CharArray (const Character* arrayOfCharacters, size_t nCharacters)
	: StringRep ()
	, fStorage (nullptr)
	, fLength (nCharacters)
{
    Assert (sizeof (Character) == sizeof (wchar_t))

	fStorage = ::new wchar_t [CalcAllocSize (fLength)];
	if (arrayOfCharacters != nullptr) {
		memcpy (fStorage, arrayOfCharacters, fLength*sizeof (Character));
	}
}


StringRep_CharArray::StringRep_CharArray ()
	: StringRep ()
	, fStorage (nullptr)
	, fLength (0)
{
}

StringRep_CharArray::~StringRep_CharArray ()
{
	delete fStorage;
}

String::StringRep*	StringRep_CharArray::Clone () const
{
	return (new StringRep_CharArray ((const Character*)fStorage, fLength));
}

size_t	StringRep_CharArray::GetLength () const
{
	return (fLength);
}

bool	StringRep_CharArray::Contains (Character item) const
{
	AssertNotNull (fStorage);

	char asciiItem = item.GetAsciiCode ();
	for (size_t i = 0; i < fLength; i++) {
		if (fStorage[i] == asciiItem) {
			return (true);
		}
	}
	return (false);
}

void	StringRep_CharArray::RemoveAll ()
{
	SetLength (0);
}

Character	StringRep_CharArray::GetAt (size_t index) const
{
	Require (index >= 0);
	Require (index < GetLength ());
	AssertNotNull (fStorage);

	return (fStorage[index]);
}

void	StringRep_CharArray::SetAt (Character item, size_t index)
{
	Require (index >= 0);
	Require (index < GetLength ());
	AssertNotNull (fStorage);

	fStorage[index] = item;
}

void	StringRep_CharArray::InsertAt (Character item, size_t index)
{
	Require (index >= 0);
	Require (index <= GetLength ());

	SetLength (GetLength () + 1);
	if (index < fLength) {
		wchar_t*	lhs	=	&fStorage [fLength];
		wchar_t*	rhs	=	&fStorage [fLength-1];
		size_t i = fLength;
		for (; i >= index; i--) {
			*lhs-- = *rhs--;
		}
		Assert (i == index);
		Assert (lhs == &fStorage [index]);
	}
	fStorage[index] = item.GetAsciiCode ();
}

void	StringRep_CharArray::RemoveAt (size_t index, size_t amountToRemove)
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

void	StringRep_CharArray::SetLength (size_t newLength)
{
	AssertNotNull (fStorage);

	size_t	oldAllocSize	=	CalcAllocSize (fLength);
	size_t	newAllocSize	=	CalcAllocSize (newLength);

	if (oldAllocSize != newAllocSize) {
		Assert (newAllocSize >= newLength);
		fStorage = (wchar_t*)realloc (fStorage, newAllocSize);
	}
	fLength = newLength;
	Ensure (fLength == newLength);
}

const Character*	StringRep_CharArray::Peek () const
{
    Require (sizeof (Character) == sizeof (wchar_t));

	return ((const Character*)fStorage);
}

/* Deliberately does not try to free up old fStorage, as this could be other than heap memory */
void	StringRep_CharArray::SetStorage (Character* storage, size_t length)
{
    Require (sizeof (Character) == sizeof (wchar_t))
	fStorage = (wchar_t*)storage;
	fLength = length;
}

size_t	StringRep_CharArray::CalcAllocSize (size_t requested)
{
	return (requested);
}





/*
 ********************************************************************************
 **************************** StringRep_BufferedCharArray ***********************
 ********************************************************************************
 */
StringRep_BufferedCharArray::StringRep_BufferedCharArray (const Character* arrayOfCharacters, size_t nCharacters)
	: StringRep_CharArray ()
{
	RequireNotNull (arrayOfCharacters);
	Assert (sizeof (Character) == sizeof (wchar_t))

	wchar_t*	storage = ::new wchar_t [CalcAllocSize (nCharacters)];
	memcpy (storage, arrayOfCharacters, nCharacters*sizeof (Character));
	SetStorage ((Character*)storage, nCharacters);
}

String::StringRep*	StringRep_BufferedCharArray::Clone () const
{
	return (new StringRep_BufferedCharArray (Peek (), GetLength ()));
}

size_t	StringRep_BufferedCharArray::CalcAllocSize (size_t requested)
{
	// round up to buffer block size
	return (Stroika::Foundation::Math::RoundUpTo (requested, static_cast<size_t> (32)));
}






/*
 ********************************************************************************
 ****************************** String_ReadOnlyChar::MyRep_ **************************
 ********************************************************************************
 */
String_ReadOnlyChar::MyRep_::MyRep_ (const Character* arrayOfCharacters, size_t nCharacters)
	: StringRep_CharArray ()
	, fAllocedMem (false)
{
	RequireNotNull (arrayOfCharacters);
	SetStorage (const_cast<Character*>(arrayOfCharacters), nCharacters);
}

String_ReadOnlyChar::MyRep_::~MyRep_ ()
{
	if (not fAllocedMem) {
		SetStorage (nullptr, 0);	// make sure that memory is not deleted, as we never alloced
	}
}

void	String_ReadOnlyChar::MyRep_::RemoveAll ()
{
	AssureMemAllocated ();
	StringRep_CharArray::RemoveAll ();
}

void	String_ReadOnlyChar::MyRep_::SetAt (Character item, size_t index)
{
	AssureMemAllocated ();
	StringRep_CharArray::SetAt (item, index);
}

void	String_ReadOnlyChar::MyRep_::InsertAt (Character item, size_t index)
{
	AssureMemAllocated ();
	StringRep_CharArray::InsertAt (item, index);
}

void	String_ReadOnlyChar::MyRep_::RemoveAt (size_t index, size_t amountToRemove)
{
	AssureMemAllocated ();
	StringRep_CharArray::RemoveAt (index, amountToRemove);
}

void	String_ReadOnlyChar::MyRep_::SetLength (size_t newLength)
{
	AssureMemAllocated ();
	StringRep_CharArray::SetLength (newLength);
}

// SSW 8/30/2011: Note: this looks wrong as no assignment to the storage. Need to copy the exiting characters over!!
void	String_ReadOnlyChar::MyRep_::AssureMemAllocated ()
{
	if (not fAllocedMem) {
	    Assert (sizeof (Character) == sizeof (wchar_t))
		SetStorage ((Character*) ::new wchar_t [GetLength ()], GetLength ());
		fAllocedMem = true;
	}

	Ensure (fAllocedMem);
}








/*
 ********************************************************************************
 ******************************** StringRep_Substring ***************************
 ********************************************************************************
 */
StringRep_Substring::StringRep_Substring (const Memory::Shared<StringRep>& baseString, size_t from, size_t length)
	: fBase (baseString)
	, fFrom (from)
	, fLength (length)
{
	Require (from >= 0);
	Require (length >= 0);
	Require ((from + length) <= fBase->GetLength ());
}

String::StringRep*	StringRep_Substring::Clone () const
{
	return (new StringRep_CharArray (Peek (), GetLength ()));
}

size_t	StringRep_Substring::GetLength () const
{
	return (fLength);
}

bool	StringRep_Substring::Contains (Character item) const
{
	for (size_t i = 0; i < fLength; i++) {
		if (GetAt (i) == item) {
			return (true);
		}
	}
	return (false);
}

void	StringRep_Substring::RemoveAll ()
{
	fBase->RemoveAll ();
	fFrom = 0;
	fLength = 0;
}

Character	StringRep_Substring::GetAt (size_t index) const
{
	Require (index < GetLength ());
	Assert ((fFrom+index) < fBase->GetLength ());
	return (fBase->GetAt (fFrom+index));
}

void	StringRep_Substring::SetAt (Character item, size_t index)
{
	Require (index < GetLength ());
	Assert ((fFrom+index) < fBase->GetLength ());
	fBase->SetAt (item, (fFrom+index));
}

void	StringRep_Substring::InsertAt (Character item, size_t index)
{
	Require (index <= GetLength ());
	Assert ((fFrom+index) <= fBase->GetLength ());
	fBase->InsertAt (item, (fFrom+index));
	fLength++;
}

void	StringRep_Substring::RemoveAt (size_t index, size_t amountToRemove)
{
	Require (index < GetLength ());
	Require ((index+amountToRemove) < GetLength ());

	Assert ((fFrom+index+amountToRemove) < fBase->GetLength ());
	fBase->RemoveAt (index, amountToRemove);
	fLength -= amountToRemove;
}

void	StringRep_Substring::SetLength (size_t newLength)
{
	size_t delta = newLength - fLength;
	if (delta != 0) {
		fBase->SetLength (fBase->GetLength () + delta);
		fLength += delta;
	}
}

const Character*	StringRep_Substring::Peek () const
{
	Assert (fFrom <= fBase->GetLength ());

	const Character* buffer = fBase->Peek ();
	return (&buffer[fFrom]);
}




/*
 ********************************************************************************
 ******************************** StringRep_Catenate ****************************
 ********************************************************************************
 */
StringRep_Catenate::StringRep_Catenate (const String& lhs, const String& rhs)
	: fLeft (lhs)
	, fRight (rhs)
	, fLength (lhs.GetLength () + rhs.GetLength ())
{
}

String::StringRep*	StringRep_Catenate::Clone () const
{
	StringRep_CharArray*	s	=	new StringRep_CharArray (nullptr, GetLength ());
	size_t lhsLengthInBytes = fLeft.GetLength ()*sizeof (Character);
	Assert (sizeof (Character) == sizeof (wchar_t));
	memcpy (&((Character*)s->Peek ())[0], fLeft.Peek (), lhsLengthInBytes);
	memcpy (&((Character*)s->Peek ())[lhsLengthInBytes], fRight.Peek (), fRight.GetLength ()*sizeof (Character));
	return (s);
}

size_t	StringRep_Catenate::GetLength () const
{
	Assert (fLength == (fLeft.GetLength () + fRight.GetLength ()));
	return (fLength);
}

bool	StringRep_Catenate::Contains (Character item) const
{
	return (bool ((fLeft.Contains (item)) or (fRight.Contains (item))));
}

void	StringRep_Catenate::RemoveAll ()
{
	fLeft = L"";
	fRight = L"";
	fLength = 0;
}

Character	StringRep_Catenate::GetAt (size_t index) const
{
	size_t length = fLeft.GetLength ();
	if (index <= length) {
		return (fLeft[index]);
	}
	else {
		return (fRight[(index - length)]);
	}
}

void	StringRep_Catenate::SetAt (Character item, size_t index)
{
	Normalize ();
	fLeft.SetCharAt (item, index);
}

void	StringRep_Catenate::InsertAt (Character item, size_t index)
{
	Normalize ();
	fLeft.InsertAt (item, index);
	fLength++;
}

void	StringRep_Catenate::RemoveAt (size_t index, size_t amountToRemove)
{
	Normalize ();
	fLeft.RemoveAt (index, amountToRemove);
	fLength -= amountToRemove;
}

void	StringRep_Catenate::SetLength (size_t newLength)
{
	Normalize ();
	fLeft.SetLength (newLength);
	fLength = newLength;
}

const Character*	StringRep_Catenate::Peek () const
{
	const_cast<StringRep_Catenate*> (this)->Normalize ();
	return (fLeft.Peek ());
}


void	StringRep_Catenate::Normalize ()
{
	if (fRight != L"") {
		String_CharArray newL	=	String_CharArray (nullptr, GetLength ());

        Assert (sizeof (Character) == sizeof (wchar_t));
        size_t lhsLengthInBytes = fLeft.GetLength ()*sizeof (Character);
        memcpy (&((Character*)newL.Peek ())[0], fLeft.Peek (), lhsLengthInBytes);
        memcpy (&((Character*)newL.Peek ())[lhsLengthInBytes], fRight.Peek (), fRight.GetLength ()*sizeof (Character));

		fLeft = newL;
		fRight = L"";
	}
	Assert (fLength == (fLeft.GetLength () + fRight.GetLength ()));
}





/*
 ********************************************************************************
 ************************************* operator+ ********************************
 ********************************************************************************
 */
String	Stroika::Foundation::Characters::operator+ (const String& lhs, const String& rhs)
{
#if 0
    size_t	lhsLen	=	lhs.GetLength ();
    size_t	rhsLen	=	rhs.GetLength ();
    String	s	=	String (nullptr, lhsLen + rhsLen);			// garbage string of right size
    memcpy (&((char*)s.Peek ())[0], lhs.Peek (), size_t (lhsLen));
    memcpy (&((char*)s.Peek ())[lhsLen], rhs.Peek (), size_t (rhsLen));
    return (s);
#else
    return (String (new StringRep_Catenate (lhs, rhs), false));
#endif
}





/*
 ********************************************************************************
 ************************************* operator== *******************************
 ********************************************************************************
 */
bool	Stroika::Foundation::Characters::operator== (const String& lhs, const String& rhs)
{
    if (lhs.fRep == rhs.fRep) {
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
    if (lhs.fRep == rhs.fRep) {
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
    return (String_ReadOnlyChar (lhs) < rhs);
}

bool	Stroika::Foundation::Characters::operator< (const String& lhs, const wchar_t* rhs)
{
    RequireNotNull (rhs);
    return (lhs < String_ReadOnlyChar (rhs));
}




/*
 ********************************************************************************
 ************************************* operator<= *******************************
 ********************************************************************************
 */
bool	Stroika::Foundation::Characters::operator<= (const String& lhs, const String& rhs)
{
    if (lhs.fRep == rhs.fRep) {
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
    return (String_ReadOnlyChar (lhs) <= rhs);
}

bool	Stroika::Foundation::Characters::operator<= (const String& lhs, const wchar_t* rhs)
{
    RequireNotNull (rhs);
    RequireNotNull (rhs);
    return (lhs <= String_ReadOnlyChar (rhs));
}





/*
 ********************************************************************************
 ************************************* operator<< *******************************
 ********************************************************************************
 */
ostream&	operator<< (ostream& out, const String& s)
{
    for (size_t i = 1; i <= s.GetLength (); i++) {
        out << s[i].GetAsciiCode ();
    }
    return (out);
}




/*
 ********************************************************************************
 ************************************* operator>> *******************************
 ********************************************************************************
 */
wistream&	operator>> (wistream& in, String& s)
{
    s = L"";
    wchar_t	bigBuf [2048];
    bigBuf[0] = '\0';
    // Do same as operator>> (istream&, char*)...
	std::streamsize oldWidth = in.width (sizeof (bigBuf)-1);	// prevent overflow (I hope thats what this does??)
    in >> bigBuf;
    in.width (oldWidth);							// maybe unncessary - reset automatically???

// LGP Thursday, October 8, 1992 2:59:51 AM
// Sterl - I know you used gcount>0 but that breaks with libg++ 2.2.
// Not sure why. This should work fine with cfront and libg++ - why did
// you change it??? If it helps, I added a NUL termination to the
// array before using builtin- char* version - not sure that matters???
// BTW, I check (in) rather than (in.good ()) since I dont want to get
// a string on EOF - I already assigned empty string above - that should be
// it.
    if (in.good ()) {
        s = String (bigBuf);
    }
    return (in);
}




/*
 ********************************************************************************
 ************************************* toupper **********************************
 ********************************************************************************
 */
String	toupper (const String& s)
{
    String			result	=	s;
    size_t	rLen	=	result.GetLength ();
    for (size_t i = 0; i < rLen; i++) {
        if (isalpha (result [i].GetAsciiCode ())) {
            result.SetCharAt (TOUPPER (result [i].GetAsciiCode ()), i);
        }
    }
    return (result);
}




/*
 ********************************************************************************
 ************************************* tolower **********************************
 ********************************************************************************
 */
String	tolower (const String& s)
{
    String			result	=	s;
    size_t	rLen	=	result.GetLength ();
    for (size_t i = 0; i < rLen; i++) {
        if (isalpha (result [i].GetAsciiCode ())) {
            result.SetCharAt (TOLOWER (result [i].GetAsciiCode ()), i);
        }
    }
    return (result);
}
