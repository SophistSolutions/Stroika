/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Integer__
#define	__Integer__

/*
 * $Header: /fuji/lewis/RCS/Integer.hh,v 1.8 1992/10/29 15:53:58 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 * Design Notes:
 *
 * 		The representation we choose for an Integer is a string of unsigned
 *	digits base kBase plus a sign of zero, one or minus one. We choose this
 *	base so that the square of the largest digit (kBase-1) plus possible
 *	carry (1) fits in a long, for the Macintosh 32 bits. Conveniently,
 *	this means we may store our digits in a 16 bit word, but we must use
 *	32 bit arithmetic inside to avoid an overflow.
 *	
 *  	Because the special cases of a number being 1 or zero, we will probably
 * 	check all over the place where we can convert things into references
 * 	to these.
 *
 * Changes:
 *	$Log: Integer.hh,v $
 *		Revision 1.8  1992/10/29  15:53:58  lewis
 *		#if whole thing out -fixup later - not part of first stroika foundation release.
 *
 *		Revision 1.7  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.6  1992/09/11  16:00:03  sterling
 *		used new Shared implementation
 *
 *		Revision 1.4  1992/07/01  01:42:07  lewis
 *		Got rid of qNestedTypesAvailable flag - assume always true.
 *
 *		Revision 1.3  1992/06/25  12:14:15  lewis
 *		Comment out the file in the case where we have no templates available, nested types, and
 *		no CFront half assed nested types. Could get it to work in this case, but difficult, and not
 *		worth doing right now.
 *
 *		Revision 1.9  1992/06/09  12:36:15  lewis
 *		Renamed SmartPointer back to Shared.
 *
 *		Revision 1.8  92/06/09  01:00:53  01:00:53  lewis (Lewis Pringle)
 *		Merged in Sterls SmartPointer code and added template case.
 *		
 *		Revision 1.6  1992/02/21  17:51:24  lewis
 *		Got rid of qGPlus_ClassOpNewDelBroke workaround.
 *
 *
 *
 */

#include	"Config-Foundation.hh"
#include	"Debug.hh"
#include	"Shared.hh"



// Temporarily disable whole thing since not part of Stroika Foundation first release...
#if		0


// This whole class can only work if either we HAVE qCFront_NestedTypesHalfAssed or qRealTemplatesAvailable because if
// we have qCFront_NestedTypesHalfAssed && !qRealTemplatesAvailable, then must do the macro hack with nested classes -
// and while than can be made to work, it is hard (use typedefs, and tricks). For now, assume we dont need that...
#if		qCFront_NestedTypesHalfAssed || qRealTemplatesAvailable

#if		!qRealTemplatesAvailable
class	IntegerRep;
#include	"TFileMap.hh"
#include	SharedOfIntegerRep_hh
#endif	/*!qRealTemplatesAvailable*/


#if		qHPCPlus_NestedClassWithVirtMethSubclassingBug
typedef	Int8	SignType;
typedef	UInt32	BigDigitType;			// BigDigitType MUST be unsigned and at least twice as big as DigitType
typedef	UInt16	DigitType;
typedef	UInt16	DigitIndex;

		class	IntegerRep {
			public:
				DigitIndex	fLength;
				DigitType*	fStorage;

				static	void*	operator new (size_t n);
				static	void	operator delete (void* p);

				nonvirtual	IntegerRep (DigitIndex words = 0);
				nonvirtual	~IntegerRep ();
		};
#endif	/*qHPCPlus_NestedClassWithVirtMethSubclassingBug*/



class	Integer {
	public:
		Integer ();
		Integer (char from);
		Integer (unsigned char from);
		Integer (short from);
		Integer (unsigned short from);
		Integer (int from);
		Integer (unsigned int from);
		Integer (long from);
		Integer (unsigned long from);
		Integer (Real from);				// round from to nearest integer
		Integer (const Integer& from);
		~Integer ();

		nonvirtual	Integer&	operator= (const Integer& newInteger);

		nonvirtual	Integer		operator- () const;

