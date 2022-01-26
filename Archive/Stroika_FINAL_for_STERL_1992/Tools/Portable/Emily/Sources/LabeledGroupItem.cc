/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/LabeledGroupItem.cc,v 1.7 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: LabeledGroupItem.cc,v $
 *		Revision 1.7  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.6  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/07/22  00:34:55  lewis
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *
 *
 */





#include	"StreamUtils.hh"

#include	"Pallet.hh"
#include	"Shape.hh"

#include	"Dialog.hh"
#include	"Scroller.hh"

#include	"CommandNumbers.hh"
#include	"LabeledGroupItem.hh"
#include	"LabeledGroupInfo.hh"
#include	"ViewItemInfo.hh"
#include	"EmilyWindow.hh"


#if		!qRealTemplatesAvailable
	Implement (Iterator, LGParmPtr);
	Implement (Collection, LGParmPtr);
	Implement (AbBag, LGParmPtr);
	Implement (Array, LGParmPtr);
	Implement (Bag_Array, LGParmPtr);
	Implement (Bag, LGParmPtr);
#endif


class	SetLabeledGroupInfoCommand : public Command {
	public:
		SetLabeledGroupInfoCommand (LabeledGroupItem& item, LabeledGroupInfo& info);
		
		override	void	DoIt ();		
		override	void	UnDoIt ();
	
	private:
		LabeledGroupItem&	fItem;
		Command*			fItemInfoCommand;
		String				fOldLabel;
		String				fNewLabel;
};


LabeledGroupParam::LabeledGroupParam (CommandNumber language, CommandNumber gui) :
	Saveable (1),
	fLanguage (language),
	fGUI (gui),
	fLabel (kEmptyString)
{
}

void	LabeledGroupParam::DoRead_ (class istream& from)
{
	Saveable::DoRead_ (from);
	from >> fLanguage >> fGUI;
	fLanguage += eFirstLanguage;
	fGUI += eFirstGUI;
	ReadString (from, fLabel);
}

void	LabeledGroupParam::DoWrite_ (class ostream& to, int tabCount) const
{
	Saveable::DoWrite_ (to, tabCount);
	to << newline;
	to << tab (tabCount) << (fLanguage - eFirstLanguage) << ' ' << (fGUI - eFirstGUI) << newline;
	to << tab (tabCount);
	WriteString (to, fLabel);
	to << newline;
}


/*
 ********************************************************************************
 ******************************** LabeledMagicGroup ******************************
 ********************************************************************************
 */

LabeledMagicGroup::LabeledMagicGroup (const String& label):
	fLabel (label)
{
	SetBorder (1, 1, eNoUpdate);
	SetMargin (1, 1, eNoUpdate);
}
		
String	LabeledMagicGroup::GetLabel () const
{
	return (fLabel);
}

void	LabeledMagicGroup::SetLabel (const String& label, UpdateMode updateMode)
{
	if (label != GetLabel ()) {
		SetLabel_ (label, updateMode);
	}
	Ensure (label == GetLabel ());
}

void	LabeledMagicGroup::DrawBorder_ (const Rect& box, const Point& border)
{
	CommandNumber gui = EmilyWindow::GetGUI ();
	if (gui == eMacUI) {
		DrawMacBorder (box, border);
	}
	else if (gui == eMotifUI) {
		Color c = PalletManager::Get ().MakeColorFromTile (GetEffectiveBackground ());
		DrawMotifBorder (box, border, c, GetPlane ());
	}
	else if (gui == eWindowsGUI) {
		DrawMacBorder (box, border);
	}
}

void	LabeledMagicGroup::Draw (const Region& update)
{
	CommandNumber gui = EmilyWindow::GetGUI ();
	if (GetLabel () == kEmptyString) {
		DrawBorder ();
	}
	else {
		Font f = GetEffectiveFont ();
		Coordinate	textHeight = f.GetFontHeight ();
		Coordinate	textWidth  = TextWidth (GetLabel (), f);
		Rect	titleRect = Rect (Point (0, 5), Point (textHeight, textWidth));
		if ((gui == eMotifUI) or (gui == eWindowsGUI)) {
			titleRect.SetOrigin (Point (GetBorder ().GetV () + 2, 5));
		}

		Rect r = GetLocalExtent ();
		if (gui == eMacUI) {
			r = Rect (Point (textHeight/2, 0), GetSize () - Point (textHeight/2, 0));
		}
		DrawBorder_ (r, GetBorder ());

		titleRect = titleRect.InsetBy (Point (0, -2));
		Erase (Rectangle (), titleRect);
		titleRect = titleRect.InsetBy (Point (0, 2));
		DrawText (GetLabel (), f, titleRect.GetOrigin () + Point (-1, 0));
	}
}

