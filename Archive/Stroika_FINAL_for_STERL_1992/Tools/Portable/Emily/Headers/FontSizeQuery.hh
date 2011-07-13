/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/FontSizeQuery.hh,v 1.5 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * To Do:
 *
 * $Log: FontSizeQuery.hh,v $
 *		Revision 1.5  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *		
 *
 */


// text before here will be retained: Do not remove or modify this line!!!

#include "FocusItem.hh"
#include "TextEdit.hh"
#include "View.hh"

#include "TextEdit.hh"
#include "NumberText.hh"
#include "TextView.hh"


class FontSizeQueryX : public View, public FocusOwner, public TextController {
	public:
		FontSizeQueryX ();
		~FontSizeQueryX ();

		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

	protected:
		override	void	Layout ();

		TextEdit			fTitle;
		NumberText			fFontSize;
		TextView			fFontSizeLabel;

	private:
#if   qMacUI
		nonvirtual void	BuildForMacUI ();
#elif qMotifUI
		nonvirtual void	BuildForMotifUI ();
#else
		nonvirtual void	BuildForUnknownGUI ();
#endif /* GUI */

};



// text past here will be retained: Do not remove or modify this line!!!
class	FontSizeQuery : public FontSizeQueryX {
	public:
		FontSizeQuery ();
		nonvirtual	UInt16	GetFontSize () const;
};

