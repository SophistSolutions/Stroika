/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ColorLookupTable.cc,v 1.5 1992/11/25 22:39:43 lewis Exp $
 *
 * TODO:
 *
 *		-	Make want to make ITables shared-Shared, since they do get copied occasionally, and
 *			can get large.
 *
 *		-	Do testsuite to be sure inverse table stuff working acceptably.
 *
 *		-	To Abstract/Portable/MacNative dichotmy for ITable stuff.
 *
 *		-	Ask sterl why he made the change he did to ColorLookupTable::Lookup (const Color& c, UInt32 (*distance) (const Color& lhs, const Color& rhs)) const:
 *			See if he has reason why he wanted to make routine to < instead of <=...
 *
 * Changes:
 *	$Log: ColorLookupTable.cc,v $
 *		Revision 1.5  1992/11/25  22:39:43  lewis
 *		Start conversion to new Genclass based template support.
 *
 *		Revision 1.4  1992/09/11  18:41:55  sterling
 *		new Shape stuff, got rid of String Peek references
 *
 *		Revision 1.3  1992/09/05  16:14:25  lewis
 *		Renamed Nil->Nil.
 *
 *		Revision 1.2  1992/09/01  15:36:53  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.23  92/06/09  12:38:09  12:38:09  lewis (Lewis Pringle)
 *		Renamed SmartPointer back to Shared.
 *		
 *		Revision 1.22  92/06/09  02:17:49  02:17:49  lewis (Lewis Pringle)
 *		Merged in sterls Shared/SmartPointer changes. Also, he changed Lookup routine to
 *		use < again - someday soon he can explain why :-).
 *		
 *		Revision 1.21  1992/05/23  00:09:46  lewis
 *		#include BlockAllocated instead of Memory.hh
 *
 *		Revision 1.20  92/05/18  16:53:41  16:53:41  lewis (Lewis Pringle)
 *		Get rid of old code - add asserts, and get rid of compiler warnings.
 *		Noticed why sterl as somewhat unhappy with CLUT::Lookup and fixed it - never did anything for macGDI case!
 *		Put in ifdefs for qRealTemplatesAvailable.
 *		
 *		Revision 1.19  92/05/13  01:51:16  01:51:16  lewis (Lewis Pringle)
 *		STERL: Added ensures and tried to change CLUT::Lookup - see todo list for comment was towhat sterl changed - ask why???
 *		
 *		Revision 1.18  92/05/13  00:34:22  00:34:22  lewis (Lewis Pringle)
 *		Added asserts to assure inverse table built properly.
 *		
 *		Revision 1.16  92/04/30  03:42:38  03:42:38  lewis (Lewis Pringle)
 *		Add new TODO items.
 *		Get rid of fWeOwnClut field for non-macgdi.
 *		Make more terse code in InverseTable::Lookup - speed tweek.
 *		
 *		Revision 1.15  92/04/29  12:20:18  12:20:18  lewis (Lewis Pringle)
 *		Added InverseTable::InverseTable (const InverseTable& from) and InverseTable::operator= (const InverseTable& rhs).
 *		Also, did better job figuring a default resolution for the inverse table.
 *		
 *		Revision 1.14  92/04/17  17:10:16  17:10:16  lewis (Lewis Pringle)
 *		Implemented portable inverse tables.
 *		
 *		Revision 1.13  92/04/17  12:37:07  12:37:07  sterling (Sterling Wight)
 *		Do quick test of equality for CLUTs with same shared rep. Also, op!= was broken - dont fix - just call not (==).
 *		
 *		Revision 1.12  92/04/07  01:02:19  01:02:19  lewis (Lewis Pringle)
 *		Color compare function for lookup now returns UInt32.
 *		
 *		Revision 1.10  1992/02/12  03:24:56  lewis
 *		Code cleanups, and use GDIConfig rather than OSConfig.
 *
 *
 */


