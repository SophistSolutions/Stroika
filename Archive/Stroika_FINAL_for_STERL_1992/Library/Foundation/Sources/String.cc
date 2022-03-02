/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/String.cc,v 1.20 1992/11/16 04:44:27 lewis Exp $
 *
 * TODO:
 *		-		Override operator new for the StringRep classes - at least the frequencely
 *		used ones. First make sure deletes work out OK in this case. Look at ARM, or generated code.
 *
 *
 * Changes:
 *	$Log: String.cc,v $
 *		Revision 1.20  1992/11/16  04:44:27  lewis
 *		Work around qBCC_OpEqualsTemplateNotAutoExpanded.
 *
 *		Revision 1.19  1992/10/16  22:53:58  lewis
 *		Added op==/<=/< overloads for const char*.
 *
 *		Revision 1.18  1992/10/15  02:10:33  lewis
 *		Fix/Hack code in op+= that does memcpy - was array indexing void*
 *		ptr - now cast to char* - much better :-).
 *
 *		Revision 1.17  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.16  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.15  1992/10/09  04:45:50  lewis
 *		Get rid of references to kEmptyString.
 *		Also, refix operator>> (istream,String) - not sure why sterls version
 *		broke with libg++ - but it did! so go back to old one.
 *
 *		Revision 1.14  1992/10/07  23:32:40  sterling
 *		Conditionally add support for switchover to GenClass - and got rid
 *		of old code in istream << routine.
 *
 *		Revision 1.13  1992/09/24  00:39:45  lewis
 *		(LGP checkin for STERL). In ToCString() NUL terminate rather than
 *		char '0' terminate. Also, make IndexOf more efficient (and maybe
 *		changed defn? Not too sure?
 *
 *		Revision 1.12  1992/09/21  06:28:48  sterling
 *		Use RoundUpTo () instead of manually computing it, and use
 *		STERLs version of op<= & op< instead of one LGP had done.
 *
 *		Revision 1.11  1992/09/20  01:14:21  lewis
 *		Fixed <, <= for strings (hopefully - they now pass new testsuite
 *		tests I added :-).
 *
 *		Revision 1.9  1992/09/05  04:55:50  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.8  1992/09/01  15:26:25  sterling
 *		Lots.
 *
 *		Revision 1.7  1992/07/22  02:32:17  lewis
 *		Tried to make the Character a real class, with I strong - temporary
 *		- eye toward backward compatability. Did minimal hacks here to get things
 *		working quickly. Interface is roughly right, but internally we are still
 *		totally char* based.
 *		One hack to fix soon is what we did with ctype...
 *
 *		Revision 1.6  1992/07/16  05:15:54  lewis
 *		Added new method Contains - a shortcut for saying IndexOf () != kBadSequenceIndex.
 *
 *		Revision 1.5  1992/07/08  01:33:31  lewis
 *		Use RequireNotReached () instead of SwitchFallThru.
 *
 *		Revision 1.4  1992/07/07  07:21:08  lewis
 *		Cleanup comments and removed RequireNotNil (rhs.fRep), etc. since broke
 *		BCC and not really necessary anymore - referenounted stuff assures not nil.
 *
 *		Revision 1.3  1992/07/02  03:33:29  lewis
 *		Moved Container class macro Implements for String/Real to
 *		Stroika-Foundation-Globals.hh.
 *
 *		Revision 1.2  1992/07/01  03:48:39  lewis
 *		Renamed Strings.hh String.hh
 *
 *		Revision 1.26  1992/06/09  17:52:29  lewis
 *		ReFixed bug VERY subtle bug Sterl had fixed in his last release - one
 *		with b+=b for Strings. He had just changed arg to op+= to
 *		String (from const String&). I changed hack so more common
 *		cases pay no performance hit, and fixed it totally locally to the function.
 *
 *		Revision 1.23  92/06/09  01:54:04  01:54:04  lewis (Lewis Pringle)
 *		Merged in sterls new smartpointer/referencecounting support, and
 *		modified to also support templates.
 *		
 *		Revision 1.21  92/05/22  23:21:19  23:21:19  lewis (Lewis Pringle)
 *		Fixed bug in my SubString () fix - must map length of kBadSequenceIndex
 *		to GetLength()-from+1.
 *		
 *		Revision 1.20  92/05/21  17:31:07  17:31:07  lewis (Lewis Pringle)
 *		Change String::SubString () to take lenght as second argument,
 *		rather than position. Seems more convienent after having used it
 *		for a while.
 *		
 *		Revision 1.19  92/05/19  14:47:02  14:47:02  lewis (Lewis Pringle)
 *		Fixed String version of IndexOf and RIndexOf () to use SubString ()
 *		properly - actually I dont like how its defined - soon we'll change
 *		definition of second arg to SubString to be a length, which
 *		is more convienient - but must be careful to fix all usages first!!!
 *		
 *		Revision 1.18  92/05/19  11:53:52  11:53:52  lewis (Lewis Pringle)
 *		Changed String iostream extractor to mimic behaviour of char* version. Not a totally safe
 *		implementation, but should be good enuf for now.
 *		
 *		Revision 1.17  92/03/30  14:28:41  14:28:41  lewis (Lewis Pringle)
 *		Wrapped Implement macro calls in #ifndef templates available.
 *		
 *		Revision 1.14  1992/03/06  05:37:56  lewis
 *		Minor cleanups, and worked on commenting things.
 *
 *		Revision 1.13  1992/01/31  16:10:08  sterling
 *		Put new kEmptyString style optimization that should be safe
 *
 *		Revision 1.12  1992/01/22  04:15:02  lewis
 *		Disable kEmptyString optimization in String() ctor 'til we can make it safe when
 *		kEmptyString not yet constructed.
 *
 *		Revision 1.11  1992/01/14  05:56:02  lewis
 *		Added Set_Array (String/Real) and got rid of old stream lib support.
 *
 *		Revision 1.7  1991/12/04  19:26:43  lewis
 *		added ToC/PString w/wo Trunc.
 *
 */



#include	<ctype.h>
#include	<string.h>
#include	<iostream.h>

#include	"Config-Foundation.hh"
#include	"Debug.hh"
#include	"Memory.hh"

#include	"String.hh"





#if 	!qRealTemplatesAvailable
	#include SharedOfStringRep_cc
#endif



#if		qRealTemplatesAvailable && qBCC_OpEqualsTemplateNotAutoExpanded
	extern	Boolean	operator== (const Shared<StringRep>&, const Shared<StringRep>&);
#endif





/*
 ********************************************************************************
 ************************************* String ***********************************
 ********************************************************************************
 */
String::String ():
	fRep (&Clone, Nil)
{
#if		qRealTemplatesAvailable
	static	Shared<StringRep>	sSharedBuffer (&Clone, new StringRep_CharArray (Nil, 0));
#else
	static	Shared(StringRep)	sSharedBuffer (&Clone, new StringRep_CharArray (Nil, 0));
#endif

	fRep = sSharedBuffer;
	Assert (fRep.CountReferences () > 1);
}

String::String (const char* cString) :
	fRep (&Clone, Nil)
{
	RequireNotNil (cString);
	fRep = new StringRep_CharArray (cString, strlen (cString));
}

String::String (StringRep* sharedPart, Boolean):
	fRep (&Clone, sharedPart)
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
	if (index != kBadSequenceIndex) {
		fRep->RemoveAt (index, 1);
	}
}

