/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Font.cc,v 1.5 1992/09/11 18:41:55 sterling Exp $
 *
 *
 * TODO:
 *		In the future, consider trap caching
 *
 *		IO Stream inserters/extranstoers not quite right - should write / read font style too
 *
 *		Constructors that don't specify everyting should not use hardwired defaults but should pick
 *		good default for that which is specified.
 *
 *		Get rid of cheatingThis crap - use casts in each place to avoid compiler optimizer bugs...
 *
 *		Go over shared reference couutnign, and BE SURE we break references when we are supposed to- ie when
 *		we make changes....LGP June 8, 1992
 *
 * Changes:
 *	$Log: Font.cc,v $
 *		Revision 1.5  1992/09/11  18:41:55  sterling
 *		new Shape stuff, got rid of String Peek references
 *
 *		Revision 1.4  1992/09/05  16:14:25  lewis
 *		Renamed Nil->Nil.
 *
 *		Revision 1.3  1992/09/01  15:36:53  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/06/25  04:12:36  sterling
 *		Add calls to SetLoad (False/True) around GetResInfo calls in FontIterator::ScanForNext..
 *		But THIS IS WRONG - SHOULD HAVE DONE AROUND GETRESOURCE CALL ABOVE!!! (LGP).
 *		Added GetFontHeight/refined GetLineHeight - check these defintions correspond to headers,
 *		and ask Tom if they make sense (LGP).
 *
 *		Revision 1.1  1992/06/19  22:34:01  lewis
 *		Initial revision
 *
 *		Revision 1.23  1992/06/09  12:38:13  lewis
 *		Renamed SmartPointer back to Shared.
 *
 *		Revision 1.22  92/06/09  03:13:31  03:13:31  lewis (Lewis Pringle)
 *		Merged in sterls changes for new SmartPointer based ReferenceCounting.
 *		
 *		Revision 1.21  92/05/23  00:10:05  00:10:05  lewis (Lewis Pringle)
 *		#include BlockAllocated instead of Memory.hh
 *		
 *		Revision 1.20  92/05/18  16:57:01  16:57:01  lewis (Lewis Pringle)
 *		Added ifdefs for qRealTemplatesAvailable for BlockAllocated.
 *		
 *		Revision 1.19  92/05/13  00:25:37  00:25:37  lewis (Lewis Pringle)
 *		Comment on FontSharedPart::DTOR being nonvirtual.
 *		
 *		Revision 1.18  92/05/10  00:23:05  00:23:05  lewis (Lewis Pringle)
 *		Fixed minor syntax errors porting to BC++/templates. And minor changes tocom
 *		pile under qWinGDI.
 *		
 *		Revision 1.17  92/05/09  00:20:42  00:20:42  lewis (Lewis Pringle)
 *		Comment on lousy semantics of font when we dont find font name - based on complaint by Bob.
 *		
 *		Revision 1.16  92/04/20  14:15:37  14:15:37  lewis (Lewis Pringle)
 *		added typedef char* caddr_t conditional on qSnake && _POSIX_SOURCE, before include of Xutil.h, since that
 *		file references caddr_t which doesn't appear to be part of POSIX.
 *		
 *		Revision 1.15  92/04/02  11:34:51  11:34:51  lewis (Lewis Pringle)
 *		Use gDisplay instead of Tablet::GetOSDisplay ().
 *		
 *		Revision 1.13  1992/02/21  18:05:52  lewis
 *		Got rid of qGPlus_ClassOpNewDelBroke workaround.
 *
 *		Revision 1.12  1992/01/22  04:16:38  lewis
 *		Use Tablet::GetOSDisplay ().
 *
 *		Revision 1.11  1992/01/20  17:17:13  sterling
 *		Tried to support kSystem font and finally ended up with a gross hack.
 *
 *		Revision 1.9  1992/01/18  09:03:24  lewis
 *		Added GetXFontStruct () method to font, and fontinfo, and kept the info around during a SetCacheValid.
 *		Then, delete it when the fontinfo guy is destroyed. I hope this is safe - what if anther fontinfo
 *		refers to the same ID. Code is safe so long as X always returns a unique font id.
 *
 *		Revision 1.8  1992/01/16  00:47:20  lewis
 *		Got to compile on mac.
 *
 *		Revision 1.6  1992/01/14  05:58:08  lewis
 *		Use a few more exceptions. Implement Fonts for UNIX. Use nested types more.
 *
 *		Revision 1.4  1991/12/04  19:41:05  lewis
 *		Hacked so for time being we get reasonbable font metric values...
 */

