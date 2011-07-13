/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/CodeGenUtils.cc,v 1.4 1992/09/08 16:00:29 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: CodeGenUtils.cc,v $
 *		Revision 1.4  1992/09/08  16:00:29  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  15:58:05  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/07/01  04:08:09  lewis
 *		Remamed Strings.hh to String.hh.
 *
 *		Revision 1.1  1992/06/20  17:35:17  lewis
 *		Initial revision
 *
 *		Revision 1.5  1992/06/03  16:28:32  lewis
 *		Change format of routines that write out calls to PixelMapFromData - see PixelMap.cc:PixelMapFromData () for more info.
 *		Also, got rid of XPixelMapFromData hack.
 *
 *		Revision 1.4  92/05/01  01:33:30  01:33:30  lewis (Lewis Pringle)
 *		Hack up PixelMap writting to work for X - temp hacks XPixelMapFromData () routine.
 *		
 *		Revision 1.3  92/04/29  12:33:29  12:33:29  lewis (Lewis Pringle)
 *		Finish implementing routine to write out pixelmapfromdata call. Worked around compiler bugs in the
 *		genreated code!!!! Must init pointer to nil, and do the if ourselves, or it generates bogus temporary
 *		and destroyes it every time!!! and Inits it once (CFront 2.1). No big deal.
 *		Also, gen function rather than global object because of inheritent order of construction problems in C++.
 *		
 *		Revision 1.2  92/04/27  18:44:35  18:44:35  lewis (Lewis Pringle)
 *		Added WriteAsCString (ostream& out, const String& s) and WriteAsPixelMapFromData (ostream& out, const PixelMap& pm, const String& varName).
 *		
 *		Revision 1.1  92/04/16  12:37:02  12:37:02  lewis (Lewis Pringle)
 *		Initial revision
 *		
 *
 */

#include	<ctype.h>

#include	"Debug.hh"
#include	"Format.hh"
#include	"String.hh"
#include	"StreamUtils.hh"

#include	"ColorLookupTable.hh"
#include	"PixelMap.hh"

#include	"CodeGenUtils.hh"






/*
 ********************************************************************************
 ********************************** WriteAsCString ******************************
 ********************************************************************************
 */
ostream&	WriteAsCString (ostream& out, const String& s)
{
	const	kMaxCharsPerColumn	=	80;
	UInt8	column				=	0;
	Boolean	firstTime			=	True;

	out << "\"";
	for (CollectionSize i = 1; i <= s.GetLength (); i++) {
		// At column breaks, output closing quote, newline, and open quote - counts on ansi CPP string catenation...
		if (not firstTime and column % kMaxCharsPerColumn == 0) {
			out << "\"\n\"";
		}
		firstTime = False;

		char	c = s[i].GetAsciiCode ();
		if (isalpha (c) or isdigit (c) or (ispunct (c) and (c != '\\') and (c != '"')) or (c == ' ') or (c == '\t')) {
			out << c;
			column++;
		}
		else if (c == '\n') {
			out << "\\n";
			column += 2;
		}
		else if (c == '\r') {
			out << "\\r";
			column += 2;
		}
		else if (c == '\\') {
			out << "\\\\";
			column += 4;
		}
		else if (c == '\"') {
			out << "\\\"";
			column += 4;
		}
		else {
			String	sRep	=	ultostring (c, 8);	// base 8...
			Assert (sRep != kEmptyString);
			if (sRep [1] != '0') {
				sRep = String ("0") + sRep;
			}
			out << "\\" << sRep;
			column += (1 + sRep.GetLength ());
		}
	}
	out << "\"";
	return (out);
}






/*
 ********************************************************************************
 ******************************* WriteAsPixelMapFromData ************************
 ********************************************************************************
 */