#include	"OSRenamePre.hh"
#if		qMacGDI
#include	<Memory.h>
#include	<OSUtils.h>
#include	<QDOffscreen.h>
#include	<QuickDraw.h>
#endif	/*qMacGDI*/
#include	"OSRenamePost.hh"

#include	"BlockAllocated.hh"
#include	"Debug.hh"
#include	"Math.hh"
#include	"Memory.hh"
#include	"StreamUtils.hh"

#include	"GDIConfiguration.hh"

#include	"ColorLookupTable.hh"




#if		qMacGDI
inline	void	ClobberCTable (osColorTable** osCTab)
	{
		RequireNotNil (osCTab);
		if (GDIConfiguration ().ColorQD32Available ()) {
			::DisposeCTable (osCTab);
#if		qDebug
			OSErr e = MemError ();
			if (e != 0) {
				DebugMessage ("e = %d", e);
			}
#endif	/*qDebug*/
			Assert (MemError () == 0);
			Assert (QDError () == 0);
		}
		else {
			::DisposHandle (osHandle (osCTab));
			Assert (MemError () == noErr);
		}
	}
#endif	/*qMacGDI*/






/*
 ********************************************************************************
 *********************************** CLUTSharedPart *****************************
 ********************************************************************************
 */

#if		!qRealTemplatesAvailable
	#include "TFileMap.hh"
	#include BlockAllocatedOfCLUTSharedPart_hh
#endif

void*	CLUTSharedPart::operator new (size_t n)
{
#if		qRealTemplatesAvailable
	return (BlockAllocated<CLUTSharedPart>::operator new (n));
#else	/*qRealTemplatesAvailable*/
	return (BlockAllocated(CLUTSharedPart)::operator new (n));
#endif	/*qRealTemplatesAvailable*/
}

void	CLUTSharedPart::operator delete (void* p)
{
#if		qRealTemplatesAvailable
	BlockAllocated<CLUTSharedPart>::operator delete (p);
#else	/*qRealTemplatesAvailable*/
	BlockAllocated(CLUTSharedPart)::operator delete (p);
#endif	/*qRealTemplatesAvailable*/
}

#if		qMacGDI
CLUTSharedPart::CLUTSharedPart (osColorTable** anOSColorLookupTable, Boolean weOwnCLUT):
	fWeOwnCLUT (weOwnCLUT),
	fCTabHandle (anOSColorLookupTable)
{
	RequireNotNil (anOSColorLookupTable);
}
#elif	qXGDI
CLUTSharedPart::CLUTSharedPart (ColorIndex count, Color* colors):
	fColorCount (count),
	fColors (colors)
{
	RequireNotNil (colors);
}
#endif	/*GDI*/

CLUTSharedPart::~CLUTSharedPart ()
{
#if		qMacGDI	
	if (fWeOwnCLUT) {
		AssertNotNil (fCTabHandle);
		ClobberCTable (fCTabHandle);
	}
#elif	qXGDI
	delete (fColors);
#endif	/*GDI*/
}










/*
 ********************************************************************************
 ********************************* ColorLookupTable *****************************
 ********************************************************************************
 */