#include	"OSRenamePre.hh"
#if		qMacGDI
#include	<Fonts.h>
#include	<Memory.h>
#include	<OSUtils.h>
#include	<QuickDraw.h>
#include	<Resources.h>
#elif	qXGDI
#if		qSnake && _POSIX_SOURCE
typedef	char*	caddr_t;		// work around hp POSIX headers bug
#endif
#include	<X11/Xlib.h>
#include	<X11/Xutil.h>
#endif	/*GDI*/
#include	"OSRenamePost.hh"

#include	"BlockAllocated.hh"
#include	"Debug.hh"
#include	"Memory.hh"
#include	"StreamUtils.hh"

#include	"Tablet.hh"

#include	"Font.hh"




#if		!qRealTemplatesAvailable
	Implement (Iterator, FontStyle);
	Implement (Collection, FontStyle);
	Implement (AbSet, FontStyle);
	Implement (Array, FontStyle);
	Implement (Set_Array, FontStyle);
	Implement (Set_BitString, FontStyle);
	Implement (Set, FontStyle);
#endif


class	FontInfo {
	public:
#if		qBorlandCPlus
		FontInfo (const Font::Name& fontName, Font::Size fontSize, const AbSet<Font::Style>& fontStyleSet);
#else
		FontInfo (const FontName& fontName, FontSize fontSize, const AbSet(FontStyle)& fontStyleSet);
#endif

		/*
		 * Note that since the DTOR is nonvirtual, we must be careful about not keeping ptrs to this guys base class
		 * so that when we delete, our DTOR is called. Done this way for performance reasons.
		 */
		nonvirtual	~FontInfo ();
		
		nonvirtual	FontName		GetName () const;
		nonvirtual	void			SetName (const FontName& fontName);
		nonvirtual	FontSize		GetSize () const;
		nonvirtual	void			SetSize (FontSize fontSize);
#if		qRealTemplatesAvailable
		nonvirtual	const AbSet<Font::Style>&	GetStyle () const;
		nonvirtual	void					SetStyle (const AbSet<Font::Style>& fontStyleSet);
#else
		nonvirtual	const AbSet(FontStyle)&	GetStyle () const;
		nonvirtual	void					SetStyle (const AbSet(FontStyle)& fontStyleSet);
#endif

		nonvirtual	Coordinate	GetAscent () const;
		nonvirtual	Coordinate	GetDescent () const;
		nonvirtual	Coordinate	GetMinCharWidth () const;
		nonvirtual	Coordinate	GetMaxCharWidth () const;
		nonvirtual	Boolean		Monospace () const;
		nonvirtual	Coordinate	Proportional () const;
		nonvirtual	Coordinate	GetLeading () const;

#if		qMacGDI
		nonvirtual	short	GetID () const;						// OS representation
		nonvirtual	short	GetStyleOSRep () const;				// OS representation
#elif	qXGDI
		nonvirtual	unsigned long		GetID () const;			// OS representation
		nonvirtual	const XFontStruct*	GetXFontStruct () const;
#endif	/*qMacGDI*/

	protected:		
		nonvirtual	void	SetValidCache () const;
		nonvirtual	void	InvalidateCache ();

	private:
		FontName					fName;
		FontSize					fSize;
#if		qRealTemplatesAvailable
		Set_BitString<Font::Style>	fStyleSet;
#else
		Set_BitString(FontStyle)	fStyleSet;
#endif
#if		qMacGDI
		short						fID;
		short						fStyleOSRep;
#elif	qXGDI
		unsigned long				fID;
		XFontStruct*				fFontStruct;
#endif	/*qMacGDI*/
		Coordinate					fAscent;
		Coordinate					fDescent;
		Coordinate					fLeading;
		Coordinate					fMaxCharWidth;
		Boolean						fCacheValid;
};


#if		!qRealTemplatesAvailable
Implement (Shared, FontInfo);
#endif



#if		qMacGDI
static	osGrafPort*	GetWorkPort ()
{
	static	Boolean		inited		=	False;
	static	osGrafPort	theWorkPort;
	if (not inited) {
		memset (&theWorkPort, 0, sizeof (theWorkPort));
		::OpenPort (&theWorkPort);
		inited = True;
	}
	return (&theWorkPort);
}
#endif	/*qMacGDI*/








