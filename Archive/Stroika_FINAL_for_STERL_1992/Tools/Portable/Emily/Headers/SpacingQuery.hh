/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/SpacingQuery.hh,v 1.5 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * To Do:
 *
 * $Log: SpacingQuery.hh,v $
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

#include "TextView.hh"
#include "NumberText.hh"
#include "TextEdit.hh"


class SpacingQueryX : public View, public FocusOwner, public TextController {
	public:
		SpacingQueryX ();
		~SpacingQueryX ();

		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

	protected:
		override	void	Layout ();

		TextView			fSpacingLabel;
		NumberText			fSpacing;
		TextEdit			fTitle;

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
class	SpacingQuery : public SpacingQueryX {
	public:
		SpacingQuery ();
		nonvirtual	UInt16	GetSpacing () const;
};

