/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Integer.cc,v 1.11 1992/10/29 15:53:58 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Integer.cc,v $
 *		Revision 1.11  1992/10/29  15:53:58  lewis
 *		#if whole thing out -fixup later - not part of first stroika foundation release.
 *
 *		Revision 1.10  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.8  1992/09/05  04:55:50  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.7  1992/09/01  15:26:25  sterling
 *		Lots.
 *
 *		Revision 1.6  1992/07/02  03:23:48  lewis
 *		Added #include Memory.hh - needed cuz we removed other includes elsehere that were implicitly
 *		included here.
 *
 *		Revision 1.5  1992/07/01  01:42:15  lewis
 *		Got rid of qNestedTypesAvailable flag - assume always true.
 *
 *		Revision 1.4  1992/06/25  12:14:15  lewis
 *		Comment out the file in the case where we have no templates available, nested types, and
 *		no CFront half assed nested types. Could get it to work in this case, but difficult, and not
 *		worth doing right now.
 *
 *		Revision 1.3  1992/06/25  02:11:34  lewis
 *		Started adding ifdefs to take into account that CFronts definition of nested types is broken,
 *		and gcc's is right - I think.
 *
 *		Revision 1.2  1992/06/20  19:32:08  lewis
 *		Fix nested type name usage mandated by gcc - with Integer::DigitIndex - I think there are alot
 *		more like this in here...
 *
 *		Revision 1.13  1992/06/09  17:51:59  lewis
 *		re-ordered things so inlines got expanded properly.
 *
 *		Revision 1.12  92/06/09  12:36:57  12:36:57  lewis (Lewis Pringle)
 *		Renamed SmartPointer back to Shared.
 *		
 *		Revision 1.11  92/06/09  01:33:36  01:33:36  lewis (Lewis Pringle)
 *		Merged in Sterls changes to take into account his new smartpointer implementation.
 *		
 *		Revision 1.10  1992/05/23  00:12:59  lewis
 *		Use "BlockAllocated.hh" isntead of  "Memory.hh".
 *
 *		Revision 1.7  1992/03/11  23:08:14  lewis
 *		Got rid of qOldStreamLibOnly support.
 *
 *
 */



#include	<ctype.h>

#include	"BlockAllocated.hh"
#include	"Debug.hh"
#include	"Math.hh"
#include	"Memory.hh"
#include	"StreamUtils.hh"

#include	"Integer.hh"


// Temporarily disable whole thing since not part of Stroika Foundation first release...
#if		0


#if		qCFront_NestedTypesHalfAssed || qRealTemplatesAvailable



const	size_t			kBitsPerChar		=	8;
#if		!qHPCPlus_NestedClassWithVirtMethSubclassingBug && !qCFront_NestedTypesHalfAssed
const	size_t					kRightShift			=	sizeof (Integer::DigitIndex)*kBitsPerChar;
const	Integer::BigDigitType	kBase				=	Integer::BigDigitType (1) << kRightShift;
const	Integer::BigDigitType	kLowerMask			=	kBase - 1;
const	int						kDecimalDigits		=	9; // Decimal Digits in one BigDigitType
const	Integer::BigDigitType	kBigDigitDecimal	=	1000000000; // Largest power of ten which fits in BigDigitType
#else
const	size_t			kRightShift			=	sizeof (DigitIndex)*kBitsPerChar;
const	BigDigitType	kBase				=	BigDigitType(1) << kRightShift;
const	BigDigitType	kLowerMask			=	kBase - 1;
const	int				kDecimalDigits		=	9; // Decimal Digits in one BigDigitType
const	BigDigitType	kBigDigitDecimal	=	1000000000; // Largest power of ten which fits in BigDigitType
#endif

// Assert (kBigDigitDecimal == pow(10, kDecimalDigits));