size_t	String::IndexOf (Character c) const
{
	register	size_t length = GetLength ();
	for (register size_t i = 1; i <= length; i++) {
		if (fRep->GetAt (i) == c) {
			return (i);
		}
	}
	return (kBadSequenceIndex);
}

size_t	String::IndexOf (const String& subString) const
{
// SSW: changed definition here, as former seems really el bizarro
#if 0
	if (subString == kEmptyString) {
		return ((GetLength () == 0) ? kBadSequenceIndex : 1);
	}
#endif
	if (GetLength () < subString.GetLength ()) {
		return (kBadSequenceIndex);	// important test cuz size_t is unsigned
	}
	
	register size_t	subStrLen	=	subString.GetLength ();
	size_t	limit		=	GetLength () - subStrLen;
	for (size_t i = 0; i <= limit; i++) {
		for (register size_t j = 1; j <= subStrLen; j++) {
			if (fRep->GetAt (i+j) != subString.fRep->GetAt (j)) {
				goto nogood;
			}
		}
		return (i+1);
nogood:
		;
	}
	return (kBadSequenceIndex);
}

size_t	String::RIndexOf (Character c) const
{
	register	size_t length = GetLength ();
	for (register size_t i = length; i >= 1; i--) {
		if (fRep->GetAt (i) == c) {
			return (i);
		}
	}
	return (kBadSequenceIndex);
}

