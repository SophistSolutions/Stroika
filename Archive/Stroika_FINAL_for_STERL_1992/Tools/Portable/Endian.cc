/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Endian.cc,v 1.1 1992/06/20 17:40:19 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Endian.cc,v $
 *		Revision 1.1  1992/06/20  17:40:19  lewis
 *		Initial revision
 *
Revision 1.2  1992/04/29  23:45:43  lewis
*** empty log message ***

 *
 *
 */
#if		qIncludeRCSIDs
static	const	char	rcsid[]	=	"$Header: /fuji/lewis/RCS/Endian.cc,v 1.1 1992/06/20 17:40:19 lewis Exp $";
#endif

#include	<iostream.h>
#include	<stdlib.h>



static	int	BitOrder_LittleEndianP ();
static	int	ByteOrder_LittleEndianP ();

int	main (int argc, char* argv[])
{
	cout << "Testing to see if we are big or little endian\n";
	cout << "Bit order is: " << (BitOrder_LittleEndianP ()? "Little": "Big") << " Endian\n";
	cout << "Byte order is: " << (ByteOrder_LittleEndianP ()? "Little": "Big") << " Endian\n";
	return (0);
}





static	int	BitOrder_LittleEndianP ()
{
	// if we are little endian, then 1 becomes two when shifted left, and if we are
	// big endian, then it should get shifted off into lala land, and become zero
	unsigned	w	=	1;
	unsigned	x	=	w << 1;

	if (x == 0) {
		return (0);	// false
	}
	else if (x == 2) {
		return (1);	// true
	}
	else {
		cerr << "Well maybe I still dont understand this stuff\n";
		exit (1);
	}
}


static	int	ByteOrder_LittleEndianP ()
{
	unsigned long	value	=	0x44332211;	// hope long >= 4 bytes!!!
	char*		vp	=	(char*)&value;
	if ((vp[0] == 0x11) && (vp[1] == 0x22) && (vp[2] == 0x33) && (vp[3] == 0x44)) {
		return (0);	// false
	}
	else if ((vp[3] == 0x11) && (vp[2] == 0x22) && (vp[1] == 0x33) && (vp[0] == 0x44)) {
		return (1);	// true
	}
	else {
		cerr << "Well maybe I still dont understand this stuff\n";
		exit (1);
	}
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***