ColorLookupTable::ColorLookupTable ():
	fSharedPart (Nil)
{
	/*
	 * Make one shared copy of this, and just re-reference it for each call to this constructor.
	 */
// why does this need to be a pointer to a smart pointer? LGP June 8, 1992
#if		qRealTemplatesAvailable
	static	Shared<CLUTSharedPart>*	kDefaultCLUT = Nil;
#else
	static	Shared(CLUTSharedPart)*	kDefaultCLUT = Nil;
#endif
	if (kDefaultCLUT == Nil) {
#if		qMacGDI
		osColorTable**		aCTabHandle	=	Nil;
		if ((aCTabHandle = (struct osColorTable**)::NewHandle (sizeof (osColorTable) - sizeof (osColorSpec))) == Nil) {
			gMemoryException.Raise (MemError ());
		}
		if (GDIConfiguration ().ColorQDAvailable ()) {
			(*aCTabHandle)->ctSeed = ::GetCTSeed ();
		}
		(*aCTabHandle)->ctFlags = 0;
		(*aCTabHandle)->ctSize = -1;		// implies zero entries
		if (GDIConfiguration ().ColorQD32Available ()) {
			::CTabChanged (aCTabHandle);
		}
#if		qRealTemplatesAvailable
		kDefaultCLUT = new Shared<CLUTSharedPart> (new CLUTSharedPart (aCTabHandle, True));
#else
		kDefaultCLUT = new Shared(CLUTSharedPart) (new CLUTSharedPart (aCTabHandle, True));
#endif
#elif	qXGDI
#if		qRealTemplatesAvailable
		kDefaultCLUT = new Shared<CLUTSharedPart> (new CLUTSharedPart (0, new Color[0]));
#else
		kDefaultCLUT = new Shared(CLUTSharedPart) (new CLUTSharedPart (0, new Color[0]));
#endif
#endif /*GDI*/
	}
	EnsureNotNil (kDefaultCLUT);
	fSharedPart = *kDefaultCLUT;
}

#if		qMacGDI
ColorLookupTable::ColorLookupTable (struct osColorTable** anOSColorLookupTable):
	fSharedPart (Nil)
{
	RequireNotNil (anOSColorLookupTable);
	fSharedPart = new CLUTSharedPart (anOSColorLookupTable, False);
}
#endif	/*qMacGDI*/

ColorLookupTable::ColorLookupTable (const Color colorList[], ColorIndex nColorListEntries):
	fSharedPart (Nil)
{
#if		qMacGDI
	osColorTable**		aCTabHandle	=	Nil;
	if ((aCTabHandle = (struct osColorTable**)::NewHandle (sizeof (osColorTable) - sizeof (osColorSpec))) == Nil) {
		gMemoryException.Raise (MemError ());
	}
	if (GDIConfiguration ().ColorQDAvailable ()) {
		(*aCTabHandle)->ctSeed = ::GetCTSeed ();
	}
	(*aCTabHandle)->ctFlags = 0;
	(*aCTabHandle)->ctSize = -1;		// implies zero entries
	fSharedPart = new CLUTSharedPart (aCTabHandle, True);
#elif	qXGDI
	fSharedPart = new CLUTSharedPart (0, new Color[0]);
#endif	/*GDI*/
	SetEntryCount (nColorListEntries);
	for (ColorIndex i = 1; i <= nColorListEntries; i++) {
		SetEntry (i, colorList [i-1]);
	}
}

ColorLookupTable::ColorLookupTable (const ColorLookupTable& colorLookupTable):
	fSharedPart (colorLookupTable.fSharedPart)
{
}

ColorLookupTable::~ColorLookupTable ()
{
}

const ColorLookupTable&	ColorLookupTable::operator= (const ColorLookupTable& colorLookupTable)
{
	fSharedPart = colorLookupTable.fSharedPart;
	return (*this);
}

ColorIndex	ColorLookupTable::GetEntryCount () const
{
#if		qMacGDI
	const	osColorTable**	osctab	=	PeekOSRepresention ();
	// assert whole number of colorspec entries...
	Assert (((::GetHandleSize (osHandle (osctab)) - (sizeof (osColorTable) - sizeof (osColorSpec))) % sizeof (osColorSpec)) == 0);
	// assert number of colorspec entries == stored size
	Assert (((::GetHandleSize (osHandle (osctab)) - (sizeof (osColorTable) - sizeof (osColorSpec))) / sizeof (osColorSpec)) == (*osctab)->ctSize+1);
	return ((*osctab)->ctSize + 1);	// See IM - V - Page 136
#elif	qXGDI
	return (fSharedPart->fColorCount);
#endif	/*GDI*/
}