/*
 ********************************************************************************
 *************************************** Font ***********************************
 ********************************************************************************
 */

#if		!qRealTemplatesAvailable
BlockAllocatedDeclare (Font);
BlockAllocatedImplement (Font);
#endif	/*!qRealTemplatesAvailable*/

Exception	Font::sFontUnavailable;

Font::Font (const Font& font) :
	fFontInfo (font.fFontInfo)
{
}

Font::Font (const FontName& fontName) :
	fFontInfo (Nil)
{
	// This is wrong - if we specify no size, or style, pick the best one for that name!!! LGP JAN 13, 1992
	fFontInfo = new FontInfo (fontName, 12, kPlainFontStyle);
}

Font::Font (const FontName& fontName, FontSize fontSize) :
	fFontInfo (Nil)
{
	// This is wrong - if we specify no style, pick the best one for that name!!! LGP JAN 13, 1992
	fFontInfo = new FontInfo (fontName, fontSize, kPlainFontStyle);
}

#if		qRealTemplatesAvailable
Font::Font (const Font::Name& fontName, Font::Size fontSize, const AbSet <Font::Style>& fontStyleSet) :
	fFontInfo (Nil)
{
	fFontInfo = new FontInfo (fontName, fontSize, fontStyleSet);
}
#else
Font::Font (const FontName& fontName, FontSize fontSize, const AbSet (FontStyle)& fontStyleSet) :
	fFontInfo (Nil)
{
	fFontInfo = new FontInfo (fontName, fontSize, fontStyleSet);
}
#endif

#if		qMacGDI
Font::Font (short fontID, FontSize fontSize, short osStyleRep) :
	fFontInfo (Nil)
{
	osStr255	buffer;

	::GetFontName (fontID, buffer);
	String fontName = String (&buffer[1], buffer[0]);

	Set_BitString(FontStyle)	fontStyleSet;
	if (osStyleRep & bold) {
		fontStyleSet.Add (eBoldFontStyle);	
	}
	if (osStyleRep & italic) {
		fontStyleSet.Add (eItalicFontStyle);	
	}
	if (osStyleRep & underline) {
		fontStyleSet.Add (eUnderlineFontStyle);	
	}
	if (osStyleRep & outline) {
		fontStyleSet.Add (eOutlineFontStyle);	
	}
	if (osStyleRep & shadow) {
		fontStyleSet.Add (eShadowFontStyle);	
	}
	if (osStyleRep & condense) {
		fontStyleSet.Add (eCondenseFontStyle);	
	}
	if (osStyleRep & extend) {
		fontStyleSet.Add (eExtendFontStyle);	
	}
	fFontInfo = new FontInfo (fontName, fontSize, fontStyleSet);
}
#endif	/*qMacGDI*/

Font::~Font ()
{
}

Font&	Font::operator= (const Font& newFont)
{
	fFontInfo = newFont.fFontInfo;
	return (*this);
}

Name	Font::GetName () const
{
	return (fFontInfo->GetName ());
}

void	Font::SetName (const Name& fontName)
{
	if (fontName != GetName ()) {
		AssureOneReference ();
		fFontInfo->SetName (fontName);
	}
}

FontSize	Font::GetSize () const
{
	return (fFontInfo->GetSize ());
}

void	Font::SetSize (FontSize fontSize)
{
	if (fontSize != GetSize ()) {
		AssureOneReference ();
		fFontInfo->SetSize (fontSize);
	}
}

#if		qRealTemplatesAvailable
const AbSet<Font::Style>&	Font::GetStyle () const
{
	return (fFontInfo->GetStyle ());
}

void	Font::SetStyle (const AbSet<Font::Style>& fontStyleSet)
{
#if		!qBorlandCPlus
// bug workaround for borland -investigate later...LGP May 5, 1992
	if (fontStyleSet != GetStyle ()) {
		AssureOneReference ();
		fFontInfo->SetStyle (fontStyleSet);
	}
#endif
}

#else	/*qRealTemplatesAvailable*/

const AbSet(FontStyle)&	Font::GetStyle () const
{
	return (fFontInfo->GetStyle ());
}

void	Font::SetStyle (const AbSet(FontStyle)& fontStyleSet)
{
	if (fontStyleSet != GetStyle ()) {
		AssureOneReference ();
		fFontInfo->SetStyle (fontStyleSet);
	}
}
#endif	/*qRealTemplatesAvailable*/