		nonvirtual	Integer&	operator+= (Integer rhs);
		nonvirtual	Integer&	operator-= (Integer rhs);
		nonvirtual	Integer&	operator*= (Integer rhs);
		nonvirtual	Integer&	operator/= (Integer rhs);
		nonvirtual	Integer&	operator%= (Integer rhs);	// remainder - as in C - not MOD

// while debugging
//	private:
#if		!qHPCPlus_NestedClassWithVirtMethSubclassingBug
		typedef	Int8	SignType;
		typedef	UInt32	BigDigitType;			// BigDigitType MUST be unsigned and at least twice as big as DigitType
		typedef	UInt16	DigitType;
		typedef	UInt16	DigitIndex;


		class	IntegerRep {
			public:
				DigitIndex	fLength;
				DigitType*	fStorage;

				static	void*	operator new (size_t n);
				static	void	operator delete (void* p);

				nonvirtual	IntegerRep (DigitIndex words = 0);
				nonvirtual	~IntegerRep ();
		};
#endif	/*!qHPCPlus_NestedClassWithVirtMethSubclassingBug*/

		#if		qRealTemplatesAvailable
			Shared<IntegerRep>	fRep;
		#else
			Shared(IntegerRep)	fRep;
		#endif
		
		SignType	fSign;

		nonvirtual	void	MakeInteger (unsigned long from);

		nonvirtual	void	AssureOneReference ();
		nonvirtual	void	BreakReferences ();								// break shared part, but copy
		nonvirtual	void	NewReference (DigitIndex words = 0);			// clear out our old reference and build a new one of size n

		/*
		 * Set fLength to the given number of words, and resize the memory
		 * for the wordstring as necessary.
		 */
		nonvirtual	void	ReSize (DigitIndex words);

		/*
		 * Zero each of the allocated cells.
		 */
		nonvirtual	void	ZeroOut ();

		/*
		 * Downwardly adjust the length to account for zeros at the end of
		 * the string. This is useful because the digits are ordered from least to
		 * most significant, and so trailing zeros there are insignificant.
		 */
		nonvirtual	void	TrimTrailingZeros ();

		/*
		 * To optimize free store operations, we may sometimes leave more
		 * allocated than necessary. Call this a good breaking point
		 * to clean up usage. <Not sure we need this>
		 */
		nonvirtual	void	Scrunch ();

		nonvirtual	DigitType 	GetDigit (DigitIndex i) const;
		nonvirtual	DigitType&	Digit (DigitIndex i);
		nonvirtual	DigitIndex	GetLength () const;
		nonvirtual	DigitIndex&	Length ();
		nonvirtual	SignType	GetSign () const;
		nonvirtual	SignType&	Sign ();

	/*
	 * These are made freinds so they can peek at the shared part, as an optimization
	 */
	friend	Boolean	operator== (const Integer& lhs, const Integer& rhs);
	friend	Boolean	operator!= (const Integer& lhs, const Integer& rhs);
	friend	Boolean	operator< (const Integer& lhs, const Integer& rhs);
	friend	Boolean	operator<= (const Integer& lhs, const Integer& rhs);
	friend	Boolean	operator> (const Integer& lhs, const Integer& rhs);
	friend	Boolean	operator>= (const Integer& lhs, const Integer& rhs);

	static	IntegerRep*	Clone (const IntegerRep& old);
};


extern	Integer	operator+ (const Integer& lhs, const Integer& rhs);
extern	Integer	operator- (const Integer& lhs, const Integer& rhs);
extern	Integer	operator* (const Integer& lhs, const Integer& rhs);
extern	Integer	operator/ (const Integer& lhs, const Integer& rhs);
extern	Integer	operator% (const Integer& lhs, const Integer& rhs);

extern	Integer	Mod (const Integer& lhs, const Integer& rhs);
extern	Integer	GCD (const Integer& lhs, const Integer& rhs);

extern	Boolean	operator== (const Integer& lhs, const Integer& rhs);
extern	Boolean	operator!= (const Integer& lhs, const Integer& rhs);
extern	Boolean	operator< (const Integer& lhs, const Integer& rhs);
extern	Boolean	operator<= (const Integer& lhs, const Integer& rhs);
extern	Boolean	operator> (const Integer& lhs, const Integer& rhs);
extern	Boolean	operator>= (const Integer& lhs, const Integer& rhs);


/*
 *   Stream inserters and extractors.
 */
class ostream;
class istream;
extern	ostream&	operator<< (ostream& out, const Integer& s);
extern	istream&	operator>> (istream& in, Integer& s);



/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */



#endif	/*!qCFront_NestedTypesHalfAssed || qRealTemplatesAvailable*/



#endif



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Integer__*/
