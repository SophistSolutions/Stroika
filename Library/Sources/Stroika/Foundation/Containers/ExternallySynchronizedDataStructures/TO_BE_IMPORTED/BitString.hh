/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__BitString__
#define	__BitString__

/*
 * $Header: /fuji/lewis/RCS/BitString.hh,v 1.4 1992/10/09 16:54:15 lewis Exp $
 *
 * Description:
 *
 *		BitString is a compact array of Booleans.
 *
 * TODO:
 *		-	Think out more carefully what the INDEX type should be for this
 *			class. size_t would be too small, since we can get 8* that many
 *			bits into a bitstring. But UInt32 isn't necessarily right either.
 *			Probably unsigned long would be the best choice, since??
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: BitString.hh,v $
 *		Revision 1.4  1992/10/09  16:54:15  lewis
 *		Use size_t instead of UInt32/UInt8.
 *
 *
 *
 */

#include	"Config-Foundation.hh"



class	BitString {
	public:
		BitString (const BitString& from);
		BitString (size_t length = 0);
		~BitString ();
		const BitString& operator= (const BitString& rhs);

		nonvirtual	size_t	GetLength () const;
		nonvirtual	void	SetLength (size_t newLength, Boolean fillValue = False);

		/*
		 * Indices are 1 based.
		 */
		nonvirtual	Boolean	GetBit (size_t index) const;
		nonvirtual	void	SetBit (size_t index, Boolean newValue);

		nonvirtual	void	ShiftRight (size_t by);
		nonvirtual	void	ShiftLeft (size_t by);
		nonvirtual	void	RotateRight (size_t by);
		nonvirtual	void	RotateLeft (size_t by);

	private:
		size_t	fBitLength;
		UInt8*	fBits;

		static	size_t	CalcBytes (size_t index);
		static	size_t	CalcOffset (size_t index);
};



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__BitString__*/


