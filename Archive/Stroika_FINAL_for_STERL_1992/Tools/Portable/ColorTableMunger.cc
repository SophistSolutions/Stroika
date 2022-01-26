/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ColorTableMunger.cc,v 1.1 1992/06/20 17:40:19 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: ColorTableMunger.cc,v $
 *		Revision 1.1  1992/06/20  17:40:19  lewis
 *		Initial revision
 *
Revision 1.2  1992/05/22  23:24:48  lewis
Had to further munge output for lousy Apple C compiler.

Revision 1.1  92/05/20  00:26:26  00:26:26  lewis (Lewis Pringle)
Initial revision

 *
 *
 */
#if		qIncludeRCSIDs
static	const	char	rcsid[]	=	"$Header: /fuji/lewis/RCS/ColorTableMunger.cc,v 1.1 1992/06/20 17:40:19 lewis Exp $";
#endif

#include	<ctype.h>
#include	<iostream.h>
#include	<stdlib.h>




int	main (int argc, char* argv[])
{
	cout << "\t//\n";
	cout << "\t// Output of ColorTableMunger program operating on some X11 rgb.txt file\n";
	cout << "\t//\n";
	while (cin) {
		unsigned long	r;
		unsigned long	g;
		unsigned long	b;
		char	name[1024];
		cin >> r >> g >> b;
		r = double (r)*0xffff/0xff;
		g = double (g)*0xffff/0xff;
		b = double (b)*0xffff/0xff;
		while (cin) {		// skip whitespace
			int c = cin.get ();
			if (!isspace (c)) {
				cin.putback (c);
				break;
			}
		}
		cin.getline (name, sizeof (name));
		if (cin) {
			cout << "\tEnter_D (d, \"" << name << "\", " << r << ", " << g << ", " << b << ");\n";
		}
	}
	return (0);
}






// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***