void	ColorLookupTable::SetEntryCount (ColorIndex entryCount, const Color& fillWith)
{
	ColorIndex		oldEntryCount	=	GetEntryCount ();

	if (oldEntryCount != entryCount) {
		Assure1Reference ();
#if		qMacGDI
		osColorTable**	osctab			=	GetOSRepresention ();
		::SetHandleSize (osHandle (osctab), sizeof (osColorTable) + (int (entryCount)-1) * sizeof (osColorSpec));
		if (MemError () != noErr) {
			gMemoryException.Raise (MemError ());
		}
		/*
		 * Init new entries, if any.
		 */
		for (ColorIndex i = oldEntryCount; i < entryCount; i++) {
			// value defined rather bizarrely IM-V-137
			// make value left aligned in word (brain damaged?).
			// Actually, I believe that nonsense only applied to CLUTs in the Apple, 13inch monitor,
			// and did not apply generally. In fact, for user cluts, it appears to be much
			// more common to have the value == index (this may even be require, but who knows?)
			UInt16			value	=	UInt16 (i);
			(*osctab)->ctTable [i].value = value;
			fillWith.GetRGB ((*osctab)->ctTable [i].rgb.red,
							 (*osctab)->ctTable [i].rgb.green,
							 (*osctab)->ctTable [i].rgb.blue);
		}
		(*osctab)->ctSize = short (entryCount)-1;
		if (GDIConfiguration ().ColorQD32Available ()) {
			::CTabChanged (osctab);
		}
#elif	qXGDI
		// Since Colors don't have destructors/constructors that do anyhting we care about, just call
		// realloc here...
		fSharedPart->fColorCount = entryCount;
		fSharedPart->fColors = (Color*)ReAlloc (fSharedPart->fColors, sizeof (Color)*entryCount);
		/*
		 * Init new entries, if any.
		 */
		for (ColorIndex i = oldEntryCount; i < entryCount; i++) {
			fSharedPart->fColors[i] = fillWith;
		}
#endif	/*GDI*/
	}
	Ensure (GetEntryCount () == entryCount);
}

#if		qMacGDI
// XGDI version inlined...
Color	ColorLookupTable::operator[] (ColorIndex i) const
{
	Require (i >= 1);
	Require (i <= GetEntryCount ());
	const	osColorTable**	osc		=	PeekOSRepresention ();
	return (Color ((*osc)->ctTable [i-1].rgb.red,
				   (*osc)->ctTable [i-1].rgb.green,
				   (*osc)->ctTable [i-1].rgb.blue
				));
}
#endif	/*qMacGDI*/

ColorIndex	ColorLookupTable::Lookup (const Color& c) const
{
	// why not just always have this guy search - why build the itable on the mac? less portable, more
	// work ??? LGP 11/4/91

// get rid of itable stuff from here - isntead to abtractITable, native and portable, and use ITable from
// bitblit code as needed... LGP April 29, 1992

// Leave this code here for now ifdefed out so I have a place to copy from when implementing native
// version for Mac... LGP May 13, 1992
#if		qMacGDI && 0
	const	kITabRes	=	4;				// ave number (see IM - V - 137-140,142)
	ITabHandle	ith	=	(ITabHandle)::NewHandle (0);
	if (MemError () != noErr) {
		gMemoryException.Raise (MemError ());
	}
	AssertNotNil (ith);

	::MakeITable ((osColorTable**)PeekOSRepresention (), ith, kITabRes);
	if (MemError () != noErr) {
		gMemoryException.Raise (MemError ());
	}
	if (::QDError () != noErr) {
	// really should have qd error exception???
		gMemoryException.Raise (::QDError ());
	}

	RGBColor	rbgColor;
	Assert (kITabRes == 4);	// assumed in catIndex computation
	c.GetRGB (rbgColor.red, rbgColor.green, rbgColor.blue);
	UInt16	catIndex = (rbgColor.red>>4) + (rbgColor.green>>8) + (rbgColor.blue>>12);
	ColorIndex	result	=	(*ith)->iTTable [catIndex];
	::DisposHandle (osHandle (ith));
	return (result);
#endif
	return (Lookup (c, EuclideanRGBDistance));
}