size_t	String::RIndexOf (const String& subString) const
{
	/*
	 * Do quickie implementation, and dont worry about efficiency...
	 */
	if (subString.GetLength () == 0) {
		return ((GetLength () == 0) ? kBadSequenceIndex : GetLength ());
	}

	size_t	subStrLen	=	subString.GetLength ();
	size_t	limit		=	GetLength () - subStrLen + 1;
	for (size_t i = limit; i >= 1; i--) {
		if (SubString (i, subStrLen) == subString) {
			return (i);
		}
	}
	return (kBadSequenceIndex);
}

Boolean	String::Contains (Character c) const
{
	return (fRep->Contains (c));
}

Boolean	String::Contains (const String& subString) const
{
	return Boolean (IndexOf (subString) != kBadSequenceIndex);
}

String	String::SubString (size_t from, size_t length) const
{
	Require (from >= 1);
	Require ((length == kBadSequenceIndex) or (length <= (GetLength ()-from+1) and (length >= 0)));

	if (length == 0) {
		return ("");
	}
	if (length == kBadSequenceIndex) {
		length = GetLength () - from + 1;
	}
	return (String (new StringRep_Substring (fRep, from, length), False));
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
	size_t bytesToCopy = Min (GetLength (), bufferSize-1);
	memcpy (buf, Peek (), bytesToCopy);
	buf [bytesToCopy] = '\0';
	return (buf);
}

char*	String::ToCStringTrunc (char buf [], size_t bufferSize) const
{
	size_t	len	= Min (GetLength (), bufferSize);
	len = Min (len, size_t (kMaxUInt8));
	if (len == GetLength ()) {
		return (ToCString (buf, bufferSize));
	}
	else {
		return (SubString (1, len).ToCString (buf, bufferSize));
	}
}

#if		qMacOS
unsigned char*	String::ToPString () const
{
//	size_t	length = (GetLength () * sizeof (Character)) + 1;		// n Characters plus a Nil char???
	size_t	length = (GetLength () * sizeof (char)) + 1;			// n Characters plus a Nil char???
	Assert (length < kMaxUInt8);
	return (ToPString (new unsigned char [length], length));
}

unsigned char*	String::ToPString (unsigned char buf [], size_t bufferSize) const
{
	Require (bufferSize > 0);
	Require (GetLength () < bufferSize);							// better to say so than silently truncate
	size_t bytesToCopy = Min (GetLength (), bufferSize-1);
	Assert (bytesToCopy < kMaxUInt8);
	memcpy (&buf[1], Peek (), bytesToCopy);
	buf [0] = bytesToCopy;
	return (buf);
}

unsigned char*	String::ToPStringTrunc () const
{
//	size_t	length = (GetLength () * sizeof (Character)) + 1;		// n Characters plus a Nil char???
	size_t	length = (GetLength () * sizeof (char)) + 1;			// n Characters plus a Nil char???
	length = Min (UInt8 (length), kMaxUInt8);								// shorten so fits in pstring
	return (ToPStringTrunc (new unsigned char [length], length));
}

unsigned char*	String::ToPStringTrunc (unsigned char buf [], size_t bufferSize) const
{
	size_t	len	= Min (GetLength (), bufferSize);
	len = Min (UInt8 (len), kMaxUInt8);
	if (len == GetLength ()) {
		return (ToPString (buf, bufferSize));
	}
	else {
		return (SubString (1, len).ToPString (buf, bufferSize));
	}
}
#endif	/*qMacOS*/











/*
 ********************************************************************************
 ******************************** String_CharArray ******************************
 ********************************************************************************
 */
String_CharArray::String_CharArray () :
	String (Nil, False)
{
	SetRep (new StringRep_CharArray (Nil, 0));
}