/*
 ********************************************************************************
 ******************************** IntegerRep *****************************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable
Declare (BlockAllocated, IntegerRep);
Implement (BlockAllocated, IntegerRep);
#endif

void*	IntegerRep::operator new (size_t n)
{
#if		qRealTemplatesAvailable
	return (BlockAllocated<IntegerRep>::operator new (n));
#else	/*qRealTemplatesAvailable*/
	return (BlockAllocated(IntegerRep)::operator new (n));
#endif	/*qRealTemplatesAvailable*/
}

void	IntegerRep::operator delete (void* p)
{
#if		qRealTemplatesAvailable
	BlockAllocated<IntegerRep>::operator delete (p);
#else	/*qRealTemplatesAvailable*/
	BlockAllocated(IntegerRep)::operator delete (p);
#endif	/*qRealTemplatesAvailable*/
}

inline	IntegerRep::IntegerRep (DigitIndex words):
	fLength (words),
	fStorage (Nil)
{
	fStorage = ::new DigitType [words];
}

inline	IntegerRep::~IntegerRep ()
{
	delete (fStorage);
}





#if		!qRealTemplatesAvailable
Implement (Shared, IntegerRep);
#endif


/*
 ********************************************************************************
 ************************************ Integer ***********************************
 ********************************************************************************
 */

static Integer kZero = 0;
static Integer kOne = 1;

Integer::Integer ():
	fRep (&Clone, Nil),
	fSign (0)
{
	fRep = new IntegerRep (0); // Perhaps this should be 1?
}

Integer::Integer (char from):
	fRep (&Clone, Nil),
	fSign (1)
{
	if (from >= 0) {
		MakeInteger (from);
	}
	else {
		MakeInteger (-from);
		Sign () = -1;
	}
}

Integer::Integer (unsigned char from):
	fRep (&Clone, Nil),
	fSign (1)
{
	MakeInteger (from);
}

Integer::Integer (short from):
	fRep (&Clone, Nil),
	fSign (1)
{
	if (from >= 0) {
		MakeInteger (from);
	}
	else {
		MakeInteger (-from);
		Sign () = -1;
	}
}

Integer::Integer (unsigned short from):
	fRep (&Clone, Nil),
	fSign (1)
{
	MakeInteger (from);
}

Integer::Integer (int from):
	fRep (&Clone, Nil),
	fSign (1)
{
	if (from >= 0) {
		MakeInteger (from);
	}
	else {
		MakeInteger (-from);
		Sign () = -1;
	}
}

Integer::Integer (unsigned int from):
	fRep (&Clone, Nil),
	fSign (1)
{
	MakeInteger (from);
}

Integer::Integer (long from):
	fRep (&Clone, Nil),
	fSign (1)
{
	if (from >= 0) {
		MakeInteger (from);
	}
	else {
		MakeInteger (-from);
		Sign () = -1;
	}
}

Integer::Integer (unsigned long from):
	fRep (&Clone, Nil),
	fSign (1)
{
	MakeInteger (from);
}

Integer::Integer (Real from):
	fRep (&Clone, Nil),
	fSign (1)
{
	int sign = 1;

	from = ::floor (from + 0.5) ; /* Round to the nearest integer. */
	if (from < 0) {
		sign = -1;
		from = -from;
	}

	DigitIndex size = DigitIndex (ceil (log (from)/log (kBase))); /* Size in DigitType units */
	fRep = new IntegerRep (size);
	from /= pow (kBase, size); /* Mantissa */
	for (DigitIndex i = size ; i >= 1 ; i--) {
		from *= kBase;
		DigitType k = DigitType (floor (from));
		Digit(i) = k;
		from -= k;
		/* As we exceed the precision of the floating point number, we
		   get basically random values. However who is to say that is
		   wrong. If we are lucky, in fact, from will end up zero.
		 */
	}
	Sign() = sign;
}

Integer::Integer (const Integer& from):
	fRep (from.fRep),
	fSign (from.fSign)
{
}

