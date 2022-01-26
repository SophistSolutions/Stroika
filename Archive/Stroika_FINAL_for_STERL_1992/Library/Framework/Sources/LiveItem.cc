/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/LiveItem.cc,v 1.2 1992/07/03 01:00:30 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: LiveItem.cc,v $
 *		Revision 1.2  1992/07/03  01:00:30  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.1  1992/06/20  17:30:04  lewis
 *		Initial revision
 *
 *		Revision 1.5  1992/03/26  09:48:54  lewis
 *		Got rid of oldLive arg to EffectiveLiveChanged.
 *
 *		Revision 1.4  1992/02/15  05:32:52  sterling
 *		added EffectiveLiveChanged ()
 *
 *
 */

#include "Debug.hh"

#include "LiveItem.hh"




/*
 ********************************************************************************
 ************************************* LiveItem *********************************
 ********************************************************************************
 */
const	Boolean	LiveItem::kLive	= True;

LiveItem::LiveItem ()
{
}

LiveItem::~LiveItem ()
{
}

Boolean	LiveItem::GetEffectiveLive () const
{
	return (GetLive ());
}

void	LiveItem::EffectiveLiveChanged (Boolean /*newLive*/, Panel::UpdateMode /*updateMode*/)
{
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