#if		qMacGDI
short	Font::GetID () const			
{ 
	return (fFontInfo->GetID ());
}

short	Font::GetStyleOSRep () const	
{ 
	return (fFontInfo->GetStyleOSRep ());
}
#elif	qXGDI
unsigned long	Font::GetID () const			
{ 
	return (fFontInfo->GetID ());
}

const void*	Font::GetXFontStruct () const
{
	return (fFontInfo->GetXFontStruct ());
}
#endif	/*qMacGDI*/

Coordinate	Font::GetAscent () const
{
	return (fFontInfo->GetAscent ());
}

Coordinate	Font::GetDescent () const
{
	return (fFontInfo->GetDescent ());
}

Coordinate	Font::GetFontHeight () const
{
	return (GetAscent () + GetDescent ());
}

Coordinate	Font::GetMinCharWidth () const
{
	return (fFontInfo->GetMinCharWidth ());
}

Coordinate	Font::GetMaxCharWidth () const
{
	return (fFontInfo->GetMaxCharWidth ());
}

Coordinate	Font::GetLeading () const
{
	return (fFontInfo->GetLeading ());
}

Coordinate	Font::GetLineHeight () const
{
	return (GetFontHeight () + GetLeading ());
}

void*	Font::operator new (size_t n)
{
#if		qRealTemplatesAvailable
	return (BlockAllocated<Font>::operator new (n));
#else	/*qRealTemplatesAvailable*/
	return (BlockAllocated(Font)::operator new (n));
#endif	/*qRealTemplatesAvailable*/
}

void	Font::operator delete (void* p)
{
#if		qRealTemplatesAvailable
	BlockAllocated<Font>::operator delete (p);
#else	/*qRealTemplatesAvailable*/
	BlockAllocated(Font)::operator delete (p);
#endif	/*qRealTemplatesAvailable*/
}

void	Font::AssureOneReference ()
{
	RequireNotNil (fFontInfo);
	
	if (fFontInfo.CountReferences () > 1) {
		fFontInfo = new FontInfo (GetName (), GetSize (), GetStyle ());
	}
	Ensure (fFontInfo.CountReferences () == 1);
}

Boolean	operator== (const Font& lhs, const Font& rhs)
{
	if (lhs.fFontInfo == rhs.fFontInfo) {
		return (True);
	}
	
#if		qMacGDI
	if ((lhs.GetSize () == rhs.GetSize ()) and (lhs.GetStyleOSRep () == rhs.GetStyleOSRep ()))  {
		// compiler bug won't allow temporaries with destructors in expression
		String	foo1 = lhs.GetName ();
		String	foo2 = rhs.GetName ();
		if (foo1 == foo2) {
			return (True);
		}
	}
#else	
	// compiler bug won't allow temporaries with destructors in expression
	if (lhs.GetSize () == rhs.GetSize ()) {
		if (lhs.GetName () == rhs.GetName ()) {
#if		!qBorlandCPlus
// Borland compiler workaround ??? LGP May 5, 1992
			if (lhs.GetStyle () == rhs.GetStyle ()) {
				return (True);
			}
#endif
		}
	}
#endif	/*GDI*/
	return (False);
}

Boolean	operator!= (const Font& lhs, const Font& rhs)
{
	return (not (lhs == rhs));
}