ostream&	WriteAsPixelMapFromData (ostream& out, const PixelMap& pm, const String& varName)
{
// Currently we only implement CLUT based 1-8 depth, and no clut for depth=1 ==> black and white - no direct suported yet!

	/*
	 * First write any clut iff needed.
	 */
	Boolean	hasClut		=	False;			// the PixelMap has a clut
	Boolean	wroteClut	=	False;			// we wrote a clut out - if it was a standard one, we might not...
	String	clutName	=	kEmptyString;	// only used if we have a clut written out...
	switch (pm.GetPixelStyle ()) {
		case	PixelMap::eChunky: {
			hasClut = True;
			ColorLookupTable	clut	=	pm.GetColorLookupTable ();
			wroteClut = Boolean (clut != kBlackAndWhiteCLUT);
			if (wroteClut) {
				clutName = String ("k") + varName + "_ClutData";
				out << "static	const	Color	" << clutName << " [] = {\n";
				for (ColorIndex i = 1; i <= clut.GetEntryCount (); i++) {
					Color	c = clut[i];
					out << tab;
					if (c == kWhiteColor) {
						out <<"kWhiteColor";
					}
					else if (c == kBlackColor) {
						out <<"kBlackColor";
					}
					else if (c == kRedColor) {
						out <<"kRedColor";
					}
					else if (c == kGreenColor) {
						out <<"kGreenColor";
					}
					else if (c == kBlueColor) {
						out <<"kBlueColor";
					}
					else {
						out << "Color (" << c.GetRed () << ", " << c.GetGreen () << ", " << c.GetBlue () << ")";
					}
					out << "," << newline;
				}
				out << "};\n";
			}
		}
		break;
	}


	/*
	 * Now write the data for the pixelmap itself. Write it one row (ie pixel row) at a time.
	 */
	String	dataName		=	String ("k") + varName + "_Data";
	out << "static\tconst\tUInt8\t" << dataName << " [] = {\n";
	Coordinate	nRows		=	pm.GetBounds ().GetSize ().GetV ();

	Rect	pmBounds	=	pm.GetBounds ();
	unsigned pixelsPerByte = 8/pm.GetDepth ();
	if (pixelsPerByte > 1) {
		// Pack into bytes if depth small enuf
		for (Coordinate row = pmBounds.GetTop (); row < pmBounds.GetBottom (); row++) {
			out << tab;
			UInt8		aByte 		=	0;
			unsigned	packedSoFar	=	0;
			for (Coordinate col = pmBounds.GetLeft (); col < pmBounds.GetRight (); col++) {
				Pixel	pixel	=	pm.GetPixel (Point (row, col));

				aByte += (pixel << (packedSoFar*pm.GetDepth ()));
				packedSoFar ++;

				// when we've filled as much as possible into this byte, or were at the end of the
				// scanline, force out the byte.
				if (packedSoFar >= pixelsPerByte or (col == pmBounds.GetRight () - 1)) {
					out << "0x" << ultostring (aByte, 16);
					if ((col < pmBounds.GetRight () - 1) or (row < pmBounds.GetBottom ())) {
						out << ", ";
					}
					packedSoFar = 0;
					aByte = 0;
				}
			}
			out << newline;
		}
	}
	else {
		// write out individual pixels...
		for (Coordinate row = pmBounds.GetTop (); row < pmBounds.GetBottom (); row++) {
			out << tab;
			for (Coordinate col = pmBounds.GetLeft (); col < pmBounds.GetRight (); col++) {
				Pixel	pixel	=	pm.GetPixel (Point (row, col));

				Assert (sizeof (long) >= sizeof (pixel));
				out << "0x" << ultostring (pixel, 16);
				if ((col < pmBounds.GetRight () - 1) or (row < pmBounds.GetBottom ())) {
					out << ", ";
				}
			}
			out << newline;
		}
	}
	out << "};\n";

	out << dec;


	/*
	 * Now write the call to actually build the pixelmap, calling PixelMapFromData().
	 */
	Point	size	=	pm.GetBounds ().GetSize ();
	Depth	depth	=	pm.GetDepth ();
	// EG: static	const	PixelMap&	GetFred ()	{
	//				static	const	PixelMap*	kPM	= 	Nil;
	//				if (kPM == Nil) {
  	//					PixelMapFromData (Point (32, 32), kFred_Data, sizeof (kFred_Data), 2, kFred_ClutData));
	//				}
	//				AssertNotNil (kPM);
	//				return (*kPM);
	//		};
	out << "\nstatic\tconst\tPixelMap&\tGet" << varName << " ()\n";
	out << "{\n\tstatic\tconst\tPixelMap*\tkPM\t=\tNil;\n";
	out << "\tif (kPM == Nil) {\n\t\tkPM = new PixelMap (PixelMapFromData (";
	if (wroteClut) {
		// EG: // add size/array and clut/depth
		out <<
			"Point (" << size.GetV () <<  ", " << size.GetH () << "), " <<
			dataName << ", " <<
			"sizeof (" << dataName << "), " <<
			UInt32 (depth) << ", " <<
			clutName <<
			"));";
	}
	else {
		// EG: // add only size/array
		out <<
			"Point (" << size.GetV () <<  ", " << size.GetH () << "), " <<
			dataName << ", " <<
			"sizeof (" << dataName << "))); ";
	}
	out << "\n\t}\n\tEnsureNotNil (kPM);\n";
	out << "\treturn (*kPM);\n";
	out << "}\n";
	return (out);
}





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***