ColorIndex	ColorLookupTable::Lookup (const Color& c, UInt32 (*distance) (const Color& lhs, const Color& rhs)) const
{
	RequireNotNil (distance);
	Require (GetEntryCount () != 0);
	Require (sizeof (UInt32) > sizeof (ColorComponent));		// cuz otherwise we have overflow problems

	UInt32			winningDistance =	kMaxComponent;
	ColorIndex		winningIndex	=	1;
	ColorIndex		entryCount		=	GetEntryCount ();
	for (register ColorIndex i = 1; i <= entryCount; i++) {
		UInt32 dist = (*distance) (c, (*this)[i]);
		// Lewis wants a comment here, explaining why to use <, but I hardly know
		// what to say. < seems clearly better than <=. I think he is just confused.
// OK, I admit it - I'm confused. Maybe an explanation would help. LGP June 6, 1992...:-).
		if (dist < winningDistance) {	
			winningIndex = i;
		    winningDistance = dist;
		}
	}

	Ensure (winningIndex >= 1);
	Ensure (winningIndex <= GetEntryCount ());

	return (winningIndex);
}

void	ColorLookupTable::SetEntry (ColorIndex at, const Color& c)
{
	Require (at >= 1);
	Require (at <= GetEntryCount ());
	Assure1Reference ();
#if		qMacGDI
	osColorTable**	osc			=	GetOSRepresention ();
	char			oldHState	=	::HGetState (osHandle (osc));
	::HLock (osHandle (osc));
	c.GetRGB ((*osc)->ctTable [at-1].rgb.red,
		      (*osc)->ctTable [at-1].rgb.green,
			  (*osc)->ctTable [at-1].rgb.blue);
	::HSetState (osHandle (osc), oldHState);
	if (GDIConfiguration ().ColorQD32Available ()) {
		::CTabChanged (osc);
	}
#elif	qXGDI
	fSharedPart->fColors [at-1] = c;
#endif	/*GDI*/
	Ensure (this->operator[] (at) == c);
}

#if		qMacGDI
osColorTable**	ColorLookupTable::GetOSRepresention ()		
{ 	
	Assure1Reference ();
	EnsureNotNil (fSharedPart->fCTabHandle); 
	return (fSharedPart->fCTabHandle); 
}

const osColorTable**	ColorLookupTable::PeekOSRepresention ()	const	
{
	EnsureNotNil (fSharedPart->fCTabHandle); 
	return (fSharedPart->fCTabHandle); 
}
#endif	/*qMacGDI*/

void	ColorLookupTable::Assure1Reference ()
{
	Assert (fSharedPart.CountReferences () > 0);
	if (fSharedPart.CountReferences () != 1) {
		BreakReferences ();
	}
}