/*
 ********************************************************************************
 ********************************** FontIterator ********************************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable
	Implement (Iterator, Font);
#endif

Exception	FontIterator::sFontListUnavailable;

FontIterator::FontIterator ():
	Iterator(Font) (),
#if		qMacGDI
	fCurrentIndex (0),
	fMaxIndex (0),
	fCurrentFontName (kEmptyString)
#elif	qXGDI
	fNames (),
	fNamesIterator (Nil)
#endif	/*GDI*/
{
#if		qMacGDI
	/*
	 * No guess how were supposed to do this.  In both system 6 and system 7, we can
	 * get the names from FONDs.  We do that for now.  As for other info, there seem
	 * too many different interfaces, and I'm not sure what to do for this...
	 */
	fMaxIndex = ::CountResources ('FOND');
	ScanForNextFont ();
#elif	qXGDI
	extern	osDisplay*	gDisplay;
	int		max_fonts		=	10000;
	int		actual_count	=	0;
	char**	names			=	::XListFonts (gDisplay, "-*-*-*", max_fonts, &actual_count);
	if (names == Nil) {
		sFontListUnavailable.Raise ();
	}
	for (int i = 0; i < actual_count; i++) {
		AssertNotNil (names [i]);
		String bigName = String (names [i]);

		/*
		 * The names appear to be formed -Foundry-Family-...who cares
		 * We want to snag the Family name. Note, we can assume we'll find the dashes in the right place since
		 * we matched above in XListFonts().
		 */
		String familyName = bigName;
		CollectionSize ss = familyName.IndexOf ('-');
		Assert (ss != kBadSequenceIndex);
		familyName = familyName.SubString (ss+1);		// skip over foundry
		ss = familyName.IndexOf ('-');
		Assert (ss != kBadSequenceIndex);
		familyName = familyName.SubString (ss+1);		// snag family name followed by cruft
		ss = familyName.IndexOf ('-');
		familyName.SetLength (ss-1);					// trim trailing cruft

		fNames.Add (familyName);
	}
	fNamesIterator = fNames.MakeIterator (); 
	::XFreeFontNames (names);
#endif	/*qMacGDI*/
}

FontIterator::~FontIterator ()
{
#if		qXGDI
	delete (fNamesIterator);
#endif
}

Boolean	FontIterator::Done () const
{
#if		qMacGDI
	return Boolean (fCurrentIndex > fMaxIndex);
#elif	qXGDI
	RequireNotNil (fNamesIterator);
	return (fNamesIterator->Done ());
#endif
}

void	FontIterator::Next ()
{
#if		qMacGDI
	ScanForNextFont ();
#elif	qXGDI
	RequireNotNil (fNamesIterator);
	return (fNamesIterator->Next ());
#endif
}

Font	FontIterator::Current () const
{
#if		qMacGDI
	Require (fCurrentFontName != kEmptyString);
	return (fCurrentFontName);
#elif	qXGDI
	RequireNotNil (fNamesIterator);
	return (fNamesIterator->Current ());
#endif
}

#if		qMacGDI
void	FontIterator::ScanForNextFont ()
{
	while (True) {				// til we get a good font...
		fCurrentIndex++;
		if (fCurrentIndex > fMaxIndex) {
			fCurrentFontName = kEmptyString;
			break;
		}

		osHandle	resHandle	=	::GetIndResource ('FOND', fCurrentIndex);		// snag from any resource file
		if (resHandle != Nil) {
			osStr255	curName;
			short		theID;
			ResType 	theType;
			::SetResLoad (False);
			::GetResInfo (resHandle, &theID, &theType, curName);		// check errors...
			::SetResLoad (True);
			fCurrentFontName = String (&curName[1], curName[0]);
			break;
		}
	}
}
#endif	/*qMacGDI*/




/*
 ********************************************************************************
 ****************************** iostream support ********************************
 ********************************************************************************
 */
class ostream&	operator<< (class ostream& out, const Font& f)
{
// not quite right - should write / read font style too
	WriteString (out, f.GetName ());
	out << ' ' << f.GetSize () << ' ';
	return (out);
}

class istream&	operator>> (class istream& in, Font& f)
{
	FontName	fred = kEmptyString;
	FontSize	size = 0;
	
	ReadString (in, fred);
	in >> size;
	f.SetName (fred);
	f.SetSize (size);
	return (in);
}






/*
 ********************************************************************************
 ************************************ FontInfo **********************************
 ********************************************************************************
 */
#if		qRealTemplatesAvailable
FontInfo::FontInfo (const Font::Name& fontName, Font::Size fontSize, const AbSet<Font::Style>& fontStyleSet):
#else
FontInfo::FontInfo (const FontName& fontName, FontSize fontSize, const AbSet(FontStyle)& fontStyleSet):
#endif
	fName (fontName),
	fSize (fontSize),
	fStyleSet (fontStyleSet),
	fCacheValid (False)
#if		qMacGDI
	,fID (0)
#elif	qXGDI
	,fID (Nil)
	,fFontStruct (Nil)
#endif
{
}

FontInfo::~FontInfo ()
{
#if		qXGDI
	// not too sure safe to do XFreeFont () since could screw someone else up who created this font
	// independently???
	if (fFontStruct != Nil) {
		extern	osDisplay*	gDisplay;
		::XFreeFont (gDisplay, fFontStruct);
	}
#endif
}

FontName	FontInfo::GetName () const
{
	return (fName);
}