void	LabeledMagicGroup::SetLabel_ (const String& label, UpdateMode updateMode)
{
	fLabel = label;
	Refresh (updateMode);
}

/*
 ********************************************************************************
 ****************************** LabeledGroupItemType ********************************
 ********************************************************************************
 */
LabeledGroupItemType::LabeledGroupItemType () :
	ItemType (eBuildGroup, "LabeledGroup", (ItemBuilderProc)&LabeledGroupItemBuilder)
{
	Require (sThis == Nil);
	sThis = this;
}		
		
LabeledGroupItemType&	LabeledGroupItemType::Get ()
{
	RequireNotNil (sThis);
	return (*sThis);
}

ViewItem*	LabeledGroupItemType::LabeledGroupItemBuilder ()
{
	return (new LabeledGroupItem ());
}

LabeledGroupItemType*	LabeledGroupItemType::sThis = Nil;




/*
 ********************************************************************************
 ****************************** LabeledGroupItem ************************************
 ********************************************************************************
 */
LabeledGroupItem::LabeledGroupItem () :
	GroupItem (LabeledGroupItemType::Get (), True),
	fLabels (),
	fLabeledGroup (Nil)
{
	fLabeledGroup = new LabeledMagicGroup (kEmptyString);
	SetGroupView (False, fLabeledGroup);
	SetMaxVersion (3);
}

LabeledGroupItem::~LabeledGroupItem ()
{
	ForEach (LabeledGroupParamPtr, it, fLabels) {
		LabeledGroupParam* param = it.Current ();
		delete param;
	}
}

String	LabeledGroupItem::GetHeaderFileName ()
{
	static	const	String kHeaderFileName = "GroupView.hh";
	return (kHeaderFileName);
}

void	LabeledGroupItem::WriteBuilder (class ostream& to, int tabCount)
{
	ViewItem::WriteBuilder (to, tabCount);	// deliberately skipped groupitem
	to << "(kEmptyString)," << newline;
}

void	LabeledGroupItem::DoRead_ (class istream& from)
{
	GroupItem::DoRead_ (from);
	
	if (GetVersion () <= 2) {
		String	foo;
		ReadString (from, foo);
		SetLabel (foo);
	}

	if (GetVersion () >= 3) {
		ForEach (LabeledGroupParamPtr, it, fLabels) {
			LabeledGroupParam* param = it.Current ();
			delete param;
		}
		fLabels.RemoveAll ();

		CollectionSize paramCount;
		from >> paramCount;
		for (int count = 1; count <= paramCount; count++) {
			LabeledGroupParam*	newParams = new LabeledGroupParam (eEnglish, eMacUI);
			char c;
			from >> c;
			Require (c == '{');
			newParams->DoRead (from);
			from >> c;
			Require (c == '}');
			fLabels.Add (newParams);	
		}
	}
}

void	LabeledGroupItem::DoWrite_ (class ostream& to, int tabCount) const
{
	GroupItem::DoWrite_ (to, tabCount);
	CollectionSize	paramCount = fLabels.GetLength ();
	to << tab (tabCount) << paramCount << newline;
	ForEach (LabeledGroupParamPtr, it, fLabels) {
		to << tab (tabCount) << '{' << newline;
		it.Current ()->DoWrite (to, tabCount+1);
		to << tab (tabCount) << '}' << newline;
	}
}

void	LabeledGroupItem::WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	GroupItem::WriteParameters (to, tabCount, language, gui);
	LabeledGroupParam*	currentParams = FindLabel (GetBaseLanguage (), gui);
	AssertNotNil (currentParams);
	to << tab (tabCount) << GetFieldName () << ".SetLabel (" << quote << currentParams->fLabel << quote << ", eNoUpdate);" << newline;
}

void	LabeledGroupItem::WriteCustomization (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	GroupItem::WriteCustomization (to, tabCount, language, gui);
	if ((language != GetBaseLanguage ()) or (gui != GetBaseGUI ())) {
		LabeledGroupParam*	baseParams 	  = FindLabel (GetBaseLanguage (), GetBaseGUI ());
		LabeledGroupParam*	currentParams = FindLabel (language, gui);
		
		AssertNotNil (baseParams);
		if (currentParams != Nil) {
			if (currentParams->fLabel != baseParams->fLabel) {
				to << tab (tabCount) << GetFieldName () << kFieldAccessor;
				to << "SetLabel (" << quote << currentParams->fLabel << quote << ");" << newline;
			}
		}
	}
}

