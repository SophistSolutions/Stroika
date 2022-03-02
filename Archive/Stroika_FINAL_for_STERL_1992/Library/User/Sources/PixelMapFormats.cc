/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/PixelMapFormats.cc,v 1.4 1992/09/08 16:00:29 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: PixelMapFormats.cc,v $
 *		Revision 1.4  1992/09/08  16:00:29  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  15:58:05  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/07/16  05:36:09  lewis
 *		Fixed minor bug in logic for parsing XPM files (and -> or).
 *
 *		Revision 1.1  1992/06/20  17:35:17  lewis
 *		Initial revision
 *
 *		Revision 1.10  1992/06/09  15:59:13  sterling
 *		reset size of dictionaries
 *
 *		Revision 1.9  1992/06/03  21:31:36  lewis
 *		Move a large hunk of code out of ReadXPMFromPixelmap and put it into ReadPixelMapLabelsFromXPM ()
 *		static function because of mac compiler bugs - it barfed saying too big. (function has
 *		versions of compiler).
 *
 *		Revision 1.8  1992/05/21  17:33:27  lewis
 *		React to change in definition of String::SubString().
 *
 *		Revision 1.7  92/05/20  01:29:52  01:29:52  lewis (Lewis Pringle)
 *		Fixed minor bug wring XPM format files.
 *		
 *		Revision 1.4  92/05/19  13:41:56  13:41:56  sterling (Sterling Wight)
 *		moved Mapping def to NamedColors
 *		
 *		Revision 1.3  92/05/19  11:56:27  11:56:27  lewis (Lewis Pringle)
 *		Implemented read XPM/XMP2, and write of XPM (write of xpm2 coming soon).
 *		
 *		Revision 1.2  92/05/18  19:10:29  19:10:29  lewis (Lewis Pringle)
 *		Worked on reader for XPM.
 *
 *		Revision 1.1  92/05/18  18:28:20  18:28:20  lewis (Lewis Pringle)
 *		Initial revision
 *
 *
 *
 */

#include	<ctype.h>

#include	"Debug.hh"
#include	"Format.hh"
#include	"StreamUtils.hh"

#include	"PixelMap.hh"

#include	"NamedColors.hh"
#include	"PixelMapFormats.hh"








static	istream&	ReadPixelMapLabelsFromXPM (istream& from, unsigned* format, unsigned* width, unsigned* height,
							unsigned* nColors, unsigned* nCharsPerPixel, String* pixelMapName);






#if		qRealTemplatesAvailable
ostream&	operator<< (ostream& out, const DictElt<String, Color>& elt)
{
	out << "(" << elt.fKey << ", " << elt.fElt << ")";
	return (out);
}
#else
ostream&	operator<< (ostream& out, const MapElement(String, Color)& elt)
{
	out << "(" << elt.fKey << ", " << elt.fElt << ")";
	return (out);
}
ImplementIOSreamOperators(MapElement(String, Color));
#endif








/*
 ********************************************************************************
 ******************************** ReadPixelMapFromXPM ***************************
 ********************************************************************************
 */
