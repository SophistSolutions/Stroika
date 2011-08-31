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

String::String (const char* cString)
	: fRep (&Clone_, nullptr)
{
	RequireNotNull (cString);
	fRep = new StringRep_CharArray (cString, strlen (cString));
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
#if		qDebug
		size_t	oldAppendLength = appendage.GetLength ();
#endif
		size_t	oldLength	=	GetLength ();
		size_t	appendLength = appendage.GetLength ();
		SetLength (oldLength + appendLength);
		Assert (appendage.GetLength () == appendLength);
		memcpy (&((char*)fRep->Peek ())[oldLength], appendage.fRep->Peek (), size_t (appendLength));
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
	Require (i >= 1);
	Require (i <= GetLength ());
	fRep->SetAt (c, i);
}

void	String::InsertAt (Character c, size_t i)
{
	Require (i >= 1);
	Require (i <= (GetLength ()+1));
	fRep->InsertAt (c, i);
}

void	String::RemoveAt (size_t i, size_t amountToRemove)
{
	Require (i >= 1);
	Require ((i+amountToRemove-1) <= (GetLength ()));
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
	for (size_t i = 1; i <= length; i++) {
		if (fRep->GetAt (i) == c) {
			return (i);
		}
	}
	return (kBadStringIndex);
}

size_t	String::IndexOf (const String& subString) const
{
	if (subString.GetLength () == 0) {
		return ((GetLength () == 0) ? kBadStringIndex : 1);
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
		return (i+1);
nogood:
		;
	}
	return (kBadStringIndex);
}