Integer::~Integer ()
{
}

Integer&	Integer::operator= (const Integer& newInteger)
{
	fRep = newInteger.fRep;
	Sign() = newInteger.GetSign();
	return (*this);
}

Integer	Integer::operator- () const
{
	/*
	 * Cheap!!
	 *
	 * Note: putting the sign into this, rather than the shared part was a major design decision,
	 * because we wante negation to be cheap (not cause a breakreferences). This means that
	 * we really share "Natural Numbers". This decision however, makes copying almost twice as
	 * expensive!!!
	 */
	Integer result = *this; 
	result.Sign () *= -1;
	return (result);
}

Integer&	Integer::operator+= (Integer rhs)
{
	/*
	 * Handle case where either side is zero.
	 */
    if (GetSign () == 0) {
		*this = rhs;
		return (*this);
	}
	if (rhs.GetSign () == 0) {
		return (*this);
	}

	/*
	 * First we have a front. We only really add when we have the same sign.
	 * Otherwise use negation and subtraction.
	 */
	if (GetSign () != rhs.GetSign ()) {
		if (GetSign () == 1) {
			Integer	that = -rhs;
			if (*this > that) { // Compare absolute value.
				*this -= that;
			}
			else {
				*this = -(that-*this);
			}
		}
		else {
			Integer that = -*this;
			if (that > rhs) {
				*this = -(that-rhs);
			}
			else {
				*this = rhs-that;
			}
		}
		return (*this);
	}
	
	if (rhs.GetLength() > GetLength()) return (*this = rhs + *this); // Is this dumb?

	/*
	 * At this point, we must break off any references, and really do any addition on our sole
	 * copy.
	 */
	Assert (GetSign () == rhs.GetSign ());
	Assert (GetLength() >= rhs.GetLength());
	
	ReSize (1 + GetLength ());		// Room for possible carry

	BigDigitType k = 0;
	for (DigitIndex i = 1, n = rhs.GetLength() ; i <= n ; i++) {
	    BigDigitType j = k + BigDigitType (GetDigit (i)) + BigDigitType (rhs.GetDigit (i));
		Digit (i) = DigitType(j&kLowerMask);
		k = j>>kRightShift;
	}

	while (k != 0) {
	    BigDigitType j = k + BigDigitType (GetDigit (i));
		Digit (i) = DigitType(j&kLowerMask);
		k = j>>kRightShift;
		i++;
	}
	
	Assert (i <= GetLength() + 2); // At worst one final carry.
	
	if (i != GetLength() + 1) Length()--;

	TrimTrailingZeros (); // Why? 
	return (*this);
}

Integer&	Integer::operator-= (Integer rhs)
{
    if (GetSign () == 0) {
		*this = -rhs;
		return (*this);
	}
	if (rhs.GetSign () == 0) {
		return (*this);
	}


	/*
	 * First we have a front. We only really subtract when we have the same signs.
	 * Otherwise use negation and addition.
	 */
	if (GetSign () != rhs.GetSign ()) {
		if (GetSign () == 1) {
			Integer	that = -rhs;
			*this += that;
		}
		else {
			Integer that = -*this;
			*this = -(that + rhs);
		}
		return (*this);
	}
	
	Assert (GetSign () == rhs.GetSign ());
	/*
	 * We want to be sure that the left hand side is larger in
	 * absolute value.
	 */
	 
	if (GetSign () == 1 && *this < rhs) {
		*this = -(rhs-*this);
		return (*this);
	}
	if (GetSign() == -1 && *this > rhs) {
		*this = -(rhs-*this);
		return (*this);
	}
	Assert (GetLength () >= rhs.GetLength ());

	Int32 k = 0;
	for (DigitIndex i = 1, n = rhs.GetLength() ; i <= n ; i++) {
	    Int32 j = k + Int32 (GetDigit(i)) - Int32 (rhs.GetDigit (i));
		if (j < 0) { j += kBase; k = -1; } else k = 0;
		Digit(i) = DigitType(j&kLowerMask);
	}
	
	while (k != 0) {
	    Int32 j = k + Int32 (GetDigit(i));
		if (j < 0) { j += kBase; k = -1; } else k = 0;
		Digit(i) = DigitType(j&kLowerMask);
		i++;
	}
	
	Assert (i <= 1 + GetLength());
	/* We know that the left is greater than or equal to the right in
	   absolute value. Therefore we had better not get a final carry.
	   However, we may have gotten shorter. */
	TrimTrailingZeros ();
	return (*this);
}