istream&	ReadPixelMapFromXPM (istream& from, PixelMap& into, String& name)
{
	/*
	 * Read the header for the file. Then validate items in the header. Don't try too hard.
	 */
	String		pixelMapName;
	unsigned	format;
	unsigned	width;
	unsigned	height;
	unsigned	nColors;
	unsigned	nCharsPerPixel;

	/*
	 * Break out into helper function since MPW C barfs compiling this function:
	 *
	 	# C - Fatal Error : 420
		# local symbol table overflow
		#--------------------------------------------------------------------------------------------------------------------------------
		    File ":Sources:PixelMapFormats.cc"; Find ¥!10037:¤!1; Open "{Target}"
		#--------------------------------------------------------------------------------------------------------------------------------
		# C - Aborted !
		### MPW Shell - Execution of /:fuji:lewis:Stroika_src:Library:User:Make.Out terminated.
	 *
  	 *		 C                 3.2.2     
 	 *		 CFront            3.2       
	 * LGP June 3, 1992.
	 */
	if (!ReadPixelMapLabelsFromXPM (from, &format, &width, &height, &nColors, &nCharsPerPixel, &pixelMapName)) {
		return (from);
	}




	/*
	 * Look for CLUT.
	 */
	{
		//eg.static  char * drunk_colors[] = {
		String	arrayDeclaration;
		from >> arrayDeclaration;
		if (arrayDeclaration != "static") {
			from.clear (ios::badbit | from.rdstate ());
			return (from);
		}
		from >> arrayDeclaration;
		if (arrayDeclaration != "char") {
			from.clear (ios::badbit | from.rdstate ());
			return (from);
		}
		from >> arrayDeclaration;
		if (arrayDeclaration != "*") {
			from.clear (ios::badbit | from.rdstate ());
			return (from);
		}
		from >> arrayDeclaration;		// eg. drunk_colors
		if (arrayDeclaration.SubString (pixelMapName.GetLength () + 2, 6) != "colors") {
			from.clear (ios::badbit | from.rdstate ());
			return (from);
		}

		// Could check more but who cares -- skip to end of line...
		while (from) {
			int c = from.get ();
			if ((c == '\n') or (c == '\r')) {
				break;
			}
		}
	}


	/*
	 * Create a Mapping mapping Strings to Colors.
	 */
	Mapping_HashTable(String, Color)	aMap (&DefaultStringHashFunction);
	for (ColorIndex ci = 1; ci <= nColors; ci++) {
		// read the name(aka string that will appear in pixelmap to name a given color)...
		char	c;
		from >> c;
		if (c != '\"') {
			from.clear (ios::badbit | from.rdstate ());
			return (from);
		}
		String	nameBuf;
		nameBuf.SetLength (nCharsPerPixel);
		from.read ((char*)nameBuf.Peek (), nCharsPerPixel);
		from >> c;
		if (c != '\"') {
			from.clear (ios::badbit | from.rdstate ());
			return (from);
		}

		// read the color its mapped to
		from >> c;
		if (c != ',') {
			from.clear (ios::badbit | from.rdstate ());
			return (from);
		}
		from >> c;
		if (c != '\"') {
			from.clear (ios::badbit | from.rdstate ());
			return (from);
		}

		String	valBuf;
		while (from) {
			int c = from.get ();
			if (c == '\"') {		// closing quote
				// Got a complete line.... Add entry and then read til newline...
				aMap.Enter (nameBuf, StringToColor (valBuf));
				while (from) {
					int c = from.get ();
					if ((c == '\n') or (c == '\r')) {
						break;
					}
				}
				break;
			}
			else {
				valBuf += c;
			}
		}
	}

	// Skip "};"
	while (from) {
		int c = from.get ();
		if ((c == '\n') or (c == '\r')) {
			break;
		}
	}



	/*
	 * Build the CLUT.
	 */
	ColorLookupTable	clut;
	clut.SetEntryCount (nColors);
	{
		ColorIndex	i = 1;
		ForEach (MapElement(String, Color), it, aMap) {
			clut.SetEntry (i++, it.Current ().fElt);
		}
	}



	/*
	 * Build an appropriate pixelmap with the given clut.
	 */
	Depth		depth	=	Depth (ceil (log (nColors) / log (2)));
	if (depth <= 0 or depth >= 8) {
		from.clear (ios::badbit | from.rdstate ());
		return (from);
	}
	PixelMap	pm		=	PixelMap (PixelMap::eChunky, depth, Rect (kZeroPoint, Point (height, width)), Nil, clut);




	/*
	 * Look for pixels leader...
	 */
	{
		//eg.static  char * drunk_pixel[] = {
		String	arrayDeclaration;
		from >> arrayDeclaration;
		if (arrayDeclaration != "static") {
			from.clear (ios::badbit | from.rdstate ());
			return (from);
		}
		from >> arrayDeclaration;
		if (arrayDeclaration != "char") {
			from.clear (ios::badbit | from.rdstate ());
			return (from);
		}
		from >> arrayDeclaration;
		if (arrayDeclaration != "*") {
			from.clear (ios::badbit | from.rdstate ());
			return (from);
		}
		from >> arrayDeclaration;		// eg. drunk_pixels
		if (arrayDeclaration.SubString (pixelMapName.GetLength () + 2, 6) != "pixels") {
			from.clear (ios::badbit | from.rdstate ());
			return (from);
		}
		// Could check more but who cares -- skip to end of line...
		while (from) {
			int c = from.get ();
			if ((c == '\n') or (c == '\r')) {
				break;
			}
		}
	}


// Now read each line...
	for (Coordinate row = 0; row < height; row++) {
		char	ch;
		from >> ch;
		if (ch != '"') {
			from.clear (ios::badbit | from.rdstate ());
			return (from);
		}
		for (Coordinate col = 0; col < width; col++) {
			String	item;
			item.SetLength (nCharsPerPixel);
			for (CollectionSize i = 1; i <= nCharsPerPixel; i++) {
				item.SetCharAt (from.get (), i);
			}

			Color	c;
			if (aMap.Lookup (item, &c)) {
				pm.SetColor (Point (row, col), c);
			}
			else {
				from.clear (ios::badbit | from.rdstate ());
				return (from);
			}
		}
		from >> ch;
		if (ch != '"') {
			from.clear (ios::badbit | from.rdstate ());
			return (from);
		}
		// skip (possibly a comma and whitespace) to EOL.
		while (from) {
			int c = from.get ();
			if ((c == '\n') or (c == '\r')) {
				break;
			}
		}
	}


	// Skip "};"
	while (from) {
		int c = from.get ();
		if ((c == '\n') or (c == '\r')) {
			break;
		}
	}


	// We finally made it!!!
	if (from) {
		name = pixelMapName;
		into = pm;
	}
	return (from);
}