void	FontInfo::SetName (const FontName& fontName)
{
	Require (fName != fontName);

	fName = fontName;
	InvalidateCache ();
}

FontSize	FontInfo::GetSize () const
{
	return (fSize);
}

void	FontInfo::SetSize (FontSize fontSize)
{
	Require (fSize != fontSize);

	fSize = fontSize;
	InvalidateCache ();
}

#if		qRealTemplatesAvailable
const AbSet <Font::Style>&	FontInfo::GetStyle () const
{
	return (fStyleSet);
}

void	FontInfo::SetStyle (const AbSet<Font::Style>& fontStyleSet)
{
#if		!qBorlandCPlus
// borland doesnt like - investigate later - lgp may 5, 1992
	Require (fStyleSet != fontStyleSet);
#endif

	fStyleSet = fontStyleSet;
	InvalidateCache ();
}
#else
const AbSet(FontStyle)&	FontInfo::GetStyle () const
{
	return (fStyleSet);
}

void	FontInfo::SetStyle (const AbSet(FontStyle)& fontStyleSet)
{
	Require (fStyleSet != fontStyleSet);

	fStyleSet = fontStyleSet;
	InvalidateCache ();
}
#endif	/*qRealTemplatesAvailable*/

Coordinate	FontInfo::GetAscent () const
{
	SetValidCache ();
	return (fAscent);
}

Coordinate	FontInfo::GetDescent () const
{
	SetValidCache ();
	return (fDescent);
}

Coordinate	FontInfo::GetMinCharWidth () const
{
	AssertNotReached ();
	return (0);
}

Coordinate	FontInfo::GetMaxCharWidth () const
{
	SetValidCache ();
	return (fMaxCharWidth);
}

Boolean		FontInfo::Monospace () const
{
 	return (Boolean (GetMinCharWidth () == GetMaxCharWidth ())); 
}

Coordinate	FontInfo::Proportional () const
{
	 return Boolean (GetMinCharWidth () != GetMaxCharWidth ()); 
}

Coordinate	FontInfo::GetLeading () const
{
	SetValidCache ();
	return (fLeading);
}

#if		qMacGDI
short	FontInfo::GetID () const			
{ 
	SetValidCache (); 
	return (fID); 
}

short	FontInfo::GetStyleOSRep () const	
{ 
	SetValidCache (); 
	return (fStyleOSRep); 
}
#elif	qXGDI
unsigned long	FontInfo::GetID () const
{ 
	SetValidCache (); 
	return (fID); 
}

const XFontStruct*	FontInfo::GetXFontStruct () const
{ 
	SetValidCache (); 
	EnsureNotNil (fFontStruct);
	return (fFontStruct);
}
#endif	/*qMacGDI*/


/*
 * Even though this routine modifies this, we call it const, so that implementing caching is
 * not logically changing the object. We must be very careful about this, and think out when
 * and how we are doing this type of thing.
 */
