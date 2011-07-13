/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__SimpleClass__
#define	__SimpleClass__

/*
 * $Header: /fuji/lewis/RCS/SimpleClass.hh,v 1.2 1992/10/22 18:17:36 lewis Exp $
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
 * Changes:
 *	$Log: SimpleClass.hh,v $
 *		Revision 1.2  1992/10/22  18:17:36  lewis
 *		Dont depend on Collection.hh. Also, added sTotalLiveObjects static.
 *		We want to assure this goes to zero when were dont to be sure everything
 *		freed.
 *
 *		Revision 1.6  1992/09/29  15:45:43  sterling
 *		Get rid of use of String - use new "SimpleClass" instead.
 *
 *		
 *
 */

#include	"Config-Foundation.hh"


class	SimpleClass {
	public:
		SimpleClass (UInt32 v);
		SimpleClass (const SimpleClass& f);
		~SimpleClass ();
		
		nonvirtual	UInt32	GetValue () const;
		static		UInt32	GetTotalLiveCount ();
	
	private:	
		UInt32	fValue;
		int		fConstructed;
		static	UInt32	sTotalLiveObjects;
	
	friend	Boolean	operator== (const SimpleClass& lhs, const SimpleClass& rhs);
	friend	Boolean	operator< (const SimpleClass& lhs, const SimpleClass& rhs);
};
Boolean	operator== (const SimpleClass& lhs, const SimpleClass& rhs);
Boolean	operator< (const SimpleClass& lhs, const SimpleClass& rhs);




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__SimpleClass__*/


