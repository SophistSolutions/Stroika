/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/EmilyPreferences.hh,v 1.6 1992/09/08 16:40:43 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: EmilyPreferences.hh,v $
 *		Revision 1.6  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
*
 *
 *
 */

#ifndef	__EmilyPreferences__
#define __EmilyPreferences__


#include "CSymbolText.hh"
class FileSuffixText : public CSymbolText {
	public:
		FileSuffixText (TextController* c = Nil);
		
	protected:
		override	void	Validate ();
		override	Boolean	ValidateKey (const KeyStroke& keyStroke);
};

// text before here will be retained: Do not remove or modify this line!!!

#include "Button.hh"
#include "FocusItem.hh"
#include "TextEdit.hh"
#include "View.hh"

#include "CheckBox.hh"
#include "GroupView.hh"
#include "ScrollableText.hh"
#include "NumberText.hh"
#include "TextView.hh"
#include "CSymbolText.hh"
#include "PickList.hh"
#include "PushButton.hh"


class PreferencesInfoX : public View, public ButtonController, public FocusOwner, public TextController {
	public:
		PreferencesInfoX ();
		~PreferencesInfoX ();

		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

	protected:
		override	void	Layout ();

		CheckBox			fCompileOnce;
		LabeledGroup		fFileHeaderGroup;
		FocusOwner			fFileHeaderGroupTabLoop;
		TextEdit			fDefaultPrepend;
		TextScroller		fDefaultPrependScroller;
		LabeledGroup		fGridSizeGroup;
		FocusOwner			fGridSizeGroupTabLoop;
		CheckBox			fShowGrid;
		CheckBox			fConstrainTracking;
		NumberText			fVertical;
		NumberText			fHorizontal;
		TextView			fHGridLabel;
		TextView			fVGridLabel;
		TextView			fDataSuffixLabel;
		TextView			fHeaderSuffixLabel;
		TextView			fSourceSuffixLabel;
		FileSuffixText		fSourceSuffix;
		FileSuffixText		fHeaderSuffix;
		FileSuffixText		fDataSuffix;
		LabeledGroup		fSpacingGroup;
		FocusOwner			fSpacingGroupTabLoop;
		NumberText			fSpace;
		TextView			fSpacingLabel;
		StringPickList		fSpacings;
		PushButton			fInsert;
		PushButton			fDelete;

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
class PreferencesInfo : public PreferencesInfoX {
	public:
		PreferencesInfo ();
		
		nonvirtual	NumberText&		PreferencesInfo::GetVerticalField ()
		{
			return (fVertical);
		}
		
		nonvirtual	NumberText&		PreferencesInfo::GetHorizontalField ()
		{
			return (fHorizontal);
		}
		
		nonvirtual	CheckBox&	PreferencesInfo::GetCompileOnceField ()
		{
			return (fCompileOnce);
		}
		
		nonvirtual	CheckBox&	PreferencesInfo::GetGridOnField ()
		{
			return (fShowGrid);
		}
		
		nonvirtual	CheckBox&	PreferencesInfo::GetConstrainTrackingField ()
		{
			return (fConstrainTracking);
		}
		
		nonvirtual	CSymbolText&	PreferencesInfo::GetDataSuffixField ()
		{
			return (fDataSuffix);
		}
		
		nonvirtual	CSymbolText&	PreferencesInfo::GetHeaderSuffixField ()
		{
			return (fHeaderSuffix);
		}
		
		nonvirtual	CSymbolText&	PreferencesInfo::GetSourceSuffixField ()
		{
			return (fSourceSuffix);
		}
		
		nonvirtual	TextEdit&	PreferencesInfo::GetDefaultPrependField ()
		{
			return (fDefaultPrepend);
		}
		
		nonvirtual	StringPickList&	PreferencesInfo::GetSpacingList ()
		{
			return (fSpacings);
		}

	protected:
		override	void	ButtonPressed (AbstractButton* button);
		override	void	TextChanged (TextEditBase* item);

	private:
		nonvirtual	void	UpdateContext ();
};

#endif /* __EmilyPreferences__ */