String_CharArray::String_CharArray (const char* cString) :
	String (Nil, False)
{
	size_t length = strlen (cString);
	SetRep (new StringRep_CharArray (cString, length));
}

String_CharArray::String_CharArray (const void* arrayOfBytes, size_t nBytes) :
	String (Nil, False)
{
	SetRep (new StringRep_CharArray (arrayOfBytes, nBytes));
}

String_CharArray::String_CharArray (const String& from) :
	String (Nil, False)
{
	SetRep (new StringRep_CharArray (from.Peek (), from.GetLength ()));
}





/*
 ********************************************************************************
 **************************** String_BufferedCharArray **************************
 ********************************************************************************
 */
String_BufferedCharArray::String_BufferedCharArray () :
	String (Nil, False)
{
	SetRep (new StringRep_BufferedCharArray (Nil, 0));
}

String_BufferedCharArray::String_BufferedCharArray (const char* cString) :
	String (Nil, False)
{
	size_t length = strlen (cString);
	SetRep (new StringRep_BufferedCharArray (cString, length));
}

String_BufferedCharArray::String_BufferedCharArray (const void* arrayOfBytes, size_t nBytes) :
	String (Nil, False)
{
	SetRep (new StringRep_BufferedCharArray (arrayOfBytes, nBytes));
}

String_BufferedCharArray::String_BufferedCharArray (const String& from) :
	String (Nil, False)
{
	SetRep (new StringRep_BufferedCharArray (from.Peek (), from.GetLength ()));
}




/*
 ********************************************************************************
 ****************************** String_ReadOnlyChar *****************************
 ********************************************************************************
 */
String_ReadOnlyChar::String_ReadOnlyChar (const char* cString) :
	String (Nil, False)
{
	size_t length = strlen (cString);
	SetRep (new StringRep_ReadOnlyChar (cString, length));
}

String_ReadOnlyChar::String_ReadOnlyChar (const void* arrayOfBytes, size_t nBytes) :
	String (Nil, False)
{
	SetRep (new StringRep_ReadOnlyChar (arrayOfBytes, nBytes));
}




/*
 ********************************************************************************
 *********************************** StringRep **********************************
 ********************************************************************************
 */
StringRep::~StringRep ()
{
}






/*
 ********************************************************************************
 ******************************** StringRep_CharArray ***************************
 ********************************************************************************
 */
StringRep_CharArray::StringRep_CharArray (const void* arrayOfBytes, size_t nBytes):
	StringRep (),
	fStorage (Nil),
	fLength (0)
{
	fLength = nBytes;
	fStorage = ::new char [CalcAllocSize (fLength)];
	if (arrayOfBytes != Nil) {
		memcpy (fStorage, arrayOfBytes, fLength);
	}
}

StringRep_CharArray::StringRep_CharArray () :
	fLength (0),
	fStorage (Nil)
{
}

StringRep_CharArray::~StringRep_CharArray ()
{
	delete fStorage;
}

StringRep*	StringRep_CharArray::Clone () const
{
	return (new StringRep_CharArray (fStorage, fLength));
}

size_t	StringRep_CharArray::GetLength () const
{
	return (fLength);
}

Boolean	StringRep_CharArray::Contains (Character item) const
{
	AssertNotNil (fStorage);

	char asciiItem = item.GetAsciiCode ();
	for (size_t i = 0; i < fLength; i++) {
		if (fStorage[i] == asciiItem) {
			return (True);
		}
	}
	return (False);
}

void	StringRep_CharArray::RemoveAll ()
{
	SetLength (0);
}

Character	StringRep_CharArray::GetAt (size_t index) const
{
	Require (index >= 1);
	Require (index <= GetLength ());
	AssertNotNil (fStorage);
	
	return (fStorage[index-1]);
}

void	StringRep_CharArray::SetAt (Character item, size_t index)
{
	Require (index >= 1);
	Require (index <= GetLength ());
	AssertNotNil (fStorage);
	
	fStorage[index-1] = item.GetAsciiCode ();
}

