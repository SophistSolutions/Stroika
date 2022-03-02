/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/HelloWorld.cc,v 1.1 1992/12/07 04:03:15 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: HelloWorld.cc,v $
 *		Revision 1.1  1992/12/07  04:03:15  lewis
 *		Initial revision
 *
 *
 *
 *
 */
static	const	char	rcsid[]	=	"$Header: /fuji/lewis/RCS/HelloWorld.cc,v 1.1 1992/12/07 04:03:15 lewis Exp $";

/*
 * Has to come before other includes, so that we know this constructor called VERY early in
 * the game.
 */
#include    "OSInit.hh"
#pragma push
    #pragma force_active    on
    #if     qGCC_SimpleClassStaticInitializerBug
        static  OSInit  sOSInit;
    #else
        static  OSInit  sOSInit =   OSInit ();
    #endif
#pragma pop

#define		qUseAllGlobals		0
#define		qUseDebugGlobals	1
#include	"Stroika-Foundation-Globals.hh"

#include	<iostream.h>

#include	"Character.hh"
#include	"Sequence.hh"

#include	"HelloWorld.hh"


#if		!qRealTemplatesAvailable
	#include	"TFileMap.hh"
	#include	SequenceOfCharacter_hh
#endif


int	main (int argc, char* argv[])
{
#if		qRealTemplatesAvailable
	Sequence<Character>	hello;
#else
	Sequence(Character)	hello;
#endif
	hello.Append ('H');
	hello.Append ('e');
	hello.Append ('l');
	hello.Append ('l');
	hello.Append ('o');
	hello.Append (' ');
	ForEach (Character, it, hello) {
		cout << it.Current ();
	}
	cout << kWorld << endl;

	return (0);
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