/*
 ********************************************************************************
 ******************************* WritePixelMapToXPM *****************************
 ********************************************************************************
 */
ostream&	WritePixelMapToXPM (ostream& to, const PixelMap& from, const String& name)
{
	unsigned	height	=	unsigned (from.GetBounds ().GetSize ().GetV ());
	unsigned	width	=	unsigned (from.GetBounds ().GetSize ().GetH ());
	ColorLookupTable	clut	=	from.GetColorLookupTable ();
	to << "#define\t" << name << "_format" << tab << 1 << newline;
	to << "#define\t" << name << "_width" << tab << width << newline;
	to << "#define\t" << name << "_height" << tab << height << newline;
	to << "#define\t" << name << "_ncolors" << tab << clut.GetEntryCount () << newline;
	to << "#define\t" << name << "_chars_per_pixel" << tab << 2 << newline;

	/*
	 * Write the clut.
	 */
	to << "static  char * " << name << "_colors[] = {" << newline;
	for (ColorIndex ci = 1; ci <= clut.GetEntryCount (); ci++) {
		// convert ci # to two digit # base 26...
		char name[3];
		name [0] = ((ci-1) / 26) + 'A';
		name [1] = ((ci-1) % 26) + 'A';
		name [2] = '\0';
		// special case white == space...
		if (clut[ci] == kWhiteColor) {
			name[0] = ' ';
			name[1] = ' ';
		}

		to << "\"" << name << "\" , \"" << ColorToXFormatColorName (clut[ci]) << "\"";
		if (ci != clut.GetEntryCount ()) {
			to << ",";
		}
		to << newline;
	}
	to << "} ;" << newline;



	/*
	 * Write the pixels.
	 */
	to << "static  char * " << name << "_pixels[] = {" << newline;
	for (Coordinate row = 0; row < height; row++) {
		to << "\"";
		for (Coordinate col = 0; col < width; col++) {
			ColorIndex ci = from.GetPixel (Point (row, col));
			// convert ci # to two digit # base 26...
			char name[3];
			name [0] = (ci / 26) + 'A';
			name [1] = (ci % 26) + 'A';
			name [2] = '\0';
			// special case white == space...
			if (clut[ci+1] == kWhiteColor) {
				name[0] = ' ';
				name[1] = ' ';
			}

			to << name;
		}
		to << "\"";
		if (row != height) {
			to << ",";
		}
		to << newline;
	}
	to << "} ;" << newline;


	return (to);
}





/*
 ********************************************************************************
 ******************************* ReadPixelMapFromXPM2 ***************************
 ********************************************************************************
 */
