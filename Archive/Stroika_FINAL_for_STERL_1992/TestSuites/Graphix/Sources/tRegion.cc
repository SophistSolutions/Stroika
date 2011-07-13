/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/tRegion.cc,v 1.2 1992/09/01 17:52:15 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: tRegion.cc,v $
 *		Revision 1.2  1992/09/01  17:52:15  sterling
 *		*** empty log message ***
 *
 *		Revision 1.5  92/03/11  23:11:46  23:11:46  lewis (Lewis Pringle)
 *		Use cout/cerr instead of gDebugStream since thats not available when debug turned off anyhow, and using
 *		two different streams allows us to differentiate info from actual errors.
 *		
 *		Revision 1.4  1992/01/22  04:11:53  lewis
 *		Call GDIInit and cleanups.
 *
 *
 */

/*
 * Has to come before other includes, so that we know this constructor called VERY early in
 * the game.
 */
#pragma	push
#pragma	force_active	on

#include	"GDIInit.hh"
static	GDIInit	sInit	=	GDIInit ();
#pragma	pop

#include	"Stroika-Graphix-Globals.hh"

#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"Region.hh"

#include	"TestSuite.hh"



static	void	Test1 ();


int	main (int /*argc*/, char* /*argv*/[])
{
	cout << "Testing regions..." << newline;
	Test1 ();
	cout << "Tested regions!" << newline;
	return (0);
}



static	void	Test1 ()
{
	Rect	r1		=	Rect (Point (10, 10), Point (10, 10));
	Rect	r2		=	Rect (Point (10, 10), Point (11, 11));
	Region	rgn1	=	r1;
	Region	rgn2	=	r2;
	TestCondition (rgn1 == r1);
	TestCondition (rgn1 != rgn2);
	TestCondition (r1 == rgn1.GetBounds ());
	TestCondition (r2 == rgn2.GetBounds ());
	Region	rgn3	=	rgn1 + rgn2;
	TestCondition (rgn3 == r1 + r2);
	TestCondition (!rgn1.Empty ());
	TestCondition (Region (Rect (kZeroPoint, kZeroPoint)).Empty ());
	TestCondition (!(rgn2-rgn1).Empty ());
	TestCondition ((rgn1-rgn2).Empty ());
	TestCondition ((rgn1^rgn2) == (rgn2^rgn1));
	TestCondition (!(rgn1^rgn2).Empty ());
	TestCondition ((rgn1+rgn2) == rgn2);	// cuz of particular regions chosen above - some orig - second one bigger
	TestCondition ((rgn1^rgn2) == ((rgn1+rgn2) - (rgn1*rgn2)));		/* definition of XOR */

	/* Test Get/SetBounds for regions */
	{
		Rect	originalRect	=	Rect (Point (1, 1), Point (500, 500));
		Region	originalRegion	=	originalRect;

		TestCondition (originalRect == originalRegion.GetBounds ());
		originalRegion.SetBounds (originalRect);
		TestCondition (originalRect == originalRegion.GetBounds ());

		Rect	rect2	=	Rect (Point (2, 2), Point (250, 250));
		Region	r2		=	originalRegion;
	}
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


