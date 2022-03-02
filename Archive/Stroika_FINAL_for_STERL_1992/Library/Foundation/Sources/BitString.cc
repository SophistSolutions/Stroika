/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/BitString.cc,v 1.4 1992/10/09 21:30:44 lewis Exp $
 *
 * TODO:
 *		+	Performance tweek. Make enuf inline so that a good compiler
 *			can inline and strength reduce and pull stuff out of loops,
 *			with GetAt () - maybe more?
 *
 *		+	Consider caching length (aka bits set count?).
 *
 *		+	Consider using Nil Ptr for empty sets - save needless alloc time
 *			here??? Common case!
 *
 *		+	CalcBytes/CalcOffset should probably be inline.
 *
 * Changes:
 *	$Log: BitString.cc,v $
 *		Revision 1.4  1992/10/09  21:30:44  lewis
 *		Cleanup. Comment on performance hacks todo. UInt32/UInt8->size_t.
 *
 *		Revision 1.3  1992/09/05  04:55:50  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.2  1992/09/01  15:26:25  sterling
 *		Lots.
 *
 *
 *
 */



#include	<string.h>
#include	<stdlib.h>

#include	"Debug.hh"
#include	"Memory.hh"

#include	"BitString.hh"




/*
 ********************************************************************************
 ************************************** BitString *******************************
 ********************************************************************************
 */

BitString::BitString (const BitString& from):
	fBitLength (from.fBitLength),
	fBits (Nil)
{
	fBits = new UInt8 [CalcBytes (fBitLength)];
	memcpy (fBits, from.fBits, size_t (CalcBytes (fBitLength)));
}

BitString::BitString (size_t length):
	fBitLength (0),
	fBits (Nil)
{
	SetLength (length);
}

BitString::~BitString ()
{
	delete (fBits);
}

const BitString& BitString::operator= (const BitString& rhs)
{
	delete (fBits); fBits = Nil;
	fBitLength = rhs.fBitLength;
	fBits = new UInt8 [CalcBytes (fBitLength)];
	memcpy (fBits, rhs.fBits, size_t (CalcBytes (fBitLength)));
	return (*this);
}

size_t	BitString::GetLength () const
{
	return (fBitLength);
}

void	BitString::SetLength (size_t newLength, Boolean fillValue)
{
	if (newLength != GetLength ()) {
		size_t	oldLength	=	GetLength ();
		if (fBits == Nil) {
			fBits = new UInt8 [CalcBytes (newLength)];
		}
		else {
			fBits = (UInt8*)ReAlloc (fBits, (size_t)CalcBytes (newLength));
		}
		fBitLength = newLength;
		if (newLength > oldLength) {
			for (size_t i = oldLength+1; i <= newLength; i++) {
				SetBit (i, fillValue);
				Assert (GetBit (i) == fillValue);
			}
		}
	}
	Ensure (GetLength () == newLength);
}

Boolean	BitString::GetBit (size_t index) const
{
	Require (index >= 1);
	Require (index <= GetLength ());
	AssertNotNil (fBits);
	register	UInt8	byte	=	fBits [CalcBytes (index)-1];
	register	UInt8	mask	=	(1 << CalcOffset (index));
	return Boolean (!!(byte & mask));
}

void	BitString::SetBit (size_t index, Boolean newValue)
{
	Require (index >= 1);
	Require (index <= GetLength ());
	AssertNotNil (fBits);
	register	UInt8&	byte	=	fBits [CalcBytes (index)-1];
	register	UInt8	mask	=	(1 << CalcOffset (index));
	if (newValue) {
		byte |= mask;
	}
	else {
		byte &= ~mask;
	}
}

void	BitString::ShiftRight (size_t by)
{
	for (size_t i = GetLength (); i > by; i--) {
		SetBit (i, GetBit (i - by));
	}
	for (; i != 0; i--) {
		SetBit (i, False);
	}
}

void	BitString::ShiftLeft (size_t by)
{
	for (size_t i = 1; i <= (GetLength ()-by); i++) {
		SetBit (i, GetBit (i + by));
	}
	for (; i <= GetLength (); i++) {
		SetBit (i, False);
	}
}

void	BitString::RotateRight (size_t by)
{
	/*
	 * Save bits, shift, and restore the ones shifted off end.
	 */
	by %= GetLength ();	// wraps around
	BitString	savedBits	=	BitString (by);
	for (size_t i = 1 ; i <= by; i++) {
		savedBits.SetBit (i, GetBit (GetLength () - by + i));
	}
	ShiftRight (by);
	for (i = 1 ; i <= by; i++) {
		SetBit (i, savedBits.GetBit (i));
	}
}

void	BitString::RotateLeft (size_t by)
{
	/*
	 * Save bits, shift, and restore the ones shifted off start.
	 */
	by %= GetLength ();	// wraps around
	BitString	savedBits	=	BitString (by);
	for (size_t i = 1 ; i <= by; i++) {
		savedBits.SetBit (i, GetBit (i));
	}
	ShiftLeft (by);
	for (i = 1 ; i <= by; i++) {
		SetBit (GetLength () - by + i, savedBits.GetBit (i));
	}
}

size_t	BitString::CalcBytes (size_t index)
{
	Require (index >= 0);
	return ((index + (8*sizeof (UInt8)) - 1) / (8*sizeof (UInt8)));
}

size_t	BitString::CalcOffset (size_t index)
{
	Require (index > 0);
	return ((index-1) % (8*sizeof (UInt8)));			// ret val zero based
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