istream&	ReadPixelMapFromXPM2 (istream& from, PixelMap& into)
{
	/*
	 * Read the header for the file. Then validate items in the header. Don't try too hard.
	 */
	String		bangLine;

	// eg. ! XPM2  
	from >> bangLine;
	if (bangLine != "!XPM2") {
		if (bangLine == "!") {
			from >> bangLine;
			if (bangLine != "XPM2") {
				from.clear (ios::badbit | from.rdstate ());
			}
		}
		else {
			from.clear (ios::badbit | from.rdstate ());
		}
	}
	if (!from) return (from);

	unsigned	width;
	unsigned	height;
	unsigned	nColors;
	unsigned	nCharsPerPixel;

	from >> width >> height >> nColors >> nCharsPerPixel;
	if (!from) return (from);


	/*
	 * Check that the values are sane.
	 */
	// any format good for me???

	const	kMaxWidth	=	1000;		// anything bigger, assume its bogus...
	const	kMaxHeight	=	1000;		// anything bigger, assume its bogus...
	if ((width == 0) or (width > kMaxWidth)) {
		from.clear (ios::badbit | from.rdstate ());
		return (from);
	}
	if ((height == 0) or (height > kMaxHeight)) {
		from.clear (ios::badbit | from.rdstate ());
		return (from);
	}
	if ((nColors <= 0) or (nColors > 255)) {
		from.clear (ios::badbit | from.rdstate ());
		return (from);
	}
	if ((nCharsPerPixel <= 0) or (nCharsPerPixel > 2)) {
		from.clear (ios::badbit | from.rdstate ());
		return (from);
	}


	// skip to end of line...
	while (from) {
		int c = from.get ();
		if ((c == '\n') or (c == '\r')) {
			break;
		}
	}

	/*
	 * Look for CLUT.
	 *
	 * Create a Mapping mapping Strings to Colors.
	 */
	Mapping_HashTable(String, Color)	aMap (&DefaultStringHashFunction);
	for (ColorIndex ci = 1; ci <= nColors; ci++) {
		// read the name(aka string that will appear in pixelmap to name a given color)...
		String	nameBuf;
		nameBuf.SetLength (nCharsPerPixel);
		from.read ((char*)nameBuf.Peek (), nCharsPerPixel);

		// Not sure what the disembodied 'c' means???
		char c;
		from >> c;
		if (c != 'c') {
			from.clear (ios::badbit | from.rdstate ());
			return (from);
		}

		// read the color its mapped to
		String	valBuf;
		from >> valBuf;
		while (from) {	// skip to EOL
			int c = from.get ();
			if ((c == '\n') or (c == '\r')) {
				break;
			}
		}

		// Got a complete line.... Add entry and then read til newline...
		aMap.Enter (nameBuf, StringToColor (valBuf));
	}



	/*
	 * Build the CLUT.
	 */
	ColorLookupTable	clut;
	clut.SetEntryCount (nColors);
	{
		ColorIndex	i = 1;
		ForEach (MapElement(String, Color), it, aMap) {
			clut.SetEntry (i++, it.Current ().fElt);
		}
	}




	/*
	 * Build an appropriate pixelmap with the given clut.
	 */
	Depth		depth	=	Depth (ceil (log (nColors) / log (2)));
	if (depth <= 0 or depth >= 8) {
		from.clear (ios::badbit | from.rdstate ());
		return (from);
	}
	PixelMap	pm		=	PixelMap (PixelMap::eChunky, depth, Rect (kZeroPoint, Point (height, width)), Nil, clut);



	/*
	 * Look for pixels
	 * Now read each line...
	 */
	for (Coordinate row = 0; row < height; row++) {
		for (Coordinate col = 0; col < width; col++) {
			String	item;
			item.SetLength (nCharsPerPixel);
			for (CollectionSize i = 1; i <= nCharsPerPixel; i++) {
				item.SetCharAt (from.get (), i);
			}

			Color	c;
			if (aMap.Lookup (item, &c)) {
				pm.SetColor (Point (row, col), c);
			}
			else {
				from.clear (ios::badbit | from.rdstate ());
				return (from);
			}
		}
		// skip (possibly a comma and whitespace) to EOL.
		while (from) {
			int c = from.get ();
			if ((c == '\n') or (c == '\r')) {
				break;
			}
		}
	}

	// We finally made it!!!
	if (from) {
		into = pm;
	}
	return (from);
}





/*
 ********************************************************************************
 ******************************* WritePixelMapToXPM2 ****************************
 ********************************************************************************
 */
ostream&	WritePixelMapToXPM2 (ostream& to, const PixelMap& from)
{
	return (to);
}










/*
 ********************************************************************************
 ************************* ReadPixelMapLabelsFromXPM ****************************
 ********************************************************************************
 */

