/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Random.cc,v 1.3 1992/10/10 04:27:57 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Random.cc,v $
 *		Revision 1.3  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.2  1992/09/01  15:26:25  sterling
 *		Lots.
 *
 *		Revision 1.2  1992/05/18  15:10:37  lewis
 *		Add cast to SeedRandom call to avoid compiler warning.
 *		
 *
 *
 */



#include	<stdlib.h>

#include	"Debug.hh"
#include	"StreamUtils.hh"
#include	"Time.hh"

#include	"Random.hh"






/*
 ********************************************************************************
 ************************************* SeedRandom *******************************
 ********************************************************************************
 */

/*
 * Suite of random number utilities - these simply wrap rand(). They assure that
 * srand () has been called. If you want a predictable series of numbers, call
 * SeedRandom (int seed).
 */
static	Boolean	sSeedSown	=	False;
void	SeedRandom (int seed)
{
	sSeedSown = True;
	srand (seed);
}




/*
 ********************************************************************************
 ************************************* Random ***********************************
 ********************************************************************************
 */
UInt16	Random ()
{
	if (not sSeedSown) {
		SeedRandom (int (GetCurrentTime ().GetSeconds ()));
	}
	return (rand ());
}

UInt16	Random (UInt16 from, UInt16 to)
{
	Require (from <= to);
	return (from + (Random () % (to-from+1)));
}



/*
 ********************************************************************************
 ************************************* RandomRollDice ***************************
 ********************************************************************************
 */
UInt32	RandomRollDice (UInt16 from, UInt16 to, UInt16 nRolls)
{
	UInt32	sum	=	0;
	for (UInt16 i = 1; i <= nRolls; i++) {
		sum += Random (from, to);
	}
	return (sum);
}



/*
 ********************************************************************************
 ************************************* RandomAverage ****************************
 ********************************************************************************
 */

UInt16	RandomAverage (UInt16 from, UInt16 to, UInt16 nRolls)
{
	return (RandomRollDice (from, to, nRolls) / nRolls);
}








// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