void	ColorLookupTable::BreakReferences ()
{
	/*
	 * This is usually called when someone is about to change the underlying ColorLookupTable, and multiple
	 * ColorLookupTable have references to the shared part. We must break these reference so only the
	 * intended copy gets changed.
	 */
	Require (fSharedPart.CountReferences () != 1);

#if		qRealTemplatesAvailable
	Shared<CLUTSharedPart>	oldSharedPart	=	fSharedPart;
#else
	Shared(CLUTSharedPart)	oldSharedPart	=	fSharedPart;
#endif
	/*
	 * Herein be careful to only blow away the old, and assign to fSharedPart where you're sure
	 * there will be no exceptions - probably need to be a bit more careful to avoid potential
	 * memory leaks
	 */
	{
#if		qMacGDI
		osHandle	newCLUT	=	osHandle (oldSharedPart->fCTabHandle);
		AssertNotNil (newCLUT);
		OSErr	err	=	::HandToHand (&newCLUT);
		if (err != noErr) {
			gMemoryException.Raise (err);
		}
		AssertNotNil (newCLUT);
		fSharedPart = new CLUTSharedPart ((osColorTable**)newCLUT, True);
	
		if (GDIConfiguration ().ColorQDAvailable ()) {
			(*GetOSRepresention ())->ctSeed = ::GetCTSeed ();
		}
		/*
		 * Always set to zero. (See IM - V - 136).  Even if we were from a device pmap, not now!
		 */
		(*GetOSRepresention ())->ctFlags = 0;
		if (GDIConfiguration ().ColorQD32Available ()) {
			::CTabChanged (GetOSRepresention ());
		}
#elif	qXGDI
		Color*	newColors = new Color [GetEntryCount ()];
		fSharedPart = new CLUTSharedPart (GetEntryCount (), new Color [GetEntryCount ()]);
		memcpy (fSharedPart->fColors, oldSharedPart->fColors, sizeof (Color)*GetEntryCount ());
#endif	/*GDI*/
	}
}








/*
 ********************************************************************************
 ******************************** iostream support ******************************
 ********************************************************************************
 */

ostream&	operator<< (ostream& out, const ColorLookupTable& clut)
{
	ColorIndex	entryCount	=	clut.GetEntryCount ();
	out << lparen << entryCount << ":";
	for (ColorIndex i = 1; i <= entryCount; i++) {
		out << clut[i];			// write out color
		if (i != entryCount) {
			out << " ";		// not sure extractor for int will skip...
		}
	}
	out << rparen;
	return (out);
}

istream&	operator>> (istream& in, ColorLookupTable& clut)
{
	char	ch;
	in >> ch;
	if (ch != lparen) {					// check for surrounding parens
		in.clear (ios::badbit | in.rdstate ());
		return (in);
	}

	ColorIndex	entryCount	=	0;
	in >> entryCount;

	char c;
	in.get (c);
	if (!in or (c != ':')) {
		in.clear (ios::badbit | in.rdstate ());
		return (in);
	}

	ColorLookupTable	aClut;
	aClut.SetEntryCount (entryCount);

	// now do forloop reading colors
	for (size_t i = 1; i <= entryCount; i++) {
		Color	c = Color (0, 0, 0);
		in >> c;
		aClut.SetEntry (i, c);
	}

	in >> ch;
	if (!in or (ch != rparen)) {		// check for surrounding parens
		in.clear (ios::badbit | in.rdstate ());
		return (in);
	}

	/*
	 * We win.
	 */
	Assert (in);
	clut = aClut;
	return (in);
}








/*
 ********************************************************************************
 *********************************** InverseTable *******************************
 ********************************************************************************
 */

InverseTable::InverseTable (const ColorLookupTable& clut, UInt8 resolution):
	fResolution (resolution),
	fColorIndexesTable (Nil)
{
	Require (resolution <= 4);		// all we support for now - zero special
	if (resolution == 0) {
		// default depends on size of CLUT.
		Assert ((log (clut.GetEntryCount ()) / log (2)) / 2 >= kMinUInt8);
		Assert ((log (clut.GetEntryCount ()) / log (2)) / 2 <= kMaxUInt8);
		fResolution = UInt8 (log (clut.GetEntryCount ()) / log (2)) / 2;		// log base 2 is number of sig bits in clut index
																		// /2 since otherwise # much too big - we dont want EXACT inv...

		if (fResolution == 0) {
			fResolution = 1;
		}
		if (fResolution >= 3) {	// 4 currently just takes too long... 6 seconds on a HP-730
			fResolution = 3;
		}
	}
	Ensure (fResolution <= 4);

	fColorIndexesTable = new ColorIndex [TableEntryCount ()];
	FillInTable (clut);
}

InverseTable::InverseTable (const InverseTable& from):
	fResolution (from.fResolution),
	fColorIndexesTable (Nil)
{
	fColorIndexesTable = new ColorIndex [TableEntryCount ()];
	memcpy (fColorIndexesTable, from.fColorIndexesTable, sizeof (fColorIndexesTable[1])*TableEntryCount ());
}

