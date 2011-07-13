/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/EnableItem.cc,v 1.3 1992/09/01 15:46:50 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: EnableItem.cc,v $
 *		Revision 1.3  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/07/03  00:24:02  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.1  1992/06/20  17:30:04  lewis
 *		Initial revision
 *
 *		Revision 1.6  1992/03/13  16:35:56  lewis
 *		Made enableItem ctor/dtor inline, and moved EnableView to User.
 *
 *		Revision 1.5  1992/02/15  05:32:25  sterling
 *		supported EffectiveLiveChanged
 *
 *		Revision 1.4  1992/01/27  06:16:17  lewis
 *		Allow SetSenabled_ () to be called with the same enable value as is current (removed assertion that it was different).
 *
 *
 *
 */



#include	"EnableItem.hh"





/*
 ********************************************************************************
 ********************************** EnableItem **********************************
 ********************************************************************************
 */

const	Boolean	EnableItem::kEnabled	=	True;

void	EnableItem::SetEnabled (Boolean enabled, Panel::UpdateMode updateMode)
{
	if (GetEnabled () != enabled) {
		SetEnabled_ (enabled, updateMode);
	}
	Ensure (GetEnabled () == enabled);
}





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