Integer&	Integer::operator*= (Integer rhs)
{
	/*
	 * If either side is zero, the result is too.
	 */
    if (GetSign () == 0) {
		return (*this);
	}
	if (rhs.GetSign () == 0) {
		NewReference (0); // From *this = rhs;
		Sign () = 0;
		return (*this);
	}

	/*
	 * Save '*this' in 'lhs', and build a new Integer all Digits zero large enough to
	 * hold the product, hence lhs.Length() + rhs.Length()
	 */
	Integer lhs = *this;
	fRep = new IntegerRep (lhs.GetLength () + rhs.GetLength ());
	Sign() *= rhs.GetSign();
	ZeroOut ();
	Length() = lhs.GetLength() + rhs.GetLength();
	 
	BigDigitType k = 0;
	for (DigitIndex j = 1 ; j <= rhs.GetLength() ; j++) {
		for (DigitIndex i = 1 ; i <= lhs.GetLength() ; i++) {
			BigDigitType l = k + BigDigitType(lhs.GetDigit(i)) * 
				BigDigitType(rhs.GetDigit(j)) + BigDigitType(GetDigit(i + j - 1));
			/* We know this will not overflow. At the first digit, the worst
			 * case is (kBase - 1)*(kBase - 1) + (kBase - 1) or
			 * kBase*(kBase - 1) so at worst k is (kBase - 1) and the whole
			 * expression is kBase*kBase - 1. Unsigned arithmetic, though,
			 * is crucial.
			 */
			Digit(i + j - 1) = DigitType(l&kLowerMask);
			k = l>>kRightShift; // We definitely need unsigned shift.
		}
		if (k != 0) {
			BigDigitType l = BigDigitType(GetDigit (i + j - 1)) + k;
			Digit(i + j - 1) = DigitType(l&kLowerMask);
			k = l>>kRightShift; 
			i++;
		}
		Assert (k == 0); // At most one carry from each row.
	}
	TrimTrailingZeros();

	return (*this);
}

static Integer sLastDividend = kZero;
static Integer sLastDivisor = kOne;
static Integer sLastQuotient = kZero;
static Integer sLastRemainder = kZero;

