/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Random__
#define	__Random__

/*
 * $Header: /fuji/lewis/RCS/Random.hh,v 1.3 1992/10/10 03:16:25 lewis Exp $
 *
 * Description:
 *		Suite of random number utilities - these simply wrap rand(). They
 *	assure that srand () has been called. If you want a predictable series
 *	of numbers, call SeedRandom (int seed).
 *
 *		Note that we dont overload the name rand() since we want these things
 *	to have sensible return values - since the value of rand is limmited
 *	to fitting in a UInt16 and being non-negative, it seems unreasonable
 *	to return an int as the ansi version does. Since we cannot overload on
 *	return value, the only practical alternative seemed to be a new
 *	(and probably better) name.
 *
 *
 * TODO:
 *
 * Changes:
 *	$Log: Random.hh,v $
 *		Revision 1.3  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *
 *
 */

#include	<math.h>

#include	"Config-Foundation.hh"


void		SeedRandom (int seed = 0);			// default is totally random, so rarely needed (if zero, then random seed)

UInt16		Random ();							// returns zero to 2^15-1 (same as ANSI rand ())
UInt16		Random (UInt16 from, UInt16 to);	// return a random number in this range


/*
 * compute nRolls random numbers, add them and return result
 */
UInt32		RandomRollDice (UInt16 from, UInt16 to, UInt16 nRolls);


/*
 * same as RandomRollDice (), but divide by nRolls - useful to get normalally distributed number from "from" to "to"
 */
UInt16		RandomAverage (UInt16 from, UInt16 to, UInt16 nRolls);




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Random__*/