void	FontInfo::SetValidCache () const
{
	if (not fCacheValid) {
		FontInfo*	cheatingThis	=	(FontInfo*)this;
#if		qMacGDI
		osStr255	pName;
		fName.ToPStringTrunc (pName, sizeof (pName));

		// Should think out the case where we dont find the font - and document it more carefully. My recolection
		// is that we get the application or system font - I forget which, but Bob was confused by this behavior and
		// it doesn't generalize well to other platforms/GUIs. Maybe raise an exception at this point - trouble
		// with that is that happens at funny time - when we call a get method???
		// Maybe do assert no error here, and test and excpetion in the SetName call - trouble with that is the font
		// could - in principle - at least - go away in the interim.
		::GetFNum (pName, &cheatingThis->fID);

		cheatingThis->fStyleOSRep = 0;
		if (fStyleSet.Contains (Font::eBoldFontStyle)) {
			cheatingThis->fStyleOSRep |= bold;
		}
		if (fStyleSet.Contains (Font::eItalicFontStyle)) {
			cheatingThis->fStyleOSRep |= italic;
		}
		if (fStyleSet.Contains (Font::eUnderlineFontStyle)) {
			cheatingThis->fStyleOSRep |= underline;
		}
		if (fStyleSet.Contains (Font::eOutlineFontStyle)) {
			cheatingThis->fStyleOSRep |= outline;
		}
		if (fStyleSet.Contains (Font::eShadowFontStyle)) {
			cheatingThis->fStyleOSRep |= shadow;
		}
		if (fStyleSet.Contains (Font::eCondenseFontStyle)) {
			cheatingThis->fStyleOSRep |= condense;
		}
		if (fStyleSet.Contains (Font::eExtendFontStyle)) {
			cheatingThis->fStyleOSRep |= extend;
		}

		osGrafPort*	aPort = Tablet::GetCurrentPort ();			// save/restore cuz called at funny times
																// when people might not expect the port to change...
																// Maybe we can get rid of this, but currntly
																// its required in a few places (Texedit mostly, I
																// think).
		Tablet::xDoSetPort (GetWorkPort ());
		::TextFont (fID);
		::TextSize (fSize);
		::TextFace (fStyleOSRep);
		osFontInfo	f;
		::GetFontInfo (&f);
		Tablet::xDoSetPort (aPort);

		cheatingThis->fAscent = f.ascent;
		cheatingThis->fDescent = f.descent;
		cheatingThis->fLeading = f.leading;
		cheatingThis->fMaxCharWidth = f.widMax;
#elif	qXGDI


		// clear out old reference...
		if (fFontStruct != Nil) {
			extern	osDisplay*	gDisplay;
			::XFreeFont (gDisplay, fFontStruct);
			cheatingThis->fFontStruct = Nil;
		}

		String fontName = fName;
// hack for now (hack of the day)
if (fontName == "Chicago") {
	fontName = "swiss 742";
//	fontName = "helvetica";
}
#if 0
else if (fontName == "Geneva") {
	fontName = "courier";
}
#endif
		if (0/* && fName == /*kSystemFont.GetName ()"Chicago"*/) {
			extern	osDisplay*	gDisplay;
			cheatingThis->fFontStruct	=	::XQueryFont (gDisplay, XGContextFromGC (DefaultGC (gDisplay, DefaultScreen (gDisplay))));
		}
		else {
			OStringStream	buf;
			buf << "-*-" << fontName;;
		
			if (fStyleSet.Contains (Font::eBoldFontStyle)) {
				buf << "-bold";
			}
			else {
				buf << "-medium";
			}
		
			if (fStyleSet.Contains (Font::eItalicFontStyle)) {
				buf << "-i";
			}
			else {
				buf << "-r";
			}
		
			if (fStyleSet.Contains (Font::eCondenseFontStyle)) {
				buf << "-condensed";
			}
			else if (fStyleSet.Contains (Font::eExtendFontStyle)) {
				buf << "-double";
			}
			else {
				buf << "-normal";
			}
		
			buf << "-*-";
		
			int fontSize = GetSize () * 10;
			buf.operator<< (fontSize);
			buf << "-*";
			char*			cbuf	=	String (buf).ToCString ();
			extern	osDisplay*	gDisplay;
			cheatingThis->fFontStruct	=	::XLoadQueryFont (gDisplay, cbuf);
		
			if (fFontStruct == Nil) {
				// look for some other size of the same font (probably not best choice, should pick different font of same size???
			
				OStringStream	buf;
				buf << "-*-" << fontName << "-*";
				char*			cbuf	=	String (buf).ToCString ();
				cheatingThis->fFontStruct	=	::XLoadQueryFont (gDisplay, cbuf);
			}
		
			if (fFontStruct == Nil) {
#if		1
			// use some default if one requested unavailble..
				delete (cbuf);
				cbuf = String ("fixed").ToCString ();		// clone default name, and try again.. I think this garuenteed to be there!!
				cheatingThis->fFontStruct	=	::XLoadQueryFont (gDisplay, cbuf);
				AssertNotNil (fFontStruct);
#else
				delete (cbuf);
				AssertNotReached ();		// manually destruct other guys til we have real exceptions...
				Font::sFontUnavailable.Raise ();
#endif
			}
			delete (cbuf);
		}
		AssertNotNil (fFontStruct);
		cheatingThis->fID = fFontStruct->fid;
		cheatingThis->fAscent = fFontStruct->ascent;
		cheatingThis->fDescent = fFontStruct->descent;
		cheatingThis->fLeading = 5;						// hack for now...
		cheatingThis->fMaxCharWidth = fFontStruct->max_bounds.width;
#endif	/*GDI*/

		cheatingThis->fCacheValid = True;
	}
	Ensure (fCacheValid);
}

void	FontInfo::InvalidateCache ()
{
	fCacheValid = False;
}









// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