Integer& 	Integer::operator/= (Integer rhs)
{
	if (rhs.GetSign() == 0) { /* Division by zero */
		AssertNotReached ();
	}

	if (GetSign() == 0) {
		*this = kZero;
		return (*this);
	}

	/*
	 * If Left < kBase ^ i and Right >= kBase ^ (j-1) then
	 * Left / Right < kBase ^ (i - j + 1). So i - j + 1 words
	 * is sufficient for the quotient. Clearly j words are
	 * sufficient for the remainder. However, we are not permitted
	 * to destroy the right hand side!
	 */


	if (GetLength() < rhs.GetLength()) {
	   *this = kZero;
	   return (*this);
	}

	Sign() /= rhs.GetSign();

	// NewReference ( ... );
	Integer lhs = *this;
	fRep =  new IntegerRep (GetLength() - rhs.GetLength() + 1);

	DigitIndex j = rhs.GetLength();
	BigDigitType test = kBase * BigDigitType(rhs.GetDigit(j)) +
		BigDigitType(rhs.GetDigit(j-1)) ; // First two Digits of divisor

	for (int tweek = 0 ; test >= kBase ; tweek++) {
		test >>= 1;
	}

	for (DigitIndex i = lhs.GetLength() ; i >= 1 ; i--) {
		BigDigitType t = (kBase * BigDigitType (GetDigit (i)) + BigDigitType (GetDigit (i-1)) ) / (test + 1);
		AssertNotReached(); // Not done
	}



	/*
	 * Mathematical notes.
	 *
	 * Consider the problem of dividing a(2) B*B + a(1) B + a(0) by
	 * b(1) B + b(0). Imagine we wish to estimate the quotient. Using
	 * the division algorithm, we find q, r such that
	 * a(2) B + a(1) - q (b(1) + 1) = r where 0 <= r < b(1) + 1.
	 *
	 * We want to prove first that this is a "safe" estimate for the
	 * quotient, that is:
	 * a(2) B*B + a(1) B + a(0) - q (b(1) B + b(0)) is positive, but
	 * since a(0) >= 0 and B > b(0) we may reach this conclusion since
	 * a(2) B*B + a(1) B - q B (b(1) + 1) is positive per the above.
	 *
	 * As a consequence of the above we know that q is an adequate
	 * estimate for the first Digit of the quotient: it is guaranteed
	 * to be less than the correct quotient Q where Q, R are positive
	 * so that a(2) B*B + a(1) B + a(0) - Q (b(1) B + b(0)) = R and
	 * 0 <= R < b(1) B + b(0). In mathematical terms, 0 <= q <= Q. The
	 * question now is how far off is q: how can we bound Q - q?
	 * 
	 * 
	 */
}

Integer&	Integer::operator%= (Integer rhs)
{
	AssertNotReached ();	// NYI
	// DO REAL WORK HERE!!!
	return (*this);
}

void	Integer::MakeInteger (unsigned long from)
{
    if (from == 0)	{
		fRep = new IntegerRep (0);
		Sign () = 0;
	}
	else if (from < kBase) {
		fRep = new IntegerRep (1);
		Sign () = 1;
		Digit (1) = DigitType(from);
	}
	else {
		fRep = new IntegerRep (2);
		Sign () = 1;
		Digit (2) = DigitType(BigDigitType(from) >> kRightShift);
		Digit (1) = DigitType(from & kLowerMask);
	}
}

IntegerRep*	Integer::Clone (const IntegerRep& old)
{
	IntegerRep* rep = new IntegerRep (old.fLength);
	memcpy (rep->fStorage, old.fStorage, old.fLength * sizeof (DigitType));
	return (rep);
}

void	Integer::NewReference (DigitIndex words)
{
	fRep = new IntegerRep (words);
}

DigitType  Integer::GetDigit (DigitIndex i) const
{
	Require (1 <= i);

/*
 * Remember fLength merely tells us the index of the most significant
 * Digit which is not zero.
 */
//	Require (i <= fRep->fLength);
	return (fRep->fStorage [i-1]);
}

DigitType& Integer::Digit (DigitIndex i)
{
	Require (1 <= i);
//	Require (i <= fRep->fLength);
	Require (fRep.CountReferences () == 1);
	return (fRep->fStorage [i-1]);
}

DigitIndex Integer::GetLength () const
{
	return (fRep->fLength);
}

DigitIndex& Integer::Length ()
{
	Require (fRep.CountReferences () == 1);
	return (fRep->fLength);
}

SignType	Integer::GetSign () const
{
	return (fSign);
}

SignType&	Integer::Sign ()
{
	return (fSign);
}

void	Integer::ReSize (DigitIndex words)
{
	Require (fRep.CountReferences () == 1);

	/* If we add Digits, they had better be zero Digits. In other words require we return the
	   same integer value we started with.
	 */
	if (words > GetLength()) {
		fRep->fStorage = (DigitType*)ReAlloc (fRep->fStorage, sizeof (DigitType) * words);
		memset (fRep->fStorage + GetLength() + 1, 0, (words - GetLength()) * sizeof (DigitType));
	}
}