InverseTable::~InverseTable ()
{
	delete (fColorIndexesTable);
}

const InverseTable& InverseTable::operator= (const InverseTable& rhs)
{
	delete (fColorIndexesTable);
	fResolution = rhs.fResolution;
	fColorIndexesTable = new ColorIndex [TableEntryCount ()];
	memcpy (fColorIndexesTable, rhs.fColorIndexesTable, sizeof (fColorIndexesTable[1])*TableEntryCount ());
	return (*this);
}

ColorIndex	InverseTable::Lookup (const Color& c) const
{
	/*
	 * Get high order fResolution bits, and use these as an index.
	 */
	register	UInt8	shiftBy	=	8*sizeof(ColorComponent)-fResolution;

	size_t	newRed		=	size_t (c.GetRed ()) >> shiftBy;
	size_t	newGreen	=	size_t (c.GetGreen ()) >> shiftBy;
	size_t	newBlue		=	size_t (c.GetBlue ()) >> shiftBy;

	size_t	index	=	newRed + (newGreen << fResolution) + (newBlue << (2*fResolution));

	Require (index >= 0);
	Require (index < TableEntryCount ());

	return (fColorIndexesTable [index]);
}

size_t	InverseTable::TableEntryCount () const
{
	return (1<<(3*fResolution));
}

void	InverseTable::FillInTable (const ColorLookupTable& clut)
{
	/*
	 * Iterate over all items in table. Index on component, and possible values in the resolution,
  	 * make up colors, look them up inverse in the clut, and save the values.
	 */
	UInt16	nValsPerRes	=	1<<fResolution;
	for (size_t redPart = 0; redPart < nValsPerRes; redPart++) {
		for (size_t greenPart = 0; greenPart < nValsPerRes; greenPart++) {
			for (size_t bluePart = 0; bluePart < nValsPerRes; bluePart++) {
				size_t	index	=	redPart + (greenPart << fResolution) + (bluePart << (2*fResolution));
				Require (index >= 0);
				Require (index < TableEntryCount ());
				
				ColorComponent	red		=	redPart;
				ColorComponent	green	=	greenPart;
				ColorComponent	blue	=	bluePart;

				/*
				 * Tricky code to get high order fResolution bits.
				 */
				UInt8	shiftBy	=	8*sizeof(ColorComponent)-fResolution;
				red <<= shiftBy;
				green <<= shiftBy;
				blue <<= shiftBy;

				fColorIndexesTable [index] = clut.Lookup (Color (red, green, blue));
			}
		}
	}

#if		qDebug
	/*
	 * Ensure all our entries are valid...
	 */
	for (size_t index = 0; index < TableEntryCount (); index++) {
		Ensure (fColorIndexesTable [index] >= 1);
		Ensure (fColorIndexesTable [index] <= clut.GetEntryCount ());
	}
#endif	/*qDebug*/
}





/*
 ********************************************************************************
 ************************************** operator== ******************************
 ********************************************************************************
 */
Boolean	operator== (const ColorLookupTable& lhs, const ColorLookupTable& rhs)
{
	if (lhs.fSharedPart == rhs.fSharedPart) {
		return (True);
	}
	
	ColorIndex	entryCount	=	lhs.GetEntryCount ();
	if (entryCount != rhs.GetEntryCount ()) {
		return (False);
	}
	
	for (ColorIndex i = 1; i <= entryCount; i++) {
		if (lhs [i] != rhs [i]) {
			return (False);
		}
	}
	return (True);
}





/*
 ********************************************************************************
 ************************************** operator!= ******************************
 ********************************************************************************
 */
Boolean	operator!= (const ColorLookupTable& lhs, const ColorLookupTable& rhs)
{
	return (not operator== (lhs, rhs));
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

