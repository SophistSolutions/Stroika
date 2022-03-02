/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Font__
#define	__Font__

/*
 * $Header: /fuji/lewis/RCS/Font.hh,v 1.5 1992/09/11 18:36:49 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *		-	Consider changing FontName to FontFamily... Not sure I understand terms well enuf, but
 *			but I think fontfamily is more correct.
 *
 * Changes:
 *	$Log: Font.hh,v $
 *		Revision 1.5  1992/09/11  18:36:49  sterling
 *		use new Shared implementation
 *
 *		Revision 1.4  1992/09/01  15:34:49  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.3  1992/07/01  03:57:05  lewis
 *		Renamed Strings.hh String.hh.
 *
 *		Revision 1.2  1992/06/25  04:02:54  sterling
 *		Added GetFontHeight, and commented on difference between this and GetLineHeight.
 *
 *		Revision 1.1  1992/06/19  22:33:01  lewis
 *		Initial revision
 *
 *		Revision 1.13  1992/06/09  12:37:40  lewis
 *		Renamed SmartPointer back to Shared.
 *
 *		Revision 1.12  92/06/09  02:25:02  02:25:02  lewis (Lewis Pringle)
 *		Merged in sterls changes for new SmartPointer based ReferenceCounting.
 *		
 *		Revision 1.11  1992/05/10  00:04:27  lewis
 *		Get to compile under BC++/PC/qWinGDI.
 *
 *		Revision 1.10  92/04/07  10:56:21  10:56:21  lewis (Lewis Pringle)
 *		Support templates for PC port.
 *		
 *		Revision 1.8  1992/02/21  18:02:45  lewis
 *		Got rid of qGPlus_ClassOpNewDelBroke workaround.
 *
 *		Revision 1.7  1992/01/18  09:02:10  lewis
 *		Add method to get XFontStruct under X.
 *
 *		Revision 1.6  1992/01/14  05:56:52  lewis
 *		Scoped some types relating to fonts, and implement fonts for X. Used
 *		new Set(String) for the iterator.
 *
 *
 *
 */

#include	"Iterator.hh"
#include	"Shared.hh"
#include	"Set_BitString.hh"
#include	"Set.hh"
#include	"String.hh"

#include	"Point.hh"



class	FontInfo;
#if		!qRealTemplatesAvailable
	Declare (Shared, FontInfo);
	class	AbSet(FontStyle);
#endif


class	Font {
	public:
		typedef	String	Name;
		typedef	UInt16	FontSize;		//qCFront_NestedTypesHalfAssed
		enum Style {
			eBoldFontStyle,
			eItalicFontStyle,
			eUnderlineFontStyle,
			eOutlineFontStyle,
			eShadowFontStyle,
			eCondenseFontStyle,
			eExtendFontStyle,
		};

		Font (const Font& font);
		Font (const Name& fontName);
		Font (const Name& fontName, FontSize fontSize);
#if		qRealTemplatesAvailable
		Font (const Name& fontName, FontSize fontSize, const AbSet<Style>& fontStyleSet);
#else
		Font (const Name& fontName, FontSize fontSize, const AbSet(FontStyle)& fontStyleSet);
#endif
#if 	qMacGDI
		Font (short fontID, FontSize fontSize, short osStyleRep);
#endif	/*qMacGDI*/
		nonvirtual	~Font ();
		
		nonvirtual	Font&	operator= (const Font& newFont);

		nonvirtual	Name	GetName () const;
		nonvirtual	void	SetName (const Name& fontName);
		nonvirtual	FontSize	GetSize () const;
		nonvirtual	void	SetSize (FontSize fontSize);
#if		qRealTemplatesAvailable
		nonvirtual	const AbSet<Style>&	GetStyle () const;
		nonvirtual	void				SetStyle (const AbSet<Style>& fontStyleSet);
#else
		nonvirtual	const AbSet(FontStyle)&	GetStyle () const;
		nonvirtual	void					SetStyle (const AbSet(FontStyle)& fontStyleSet);
#endif

		nonvirtual	Coordinate	GetAscent () const;
		nonvirtual	Coordinate	GetDescent () const;
		nonvirtual	Coordinate	GetFontHeight () const;			//	Ascent + Descent
		nonvirtual	Coordinate	GetMinCharWidth () const;
		nonvirtual	Coordinate	GetMaxCharWidth () const;
		nonvirtual	Boolean		Monospace () const;
		nonvirtual	Coordinate	Proportional () const;
		nonvirtual	Coordinate	GetLeading () const;
		nonvirtual	Coordinate	GetLineHeight () const;			//	FontHeight + Leading

#if		qMacGDI
		nonvirtual	short	GetID () const;						// OS representation
		nonvirtual	short	GetStyleOSRep () const;				// OS representation
#elif	qXGDI
		nonvirtual	unsigned long	GetID () const;				// OS representation
		nonvirtual	const void*		GetXFontStruct () const;
#endif	/*qMacGDI*/

		static	void*	operator new (size_t n);
		static	void	operator delete (void* p);

		static	Exception		sFontUnavailable;

	private:
		nonvirtual	void	AssureOneReference ();

#if		qRealTemplatesAvailable
		Shared<FontInfo>	fFontInfo;
#else
		Shared(FontInfo)	fFontInfo;
#endif

		friend	Boolean	operator== (const Font& lhs, const Font& rhs);
		friend	Boolean	operator!= (const Font& lhs, const Font& rhs);
};



//these names will go away eventaully - just around for backward compatabilty, and cuz lack of templates makes
// getting rid of name FontStyle difficult.
#if		qCFront21
typedef	String		FontName;
//typedef	UInt16		FontSize;
typedef	Font::Style	FontStyle;
#else
typedef	Font::Name	FontName;
typedef	Font::Size	FontSize;
typedef	Font::Style	FontStyle;
#endif


#if		qRealTemplatesAvailable
extern	const	Set_BitString<Font::Style>	kPlainFontStyle;
#else
Declare (Iterator, FontStyle);
Declare (Collection, FontStyle);
Declare (AbSet, FontStyle);
Declare (Array, FontStyle);
Declare (Set_Array, FontStyle);
Declare (Set_BitString, FontStyle);
Declare (Set, FontStyle);
extern	const	Set_BitString(FontStyle)	kPlainFontStyle;
#endif


#if		!qRealTemplatesAvailable
Declare (Iterator, Font);
#endif

class	FontIterator : public
#if		qRealTemplatesAvailable
	Iterator<Font>
#else
	Iterator(Font)
#endif
{
	public:
		FontIterator ();
		virtual ~FontIterator ();

		override	Boolean	Done () const;
		override	void	Next ();
		override	Font	Current () const;

		static	Exception		sFontListUnavailable;

	private:
#if		qMacGDI
		UInt16	fCurrentIndex;
		UInt16	fMaxIndex;
		String	fCurrentFontName;
#elif	qXGDI
		Set(String)	fNames;
		Iterator(String)*	fNamesIterator;
#endif

#if		qMacGDI
		nonvirtual	void	ScanForNextFont ();
#endif
};



/*
 * iostream support.
 */
class ostream;
class istream;
extern	ostream&	operator<< (ostream& out, const Font& f);
extern	istream&	operator>> (istream& in, Font& f);


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Font__*/