void	Integer::ZeroOut () 
{	
	Require (fRep.CountReferences () == 1);
	memset (fRep->fStorage, 0, GetLength() * sizeof (DigitType));
}

void	Integer::TrimTrailingZeros ()
{
	/*
	 * Trim trailing zeros so our representation is as small as possible, and if it turns out to
	 * be zero or one, return our global constant so we get sharing of that representation.
	 */
	Require (fRep.CountReferences () == 1);
	for (DigitIndex i = GetLength (); i >= 1 and GetDigit (i) == 0; i--)
		;
	if (i == 0) {
		Sign () = 0;
	}
	Length() = i;
}

void	Integer::Scrunch ()
{
	fRep->fStorage = (DigitType*)ReAlloc (fRep->fStorage, sizeof (DigitType) * GetLength());
}







/*
 ********************************************************************************
 ************************************* operator+ ********************************
 ********************************************************************************
 */

Integer	operator+ (const Integer& lhs, const Integer& rhs)
{
	Integer	result = lhs;
	result  += rhs;
	return (result);
}




/*
 ********************************************************************************
 ************************************* operator- ********************************
 ********************************************************************************
 */

Integer	operator- (const Integer& lhs, const Integer& rhs)
{
	Integer	result = lhs;
	result  -= rhs;
	return (result);
}




/*
 ********************************************************************************
 ************************************* operator* ********************************
 ********************************************************************************
 */

Integer	operator* (const Integer& lhs, const Integer& rhs)
{
	Integer	result = lhs;
	result  *= rhs;
	return (result);
}




/*
 ********************************************************************************
 ************************************* operator/ ********************************
 ********************************************************************************
 */

Integer	operator/ (const Integer& lhs, const Integer& rhs)
{
	Integer	result 	=	lhs;
	result  /= rhs;
	return (result);
}




/*
 ********************************************************************************
 ************************************* operator% ********************************
 ********************************************************************************
 */

Integer	operator% (const Integer& lhs, const Integer& rhs)
{
	Integer	result = lhs;
	result  %= rhs;
	return (result);
}





/*
 ********************************************************************************
 ************************************* operator== *******************************
 ********************************************************************************
 */

Boolean	operator== (const Integer& lhs, const Integer& rhs)
{
	if (lhs.GetSign() != rhs.GetSign())			return (False);

	Assert (lhs.GetSign () == rhs.GetSign ());
	if (lhs.fRep == rhs.fRep) {
		return (True);
	}
	if (lhs.GetLength() != rhs.GetLength()) {
		return (False);
	}
	return (Boolean (!memcmp (lhs.fRep->fStorage, rhs.fRep->fStorage, (lhs.GetLength ()) * sizeof (DigitType))));
}





/*
 ********************************************************************************
 ************************************* operator!= *******************************
 ********************************************************************************
 */

Boolean	operator!= (const Integer& lhs, const Integer& rhs)
{
	if (lhs.GetSign () != rhs.GetSign ())		return (True);

	Assert (lhs.GetSign () == rhs.GetSign ());
	if (lhs.fRep == rhs.fRep) {
		return (False); 					// Same magnitude, Same sign.
	}
	if (lhs.GetLength ()  != rhs.GetLength ()) {
		return (True);
	} // If GetLength() does not work as promised, this fails.
	return (Boolean (!!memcmp (lhs.fRep->fStorage, rhs.fRep->fStorage, lhs.GetLength() * sizeof (DigitType))));
}




/*
 ********************************************************************************
 ************************************* operator< ********************************
 ********************************************************************************
 */