void	StringRep_CharArray::InsertAt (Character item, size_t index)
{
	Require (index >= 1);
	Require (index <= GetLength ()+1);

	SetLength (GetLength () + 1);
	if (index < fLength) {
		register	char*	lhs	=	&fStorage [fLength-1];
		register	char*	rhs	=	&fStorage [fLength-2];
		for (register size_t i = fLength-1; i >= index; i--) {
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
		register	char*	lhs	=	&fStorage [index-1];
		register	char*	rhs	=	&fStorage [index+amountToRemove-1];
		for (register size_t i = fLength - index - amountToRemove + 1; i > 0; i--) {
			*lhs++ = *rhs++;
		}
	}
	fLength -= amountToRemove;
}

void	StringRep_CharArray::SetLength (size_t newLength)
{
	AssertNotNil (fStorage);

	size_t	oldAllocSize	=	CalcAllocSize (fLength);
	size_t	newAllocSize	=	CalcAllocSize (newLength);

	if (oldAllocSize != newAllocSize) {
		Assert (newAllocSize >= newLength);
		fStorage = (char*)ReAlloc (fStorage, newAllocSize);
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
StringRep_BufferedCharArray::StringRep_BufferedCharArray (const void* arrayOfBytes, size_t nBytes) :
	StringRep_CharArray ()
{
	RequireNotNil (arrayOfBytes);

	char*	storage = ::new char [CalcAllocSize (nBytes)];
	memcpy (storage, arrayOfBytes, nBytes);
	SetStorage (storage, nBytes);
}

StringRep*	StringRep_BufferedCharArray::Clone () const
{
	return (new StringRep_BufferedCharArray (Peek (), GetLength ()));
}

size_t	StringRep_BufferedCharArray::CalcAllocSize (size_t requested)
{
	// round up to buffer block size
	return (RoundUpTo (requested, 32));
}




/*
 ********************************************************************************
 ****************************** StringRep_ReadOnlyChar **************************
 ********************************************************************************
 */
StringRep_ReadOnlyChar::StringRep_ReadOnlyChar (const void* arrayOfBytes, size_t nBytes) :
	StringRep_CharArray (),
	fAllocedMem (False)
{
	RequireNotNil (arrayOfBytes);
	SetStorage ((char*)arrayOfBytes, nBytes);
}

StringRep_ReadOnlyChar::~StringRep_ReadOnlyChar ()
{
	if (not fAllocedMem) {
		SetStorage (Nil, 0);	// make sure that memory is not deleted, as we never alloced
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
		fAllocedMem = True;
	}
	
	Ensure (fAllocedMem);
}




/*
 ********************************************************************************
 ******************************** StringRep_Substring ***************************
 ********************************************************************************
 */

#if		qRealTemplatesAvailable
	StringRep_Substring::StringRep_Substring (const Shared<StringRep>& baseString, size_t from, size_t length) :
#else
	StringRep_Substring::StringRep_Substring (const Shared(StringRep)& baseString, size_t from, size_t length) :
#endif
	fBase (baseString),
	fFrom (from),
	fLength (length)
{
	Require (from >= 1);
	Require (length >= 0);
	Require ((from + length - 1) <= fBase->GetLength ());
}

StringRep*	StringRep_Substring::Clone () const
{
	return (new StringRep_CharArray (Peek (), GetLength ()));
}

size_t	StringRep_Substring::GetLength () const
{
	return (fLength);
}

Boolean	StringRep_Substring::Contains (Character item) const
{
	for (size_t i = 1; i <= fLength; i++) {
		if (GetAt (i) == item) {
			return (True);
		}
	}
	return (False);
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

StringRep_Catenate::StringRep_Catenate (const String& lhs, const String& rhs) :
	fLeft (lhs),
	fRight (rhs),
	fLength (lhs.GetLength () + rhs.GetLength ())
{
}

StringRep*	StringRep_Catenate::Clone () const
{
	StringRep_CharArray*	s	=	new StringRep_CharArray (Nil, GetLength ());
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

Boolean	StringRep_Catenate::Contains (Character item) const
{
	return (Boolean ((fLeft.Contains (item)) or (fRight.Contains (item))));
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
		String_CharArray newL	=	String_CharArray (Nil, GetLength ());
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
String	operator+ (const String& lhs, const String& rhs)
{
#if 0
	size_t	lhsLen	=	lhs.GetLength ();
	size_t	rhsLen	=	rhs.GetLength ();
	String	s	=	String (Nil, lhsLen + rhsLen);			// garbage string of right size
	memcpy (&((char*)s.Peek ())[0], lhs.Peek (), size_t (lhsLen));
	memcpy (&((char*)s.Peek ())[lhsLen], rhs.Peek (), size_t (rhsLen));
	return (s);
#else
	return (String (new StringRep_Catenate (lhs, rhs), False));
#endif
}




/*
 ********************************************************************************
 ************************************* operator== *******************************
 ********************************************************************************
 */
Boolean	operator== (const String& lhs, const String& rhs)
{
	if (lhs.fRep == rhs.fRep) {
		return (True);
	}
	size_t length = lhs.GetLength ();
	
	if (length != rhs.GetLength ()) {
		return (False);
	}
	
	for (register int i = 1; i <= length; i++) {
		if (lhs[i] != rhs[i]) {
			return (False);
		}
	}
	return (True);
}

Boolean	operator== (const char* lhs, const String& rhs)
{
	RequireNotNil (lhs);
	size_t length = ::strlen (lhs);
	
	if (length != rhs.GetLength ()) {
		return (False);
	}
	
	for (register int i = 1; i <= length; i++) {
		if (lhs[i-1] != rhs[i]) {
			return (False);
		}
	}
	return (True);
}

Boolean	operator== (const String& lhs, const char* rhs)
{
	RequireNotNil (rhs);
	size_t length = lhs.GetLength ();

	if (length != ::strlen (rhs)) {
		return (False);
	}

	for (register int i = 1; i <= length; i++) {
		if (lhs[i] != rhs[i-1]) {
			return (False);
		}
	}
	return (True);
}




/*
 ********************************************************************************
 ************************************* operator< ********************************
 ********************************************************************************
 */
Boolean	operator< (const String& lhs, const String& rhs)
{
	if (lhs.fRep == rhs.fRep) {
		return (False);
	}
	size_t length = Min (lhs.GetLength (), rhs.GetLength ());
	for (register int i = 1; i <= length; i++) {
		if (lhs[i] != rhs[i]) {
			return (Boolean (lhs[i] < rhs[i]));
		}
	}
	return (Boolean (rhs.GetLength () != length));	// implies lhs.GetLength () < rhs.GetLength
}

Boolean	operator< (const char* lhs, const String& rhs)
{
	RequireNotNil (lhs);
	return (String (lhs) < rhs);
}

Boolean	operator< (const String& lhs, const char* rhs)
{
	RequireNotNil (rhs);
	return (lhs < String (rhs));
}




/*
 ********************************************************************************
 ************************************* operator<= *******************************
 ********************************************************************************
 */
Boolean	operator<= (const String& lhs, const String& rhs)
{
	if (lhs.fRep == rhs.fRep) {
		return (True);
	}
	size_t length = Min (lhs.GetLength (), rhs.GetLength ());
	for (register int i = 1; i <= length; i++) {
		if (lhs[i] != rhs[i]) {
			return (Boolean (lhs[i] < rhs[i]));
		}
	}
	return (Boolean (lhs.GetLength () == length)); 	// implies lhs.GetLength () <= rhs.GetLength
}

Boolean	operator<= (const char* lhs, const String& rhs)
{
	RequireNotNil (lhs);
	return (String (lhs) <= rhs);
}

Boolean	operator<= (const String& lhs, const char* rhs)
{
	RequireNotNil (rhs);
	RequireNotNil (rhs);
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
	int oldWidth = in.width (sizeof (bigBuf)-1);	// prevent overflow (I hope thats what this does??)
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
	for (register size_t i = 1; i <= rLen; i++) {
		if (isalpha (result [i].GetAsciiCode ())) {
			result.SetCharAt (toupper (result [i].GetAsciiCode ()), i);
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
	for (register size_t i = 1; i <= rLen; i++) {
		if (isalpha (result [i].GetAsciiCode ())) {
			result.SetCharAt (tolower (result [i].GetAsciiCode ()), i);
		}
	}
	return (result);
}





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

