/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Changeable.cc,v 1.3 1992/10/10 04:27:57 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Changeable.cc,v $
 *		Revision 1.3  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.2  1992/09/01  15:26:25  sterling
 *		Lots.
 *
 *		Revision 1.7  1992/05/13  12:59:13  lewis
 *		STERL: changed an Ensure () in GetLastSavedChangeCount().
 *
 *
 */




#include	"Changeable.hh"




/*
 ********************************************************************************
 ************************************ Changeable ********************************
 ********************************************************************************
 */
Changeable::Changeable ():
	fChangeCount (0),
	fLastSavedCount (0)
{
}

Changeable::~Changeable ()
{
}

ChangeCount	Changeable::GetChangeCount () const					
{	
	Ensure (fChangeCount >= fLastSavedCount);
	Ensure (fChangeCount >= 0);	
	return (fChangeCount);		
}

void	Changeable::SetChangeCount (ChangeCount newCount)	
{	
	Require (newCount >= 0);
	Require (newCount >= fLastSavedCount);
	if (fChangeCount != newCount) {
		SetChangeCount_ (newCount);
	}
	Ensure (fChangeCount == newCount);
}

void	Changeable::PlusChangeCount ()							
{	
	SetChangeCount  (GetChangeCount () + 1);				
}

void	Changeable::MinusChangeCount ()
{
	Require (GetChangeCount () >= 1);
	SetChangeCount (GetChangeCount () - 1);
	Ensure (GetChangeCount () >= 0);
}

ChangeCount	Changeable::GetLastSavedCount () const
{
	Ensure (fLastSavedCount >= 0);
	Ensure (fChangeCount >= fLastSavedCount);
	return (fLastSavedCount);		
}

void	Changeable::SetLastSavedCount (ChangeCount newCount)
{
	Require (newCount <= fChangeCount);
	Require (newCount >= 0);
	fLastSavedCount = newCount;
}
		
Boolean	Changeable::GetDirty () const			
{	
	return (Boolean (GetChangeCount () > fLastSavedCount));		
}

void	Changeable::SetDirty ()
{
	if (not GetDirty ()) {
		PlusChangeCount ();
	}
	Ensure (GetDirty ());
}

void	Changeable::SetClean ()	
{
	if (GetDirty ()) {
		SetChangeCount (GetLastSavedCount ());
	}
	Ensure (not GetDirty ());
}

void	Changeable::ResetChangeCount ()
{
	fLastSavedCount = 0;
	SetChangeCount (fLastSavedCount);
	Ensure (not GetDirty ());
}

void	Changeable::SetChangeCount_ (ChangeCount newCount)	
{	
	fChangeCount = newCount;	
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

