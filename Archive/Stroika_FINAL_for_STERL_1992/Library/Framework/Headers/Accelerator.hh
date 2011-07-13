/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Accelerator__
#define	__Accelerator__

/*
 * $Header: /fuji/lewis/RCS/Accelerator.hh,v 1.4 1992/09/01 15:42:04 sterling Exp $
 *
 * Description:
 *
 *
 *
 * TODO:
 *
 * Notes:
 *
 * Changes:
 *	$Log: Accelerator.hh,v $
 *		Revision 1.4  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/07/01  04:05:14  lewis
 *		Renamed Strings.hh String.hh
 *
 *		Revision 1.2  1992/02/19  17:15:06  lewis
 *		Started making Accelerator a real class.
 *
 *		Revision 1.1  1992/02/15  00:40:15  lewis
 *		Initial revision
 *
 *
 */

#include	"String.hh"

#include	"Config-Framework.hh"


#if		qMPW_MacroOverflowProblem
//(DUPLICATED FROM MENUITEM.hh)
#define	Accelerator					accl
#endif


class	Accelerator {
	public:
		enum AccKind { eCommandKey, eAltKey, eOptionKey, eFunctionKey };
		Accelerator (const String& oldWay):
			fAccelerator (oldWay)
		  {
		  }

		nonvirtual	Boolean		Empty () const 			{ return Boolean (fAccelerator == kEmptyString); }
		nonvirtual	AccKind		GetAccKind () const 	{ return (eCommandKey); }
		nonvirtual	Character	GetAccChar () const		{ return (fAccelerator [2]); }
		nonvirtual	String		AsString () const		{ return fAccelerator; }

	private:
		String	fAccelerator;
};


Boolean	operator== (const Accelerator& lhs, const Accelerator& rhs)
{
	return (Boolean (lhs.AsString ()  == rhs.AsString () ));
}

Boolean	operator!= (const Accelerator& lhs, const Accelerator& rhs)
{
	return (Boolean (lhs.AsString ()  != rhs.AsString () ));
}






// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Accelerator__*/