Boolean	operator< (const Integer& lhs, const Integer& rhs)
{
	if (lhs.GetSign() < rhs.GetSign()) return (True);
	if (rhs.GetSign() < lhs.GetSign()) return (False);
	Assert (lhs.GetSign() == rhs.GetSign()) ;
	
	if (lhs.fRep == rhs.fRep) {
		return (False);
	}
	
	if (lhs.GetSign() == -1) return (-lhs > -rhs);
	
	if (lhs.GetLength() < rhs.GetLength()) {
		return (True);
	}
	if (rhs.GetLength() < lhs.GetLength()) {
		return (False);
	}
	Assert (lhs.GetLength() == rhs.GetLength());

	for (DigitIndex i = lhs.GetLength() ; i >= 1 ; i--)
	{
		if (lhs.GetDigit(i) < rhs.GetDigit(i)) return (True);
		if (lhs.GetDigit(i) > rhs.GetDigit(i)) return (False);
	}
	return (False);
}




/*
 ********************************************************************************
 ************************************* operator<= *******************************
 ********************************************************************************
 */

Boolean	operator<= (const Integer& lhs, const Integer& rhs)
{
	return Boolean (lhs < rhs or lhs == rhs);
}




/*
 ********************************************************************************
 ************************************* operator> ********************************
 ********************************************************************************
 */

Boolean	operator> (const Integer& lhs, const Integer& rhs)
{
	if (lhs.GetSign() > rhs.GetSign()) return (True);
	if (rhs.GetSign() > lhs.GetSign()) return (False);
	Assert (lhs.GetSign() == rhs.GetSign()) ;

	if (lhs.fRep == rhs.fRep) {
		return (False);
	}

	if (lhs.GetSign() == -1) return (-lhs < -rhs);

	if (lhs.GetLength() > rhs.GetLength()) {
		return (True);
	}
	if (rhs.GetLength() > lhs.GetLength()) {
		return (False);
	}
	Assert (lhs.GetLength() == rhs.GetLength());

	for (DigitIndex i = lhs.GetLength() ; i >= 1 ; i--)
	{
		if (lhs.GetDigit(i) > rhs.GetDigit(i)) return (True);
		if (lhs.GetDigit(i) < rhs.GetDigit(i)) return (False);
	}
	return (False);
}




/*
 ********************************************************************************
 ************************************* operator>= *******************************
 ********************************************************************************
 */

Boolean	operator>= (const Integer& lhs, const Integer& rhs)
{
	return Boolean (lhs > rhs or lhs == rhs);
}




/*
 ********************************************************************************
 ************************************* operator<< *******************************
 ********************************************************************************
 */
ostream&	operator<< (ostream& out, const Integer& s)
{
	out << "GetSign() is  " << int (s.GetSign ()) << tab;
	for (int i = 1; i <= s.GetLength (); i++) {
		out << "GetDigit (" << i << ") is  " << s.GetDigit (i) << "," << tab;
	}
	return (out);
}




/*
 ********************************************************************************
 ************************************* operator>> *******************************
 ********************************************************************************
 */
istream&	operator>> (istream& in, Integer& s)
{
	/* We are looking for a possible sign and a sequence of digits,
	   of arbitrary length.
	 */

	char SignMarker;
	if (!(in >> SignMarker)) {
		AssertNotReached();
		/* Handle( ... ) */
	}

	if (isdigit(SignMarker)) in.putback(SignMarker);
	else if (SignMarker == '-' || SignMarker == '+') {}
	else {
		AssertNotReached();
		/* Raise ( ... ) */
	}

	/* From here on in we do not want to see whitespace. */
	in >> resetiosflags(ios::skipws);
	/* This is safe for BigDigitType. */
	in >> setw(kDecimalDigits);

	BigDigitType u = 0;
	s = kZero;
	while (in >> u) {
		s *= kBigDigitDecimal;
		s += u;
	}

	in >> setiosflags(ios::skipws);
	in >> setw(0);
	return (in);
}



#endif	/*qCFront_NestedTypesHalfAssed || qRealTemplatesAvailable*/

#endif


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***