String	LabeledGroupItem::GetLabel () const
{
	return (GetCurrentLabel ().fLabel);
}

void	LabeledGroupItem::SetLabel (const String& label)
{
	GetCurrentLabel ().fLabel = label;
	CommandNumber	gui = EmilyWindow::GetGUI ();
	if (EmilyWindow::GetFullEditing ()) {
		ForEach (LabeledGroupParamPtr, it, fLabels) {
			LabeledGroupParam*	current = it.Current ();
			current->fLabel = label;
		}
	}
}

LabeledGroupParam*	LabeledGroupItem::FindLabel (CommandNumber language, CommandNumber gui) const
{
	ForEach (LabeledGroupParamPtr, it, fLabels) {
		LabeledGroupParam*	current = it.Current ();
		
		if (((current->fLanguage == language) or (language == CommandHandler::eNoCommand)) and 
			((current->fGUI == gui) or (gui == CommandHandler::eNoCommand))) {
			
			return (current);
		}
	}
	return (Nil);
}

LabeledGroupParam&	LabeledGroupItem::GetCurrentLabel () const
{
	LabeledGroupParam*	param = FindLabel (EmilyWindow::GetLanguage (), EmilyWindow::GetGUI ());
	
	if (param == Nil) {
		LabeledGroupItem*	This = (LabeledGroupItem*)this;
		This->AddParam (EmilyWindow::GetLanguage (), EmilyWindow::GetGUI (), GetBaseLanguage (), GetBaseGUI ());
		param = FindLabel (EmilyWindow::GetLanguage (), EmilyWindow::GetGUI ());
	}
	EnsureNotNil (param);
	return (*param);
}

void	LabeledGroupItem::AddParam (CommandNumber language, CommandNumber gui, CommandNumber oldLanguage, CommandNumber oldGUI)
{
	GroupItem::AddParam (language, gui, oldLanguage, oldGUI);
	LabeledGroupParam*	param = new LabeledGroupParam (language, gui);
	LabeledGroupParam* oldParams = FindLabel (oldLanguage, oldGUI);
	if (oldParams != Nil) {
		param->fLabel = oldParams->fLabel;
	}
	
	fLabels.Add (param);
}

void	LabeledGroupItem::RemoveParam (CommandNumber language, CommandNumber gui)
{
	GroupItem::RemoveParam (language, gui);
	LabeledGroupParam* param = FindLabel (language, gui);
	if (param != Nil) {
		fLabels.Remove (param);
	}
}

void	LabeledGroupItem::ApplyParams (CommandNumber language, CommandNumber gui)
{
	GroupItem::ApplyParams (language, gui);
	LabeledGroupParam* param = FindLabel (language, gui);
	if (param != Nil) {
		fLabeledGroup->SetLabel (param->fLabel);
	}
}

void	LabeledGroupItem::SetItemInfo ()
{
	LabeledGroupInfo info = LabeledGroupInfo (*this);
	Dialog d = Dialog (&info, &info, AbstractPushButton::kOKLabel, AbstractPushButton::kCancelLabel);
	d.SetDefaultButton (d.GetOKButton ());
	
	if (d.Pose ()) {
		PostCommand (new SetLabeledGroupInfoCommand (*this, info));
		DirtyDocument ();
	}
}


SetLabeledGroupInfoCommand::SetLabeledGroupInfoCommand (LabeledGroupItem& item, LabeledGroupInfo& info) :
	Command (eSetItemInfo, kUndoable),
	fItem (item),
	fItemInfoCommand (Nil),
	fNewLabel (info.GetLabelField ().GetText ()),
	fOldLabel (item.GetLabel ())
{
	fItemInfoCommand = new SetItemInfoCommand (item, info.GetViewItemInfo ());
}			

void	SetLabeledGroupInfoCommand::DoIt ()
{
	if (fNewLabel != fOldLabel) {
		fItem.SetLabel (fNewLabel);
	}
	fItemInfoCommand->DoIt ();
	
	Command::DoIt ();
}

void	SetLabeledGroupInfoCommand::UnDoIt ()
{
	if (fNewLabel != fOldLabel) {
		fItem.SetLabel (fOldLabel);
	}
	fItemInfoCommand->UnDoIt ();
	
	Command::UnDoIt ();
}

