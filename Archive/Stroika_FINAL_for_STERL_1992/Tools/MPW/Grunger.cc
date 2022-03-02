/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Grunger.cc,v 1.1 1992/09/01 18:05:53 sterling Exp $
 *
 * Description:
 *		Grunger <NO ARGS>
 *
 *		Grunger reads standard input and copies it to standard output replacing
 *	bad symbols with good. Its purpose is to patch bad symbol tables that MPW
 *	routinely builds on a Mac II Quadra. Nice.
 *
 *		EG:
 *			Grunger < Stroika.o > Stroika.o.GOOD
 *
 * NOTES:
 *		This algorithm assumes things being replaced all same size as what
 *	they are being replaced by.
 *
 *
 * TODO:
 *		-	Command line interface to specifying patches.
 *
 * Changes:
 *	$Log: Grunger.cc,v $
// Revision 1.1  1992/09/01  18:05:53  sterling
// Initial revision
//
 *
 *
 */
#if		qIncludeRCSIDs
static	const	char	rcsid[]	=	"$Header: /fuji/lewis/RCS/Grunger.cc,v 1.1 1992/09/01 18:05:53 sterling Exp $";
#endif

#include	<assert.h>
#include	<ctype.h>
#include	<iostream.h>
#include	<stdio.h>
#include	<strstream.h>
#include	<stdlib.h>


// This must be big enuf :-)
#define	kBigEnuf	100
struct	BadSym	{
	char	oldValue[kBigEnuf];
	char	newValue[kBigEnuf];
};

const	BadSym	kBadSyms[]	=	{
	{	"ÿÿÿ__FR3ios",		"dec__FR3ios"	}
};
const	int		kBadSymCount	=	sizeof (kBadSyms)/sizeof (kBadSyms[0]);

static	unsigned	GetLongestOldSym ();
static	void		DoOptionalReplacement (char* s, size_t len);

int	main (int argc, char* argv[])
{
	unsigned	maxOldSymLength	=	GetLongestOldSym ();

	if (argc > 1) {
		cerr << "Usage: " << argv[0] << " <NOARGS>\n";
		return (1);
	}

	// alloc Q
	char*	charQueue	=	new char [maxOldSymLength];
	memset (charQueue, 0, maxOldSymLength);

	// initialy fill Q(EXCEPT LAST CHAR SINCE THAT IS FILLED IN AT HEAD
	// of NEXT LOOP
	assert(maxOldSymLength>=1);
	for (int i = 0; i < maxOldSymLength-1; i++) {
		charQueue[i] = getc (stdin);
	}

	int	c;
	while ( ( c = getc (stdin) ) != EOF ) {
		charQueue[maxOldSymLength-1] = c;
		DoOptionalReplacement (charQueue, maxOldSymLength);

		// write the head of the Q
		putc (charQueue[0], stdout);

		// DEQ first elt
		memmove (&charQueue[0], &charQueue[1], maxOldSymLength-1);
	}

// write out the rest of the Q
	for (i = 0; i < maxOldSymLength-1; i++) {
		putc (charQueue[i], stdout);
	}
	return (0);
}




static	unsigned	GetLongestOldSym ()
{
	unsigned	maxGuy	=	0;
	for (int i = 0; i < kBadSymCount; i++) {
		if (strlen (kBadSyms[i].oldValue) > maxGuy) {
			assert (strlen (kBadSyms[i].oldValue) == strlen (kBadSyms[i].newValue));
			maxGuy = strlen (kBadSyms[i].oldValue);
		}
	}
	return (maxGuy);
}

static	void	DoOptionalReplacement (char* s, size_t len)
{
	for (int i = 0; i < kBadSymCount; i++) {
		for (int j = 0; j < len; j++) {
			if (strncmp (kBadSyms[i].oldValue, &s[i], strlen (kBadSyms[i].oldValue)) == 0) {
				cerr << "GOT A MATCH - REPLACING...\n";
				strncpy (&s[i],kBadSyms[i].newValue,strlen (kBadSyms[i].oldValue));
			}
		}
	}
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***
