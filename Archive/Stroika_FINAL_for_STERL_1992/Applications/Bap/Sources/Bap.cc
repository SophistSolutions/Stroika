/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * Has to come before other includes, so that we know this constructor called VERY early in
 * the game.
 */

#include	"OSInit.hh"
#pragma	push
#pragma	force_active	on
static	OSInit	sOSInit	=	OSInit ();
#pragma	pop

#include	"Stroika-Foundation-Globals.hh"

#include	"Debug.hh"
#include	"Integer.hh"
#include	"StreamUtils.hh"

int	main (int argc, char* argv[])
{
	cout << "Bap is an algebra package :-)" << newline;
	Integer i = 5;
	Integer j = 17;
	cout << i << newline;
	cout << j << newline;
	cout << "k = i + j" << newline;
	Integer k = i + j;
	cout << k << newline;
	Integer l = i * j;
	cout << l << newline;
	i = Real(6.02e23); /* Avogadro's constant */
	cout << i << newline;
	i = 5;
	k = i - j;
	cout << "k = i - j" << newline;
	cout << i << newline;
	cout << j << newline;
	cout << k << newline;
	k = j - i;
	cout << "k = j - i" << newline;
	cout << i << newline;
	cout << j << newline;
	cout << k << newline;
	cout << "Goodbye cruel world, I'm leaving you today, goodbye, goodbye, goodbye!" << newline;
	return (0);
}


// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***