static	istream&	ReadPixelMapLabelsFromXPM (istream& from, unsigned* format, unsigned* width, unsigned* height,
							unsigned* nColors, unsigned* nCharsPerPixel, String* pixelMapName)
{
	RequireNotNil (format);
	RequireNotNil (width);
	RequireNotNil (height);
	RequireNotNil (nColors);
	RequireNotNil (nCharsPerPixel);
	RequireNotNil (pixelMapName);

	/*
	 * Read the header for the file. Then validate items in the header. Don't try too hard.
	 */
	String		formatLabel;
	String		widthLabel;
	String		heightLabel;
	String		nColorsLabel;
	String		nCharsPerPixelLabel;

	String	sharpDefine;

	// eg. #define drunk_format 1
	from >> sharpDefine >> formatLabel >> *format;
	if (sharpDefine != "#define") { from.clear (ios::badbit | from.rdstate ()); }
	if (!from) return (from);

	// eg. #define drunk_width 18
	from >> sharpDefine >> widthLabel >> *width;
	if (sharpDefine != "#define") { from.clear (ios::badbit | from.rdstate ()); }
	if (!from) return (from);

	// eg. #define drunk_height 21
	from >> sharpDefine >> heightLabel >> *height;
	if (sharpDefine != "#define") { from.clear (ios::badbit | from.rdstate ()); }
	if (!from) return (from);

	// eg. #define drunk_ncolors 4
	from >> sharpDefine >> nColorsLabel >> *nColors;
	if (sharpDefine != "#define") { from.clear (ios::badbit | from.rdstate ()); }
	if (!from) return (from);

	// eg. #define drunk_chars_per_pixel 2
	from >> sharpDefine >> nCharsPerPixelLabel >> *nCharsPerPixel;
	if (sharpDefine != "#define") { from.clear (ios::badbit | from.rdstate ()); }
	if (!from) return (from);


	
	/*
	 * Do a little more validation of the header we've read in. Get the pixelmap name,
	 * strip it from the front of the labels, and then be sure the labels are valid.
	 */
	*pixelMapName	=	formatLabel;
	if (pixelMapName->RIndexOf ('_') == kBadSequenceIndex or pixelMapName->RIndexOf ('_') == 1) {
		from.clear (ios::badbit | from.rdstate ());
		return (from);
	}
	else {
		pixelMapName->SetLength (pixelMapName->RIndexOf ('_')-1);
	}
	if (pixelMapName->GetLength () == 0) {
		from.clear (ios::badbit | from.rdstate ());
		return (from);
	}

	if (formatLabel.SubString (pixelMapName->GetLength () + 2) != "format") {
		from.clear (ios::badbit | from.rdstate ());
		return (from);
	}
	if (widthLabel.SubString (pixelMapName->GetLength () + 2) != "width") {
		from.clear (ios::badbit | from.rdstate ());
		return (from);
	}
	if (heightLabel.SubString (pixelMapName->GetLength () + 2) != "height") {
		from.clear (ios::badbit | from.rdstate ());
		return (from);
	}
	if (nColorsLabel.SubString (pixelMapName->GetLength () + 2) != "ncolors") {
		from.clear (ios::badbit | from.rdstate ());
		return (from);
	}
	if (nCharsPerPixelLabel.SubString (pixelMapName->GetLength () + 2) != "chars_per_pixel") {
		from.clear (ios::badbit | from.rdstate ());
		return (from);
	}


	/*
	 * Check that the values are sane.
	 */
	// any format good for me???

	const	kMaxWidth	=	1000;		// anything bigger, assume its bogus...
	const	kMaxHeight	=	1000;		// anything bigger, assume its bogus...
	if ((*width == 0) or (*width > kMaxWidth)) {
		from.clear (ios::badbit | from.rdstate ());
		return (from);
	}
	if ((*height == 0) or (*height > kMaxHeight)) {
		from.clear (ios::badbit | from.rdstate ());
		return (from);
	}
	if ((*nColors <= 0) or (*nColors > 255)) {
		from.clear (ios::badbit | from.rdstate ());
		return (from);
	}
	if ((*nCharsPerPixel <= 0) or (*nCharsPerPixel > 2)) {
		from.clear (ios::badbit | from.rdstate ());
		return (from);
	}


	return (from);
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

