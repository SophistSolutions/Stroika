/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Angle__
#define	__Angle__

/*
 * $Header: /fuji/lewis/RCS/Angle.hh,v 1.7 1992/11/12 07:59:34 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 *
 * Changes:
 *	$Log: Angle.hh,v $
 *		Revision 1.7  1992/11/12  07:59:34  lewis
 *		Got rid of Real - use double instead.
 *
 *		Revision 1.6  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.5  1992/09/25  21:09:58  lewis
 *		Get rid of Declares/Implements for containers of Angle - no longer needed
 *
 *		Revision 1.4  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.3  1992/09/15  16:46:28  lewis
 *		Got rid of AbSequence.
 *
 *		Revision 1.11  1992/04/15  19:07:33  lewis
 *		Ooops - changed operatorX methods to operatorX=.
 *
 *		Revision 1.9  92/04/15  14:29:16  14:29:16  lewis (Lewis Pringle)
 *		Added ?= operators for +-*, and /
 *		
 *		Revision 1.8  92/03/30  15:12:46  15:12:46  lewis (Lewis Pringle)
 *		Ported to BorlandC++ and added ifdefs for qRealTemplatesAvaialble.
 *		
 *		Revision 1.6  1992/01/22  02:23:39  lewis
 *		Add iostream operators.
 *
 *
 *
 */

#include	"Config-Foundation.hh"




class	Angle {
	public:
		enum AngleFormat { eRadians, eDegrees, eGradians };
		Angle ();
		Angle (double angle, AngleFormat angleFormat = eRadians);

		nonvirtual	double	AsRadians () const;
		nonvirtual	double	AsDegrees () const;
		nonvirtual	double	AsGradians () const;

		nonvirtual	const	Angle&	operator+= (const Angle& rhs);
		nonvirtual	const	Angle&	operator-= (const Angle& rhs);
		nonvirtual	const	Angle&	operator*= (double rhs);
		nonvirtual	const	Angle&	operator/= (double rhs);

	private:
		double	fAngleInRadians;
};





/*
 * Arithmatic
 */
extern	Angle	operator+ (const Angle& lhs, const Angle& rhs);
extern	Angle	operator- (const Angle& lhs, const Angle& rhs);
extern	Angle	operator* (const Angle& lhs, double rhs);
extern	Angle	operator* (double lhs, const Angle& rhs);
extern	Angle	operator/ (const Angle& lhs, double rhs);


/*
 * Comparisons
 */
extern	Boolean	operator== (const Angle& lhs, const Angle& rhs);
extern	Boolean	operator!= (const Angle& lhs, const Angle& rhs);
extern	Boolean	operator<  (const Angle& lhs, const Angle& rhs);
extern	Boolean	operator<= (const Angle& lhs, const Angle& rhs);
extern	Boolean	operator>  (const Angle& lhs, const Angle& rhs);
extern	Boolean	operator>= (const Angle& lhs, const Angle& rhs);


extern	Angle	Min (const Angle& a1, const Angle& a2);
extern	Angle	Max (const Angle& a1, const Angle& a2);


/*
 * iostream support.
 */
class ostream;
class istream;
extern	ostream&	operator<< (ostream& out, const Angle& a);
extern	istream&	operator>> (istream& in, Angle& a);






/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */

inline	double	Angle::AsRadians () const		{	return (fAngleInRadians);	}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Angle__*/