size_t	String::RIndexOf (Character c) const
{
	size_t length = GetLength ();
	for (size_t i = length; i >= 1; i--) {
		if (fRep->GetAt (i) == c) {
			return (i);
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
		return ((GetLength () == 0) ? kBadStringIndex : GetLength ());
	}

	size_t	subStrLen	=	subString.GetLength ();
	size_t	limit		=	GetLength () - subStrLen + 1;
	for (size_t i = limit; i >= 1; i--) {
		if (SubString (i, subStrLen) == subString) {
			return (i);
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
	Require (from >= 1);
	Require ((length == kBadStringIndex) or (length <= (GetLength ()-from+1) and (length >= 0)));

	if (length == 0) {
		return ("");
	}
	if (length == kBadStringIndex) {
		length = GetLength () - from + 1;
	}
	return (String (new StringRep_Substring (fRep, from, length), false));
}

char*	String::ToCString () const
{
//	size_t	length = (GetLength () * sizeof (Character)) + 1;		// n Characters plus a length byte
	size_t	length = (GetLength () * sizeof (char)) + 1;			// n Characters plus a length byte
	return (ToCString (new char [length], length));
}

char*	String::ToCString (char buf [], size_t bufferSize) const
{
	Require (bufferSize > 0);
	size_t bytesToCopy = min (GetLength (), bufferSize-1);
	memcpy (buf, Peek (), bytesToCopy);
	buf [bytesToCopy] = '\0';
	return (buf);
}

char*	String::ToCStringTrunc (char buf [], size_t bufferSize) const
{
	size_t	len	= min (GetLength (), bufferSize);
	len = min (len, size_t (UCHAR_MAX));
	if (len == GetLength ()) {
		return (ToCString (buf, bufferSize));
	}
	else {
		return (SubString (1, len).ToCString (buf, bufferSize));
	}
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

String_CharArray::String_CharArray (const char* cString)
	: String (nullptr, false)
{
	size_t length = strlen (cString);
	SetRep (new StringRep_CharArray (cString, length));
}

String_CharArray::String_CharArray (const void* arrayOfBytes, size_t nBytes)
	: String (nullptr, false)
{
	SetRep (new StringRep_CharArray (arrayOfBytes, nBytes));
}

String_CharArray::String_CharArray (const String& from)
	: String (nullptr, false)
{
	SetRep (new StringRep_CharArray (from.Peek (), from.GetLength ()));
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

String_BufferedCharArray::String_BufferedCharArray (const char* cString)
	: String (nullptr, false)
{
	size_t length = strlen (cString);
	SetRep (new StringRep_BufferedCharArray (cString, length));
}

String_BufferedCharArray::String_BufferedCharArray (const void* arrayOfBytes, size_t nBytes)
	: String (nullptr, false)
{
	SetRep (new StringRep_BufferedCharArray (arrayOfBytes, nBytes));
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
String_ReadOnlyChar::String_ReadOnlyChar (const char* cString)
	: String (nullptr, false)
{
	size_t length = strlen (cString);
	SetRep (new StringRep_ReadOnlyChar (cString, length));
}

String_ReadOnlyChar::String_ReadOnlyChar (const void* arrayOfBytes, size_t nBytes)
	: String (nullptr, false)
{
	SetRep (new StringRep_ReadOnlyChar (arrayOfBytes, nBytes));
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
StringRep_CharArray::StringRep_CharArray (const void* arrayOfBytes, size_t nBytes)
	: StringRep ()
	, fStorage (nullptr)
	, fLength (0)
{
	fLength = nBytes;
	fStorage = ::new char [CalcAllocSize (fLength)];
	if (arrayOfBytes != nullptr) {
		memcpy (fStorage, arrayOfBytes, fLength);
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
	return (new StringRep_CharArray (fStorage, fLength));
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
	Require (index >= 1);
	Require (index <= GetLength ());
	AssertNotNull (fStorage);

	return (fStorage[index-1]);
}

void	StringRep_CharArray::SetAt (Character item, size_t index)
{
	Require (index >= 1);
	Require (index <= GetLength ());
	AssertNotNull (fStorage);

	fStorage[index-1] = item.GetAsciiCode ();
}

void	StringRep_CharArray::InsertAt (Character item, size_t index)
{
	Require (index >= 1);
	Require (index <= GetLength ()+1);

	SetLength (GetLength () + 1);
	if (index < fLength) {
		char*	lhs	=	&fStorage [fLength-1];
		char*	rhs	=	&fStorage [fLength-2];
		size_t i = fLength-1;
		for (; i >= index; i--) {
			*lhs-- = *rhs--;
		}
		Assert (i == index-1);
		Assert (lhs == &fStorage [index-1]);
	}
	fStorage[index-1] = item.GetAsciiCode ();
}

void	StringRep_CharArray::RemoveAt (size_t index, size_t amountToRemove)
{
	Require (index >= 1);
	Require (index <= fLength);

	if (index < fLength) {
		char*	lhs	=	&fStorage [index-1];
		char*	rhs	=	&fStorage [index+amountToRemove-1];
		for (size_t i = fLength - index - amountToRemove + 1; i > 0; i--) {
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
		fStorage = (char*)realloc (fStorage, newAllocSize);
	}
	fLength = newLength;
	Ensure (fLength == newLength);
}

const void*	StringRep_CharArray::Peek () const
{
	return (fStorage);
}

/* Deliberately does not try to free up old fStorage, as this could be other than heap memory */
void	StringRep_CharArray::SetStorage (char* storage, size_t length)
{
	fStorage = storage;
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
StringRep_BufferedCharArray::StringRep_BufferedCharArray (const void* arrayOfBytes, size_t nBytes)
	: StringRep_CharArray ()
{
	RequireNotNull (arrayOfBytes);

	char*	storage = ::new char [CalcAllocSize (nBytes)];
	memcpy (storage, arrayOfBytes, nBytes);
	SetStorage (storage, nBytes);
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
 ****************************** StringRep_ReadOnlyChar **************************
 ********************************************************************************
 */
StringRep_ReadOnlyChar::StringRep_ReadOnlyChar (const void* arrayOfBytes, size_t nBytes)
	: StringRep_CharArray ()
	, fAllocedMem (false)
{
	RequireNotNull (arrayOfBytes);
	SetStorage ((char*)arrayOfBytes, nBytes);
}

StringRep_ReadOnlyChar::~StringRep_ReadOnlyChar ()
{
	if (not fAllocedMem) {
		SetStorage (nullptr, 0);	// make sure that memory is not deleted, as we never alloced
	}
}

void	StringRep_ReadOnlyChar::RemoveAll ()
{
	AssureMemAllocated ();
	StringRep_CharArray::RemoveAll ();
}

void	StringRep_ReadOnlyChar::SetAt (Character item, size_t index)
{
	AssureMemAllocated ();
	StringRep_CharArray::SetAt (item, index);
}

void	StringRep_ReadOnlyChar::InsertAt (Character item, size_t index)
{
	AssureMemAllocated ();
	StringRep_CharArray::InsertAt (item, index);
}

void	StringRep_ReadOnlyChar::RemoveAt (size_t index, size_t amountToRemove)
{
	AssureMemAllocated ();
	StringRep_CharArray::RemoveAt (index, amountToRemove);
}

void	StringRep_ReadOnlyChar::SetLength (size_t newLength)
{
	AssureMemAllocated ();
	StringRep_CharArray::SetLength (newLength);
}

void	StringRep_ReadOnlyChar::AssureMemAllocated ()
{
	if (not fAllocedMem) {
		SetStorage (::new char [GetLength ()], GetLength ());
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
	Require (from >= 1);
	Require (length >= 0);
	Require ((from + length - 1) <= fBase->GetLength ());
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
	for (size_t i = 1; i <= fLength; i++) {
		if (GetAt (i) == item) {
			return (true);
		}
	}
	return (false);
}

void	StringRep_Substring::RemoveAll ()
{
	fBase->RemoveAll ();
	fFrom = 1;
	fLength = 0;
}

Character	StringRep_Substring::GetAt (size_t index) const
{
	Require (index <= GetLength ());
	Assert ((fFrom+index-1) <= fBase->GetLength ());
	return (fBase->GetAt (fFrom+index-1));
}

void	StringRep_Substring::SetAt (Character item, size_t index)
{
	Require (index <= GetLength ());
	Assert ((fFrom+index-1) <= fBase->GetLength ());
	fBase->SetAt (item, (fFrom+index-1));
}

void	StringRep_Substring::InsertAt (Character item, size_t index)
{
	Require (index <= (GetLength ()+1));
	Assert ((fFrom+index-1) <= (fBase->GetLength ()+1));
	fBase->InsertAt (item, (fFrom+index-1));
	fLength++;
}

void	StringRep_Substring::RemoveAt (size_t index, size_t amountToRemove)
{
	Require (index <= GetLength ());
	Require ((index+amountToRemove) <= GetLength ());

	Assert ((fFrom+index-1) <= fBase->GetLength ());
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

const void*	StringRep_Substring::Peek () const
{
	Assert (fFrom <= fBase->GetLength ());

	const char* buffer = (const char*) fBase->Peek ();
	return (&buffer[fFrom-1]);
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
	size_t lhsLength = fLeft.GetLength ();
	memcpy (&((char*)s->Peek ())[0], fLeft.Peek (), size_t (lhsLength));
	memcpy (&((char*)s->Peek ())[lhsLength], fRight.Peek (), size_t (fRight.GetLength ()));
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
	fLeft = "";
	fRight = "";
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

const void*	StringRep_Catenate::Peek () const
{
	// ~const
	((StringRep_Catenate*) this)->Normalize ();
	return (fLeft.Peek ());
}


void	StringRep_Catenate::Normalize ()
{
	if (fRight != "") {
		String_CharArray newL	=	String_CharArray (nullptr, GetLength ());
		size_t lhsLength = fLeft.GetLength ();
		memcpy (&((char*)newL.Peek ())[0], fLeft.Peek (), size_t (lhsLength));
		memcpy (&((char*)newL.Peek ())[lhsLength], fRight.Peek (), size_t (fRight.GetLength ()));

		fLeft = newL;
		fRight = "";
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

    for (size_t i = 1; i <= length; i++) {
        if (lhs[i] != rhs[i]) {
            return (false);
        }
    }
    return (true);
}

bool	Stroika::Foundation::Characters::operator== (const char* lhs, const String& rhs)
{
    RequireNotNull (lhs);
    size_t length = ::strlen (lhs);

    if (length != rhs.GetLength ()) {
        return (false);
    }

    for (size_t i = 1; i <= length; i++) {
        if (Character(lhs[i-1]) != rhs[i]) {
            return (false);
        }
    }
    return (true);
}

bool	Stroika::Foundation::Characters::operator== (const String& lhs, const char* rhs)
{
    RequireNotNull (rhs);
    size_t length = lhs.GetLength ();

    if (length != ::strlen (rhs)) {
        return (false);
    }

    for (size_t i = 1; i <= length; i++) {
        if (lhs[i] != Character (rhs[i-1])) {
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
    for (size_t i = 1; i <= length; i++) {
        if (lhs[i] != rhs[i]) {
            return (bool (lhs[i] < rhs[i]));
        }
    }
    return (bool (rhs.GetLength () != length));	// implies lhs.GetLength () < rhs.GetLength
}

bool	Stroika::Foundation::Characters::operator< (const char* lhs, const String& rhs)
{
    RequireNotNull (lhs);
    return (String (lhs) < rhs);
}

bool	Stroika::Foundation::Characters::operator< (const String& lhs, const char* rhs)
{
    RequireNotNull (rhs);
    return (lhs < String (rhs));
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
    for (size_t i = 1; i <= length; i++) {
        if (lhs[i] != rhs[i]) {
            return (bool (lhs[i] < rhs[i]));
        }
    }
    return (bool (lhs.GetLength () == length)); 	// implies lhs.GetLength () <= rhs.GetLength
}

bool	Stroika::Foundation::Characters::operator<= (const char* lhs, const String& rhs)
{
    RequireNotNull (lhs);
    return (String (lhs) <= rhs);
}

bool	Stroika::Foundation::Characters::operator<= (const String& lhs, const char* rhs)
{
    RequireNotNull (rhs);
    RequireNotNull (rhs);
    return (lhs <= String (rhs));
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
istream&	operator>> (istream& in, String& s)
{
    s = "";
    char	bigBuf [2048];
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
    for (size_t i = 1; i <= rLen; i++) {
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
    for (size_t i = 1; i <= rLen; i++) {
        if (isalpha (result [i].GetAsciiCode ())) {
            result.SetCharAt (TOLOWER (result [i].GetAsciiCode ()), i);
        }
    }
    return (result);
}

