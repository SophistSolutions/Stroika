/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/OptionMenu.cc,v 1.7 1992/09/08 15:34:00 lewis Exp $
 *
 * TODO:
 *		-- Add ability to set Enabled flags per item...
 *		-- Add fAutoResize even for motif - just simply very handy...
 *
 * Changes:
 *	$Log: OptionMenu.cc,v $
 *		Revision 1.7  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.6  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/07/21  20:12:43  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.4  1992/07/16  16:44:08  sterling
 *		changed GUI to UI
 *
 *		Revision 1.3  1992/07/03  01:03:45  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.2  1992/06/25  08:20:21  sterling
 *		Lots of changes including DrawText calls. Renamed CalcDefaultSize to CalcDefaultSize_.
 *
 *		Revision 1.8  1992/05/18  16:35:40  lewis
 *		Fix compiler warniogns on mac. Make calcdefaultsize for motif a little larger.
 *		Place mac optionmenu properly - so pops up over old menu - not over mouse.
 *
 *		Revision 1.7  92/05/11  21:33:53  21:33:53  lewis (Lewis Pringle)
 *		Add Require ().
 *		
 *		Revision 1.6  92/05/01  01:37:42  01:37:42  lewis (Lewis Pringle)
 *		Comment out Application::Get ().ResumeCommandProcessing (); - fix again later.. temp hack
 *		for doing relase.
 *		
 *		Revision 1.4  92/04/30  13:24:26  13:24:26  sterling (Sterling Wight)
 *		made work on Mac
 *		
 *		Revision 1.3  92/04/30  03:46:09  03:46:09  lewis (Lewis Pringle)
 *		Comment out suspend/resume of idletasks - prevents update event processing currently.
 *		This whole area is a mess. We must debug but low priority now.
 *		
 *
 *		<< Based on PopUpMenu.cc >>
 *
 */



#include	"OSRenamePre.hh"
#if		qMacToolkit
#elif	qXmToolkit
#if		qSnake && _POSIX_SOURCE
typedef	char*	caddr_t;		// work around hp POSIX headers bug
#endif
#include	<X11/Intrinsic.h>
#include	"OSRenamePost.hh"		// they undef a bunch of things we define
#include	"OSRenamePre.hh"		// so blast back!!!
#include	<X11/IntrinsicP.h>
#include	"OSRenamePost.hh"		// they undef a bunch of things we define
#include	"OSRenamePre.hh"		// so blast back!!!
#if		qXmToolkit
#include	<Xm/XmP.h>
#undef	Max
#undef	Min
#undef	XtWindow
#undef	XtDisplay
#include	<Xm/RowColumn.h>
#include	<Xm/CascadeBG.h>
#include	<Xm/Label.h>
#include	<Xm/PushBG.h>
#endif
#endif	/*Toolkit*/
#include	"OSRenamePost.hh"

#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"Shape.hh"

#include	"Application.hh"
#include	"DeskTop.hh"
#include	"OSControls.hh"
#include	"Menu.hh"
#include	"PopUpMenu.hh"

#include	"OptionMenu.hh"	














/*
 ********************************************************************************
 ****************************** AbstractOptionMenuButton ************************
 ********************************************************************************
 */

AbstractOptionMenuButton::AbstractOptionMenuButton ():
	Button (Nil)
{
}

String	AbstractOptionMenuButton::GetLabel () const
{
	return (GetLabel_ ());
}

void	AbstractOptionMenuButton::SetLabel (const String& label, Panel::UpdateMode updateMode)
{
	if (label != GetLabel ()) {
		SetLabel_ (label, updateMode);
	}
}

CollectionSize	AbstractOptionMenuButton::GetItemCount () const
{
	return (GetItems_ ().GetLength ());
}

void	AbstractOptionMenuButton::SetItemCount (CollectionSize itemCount)
{
	if (itemCount != GetItemCount ()) {
		SetItemCount_ (itemCount);
	}
	Ensure (itemCount == GetItemCount ());
	Ensure (GetCurrentItem () == kBadSequenceIndex or GetCurrentItem () <= itemCount);	// didn't blow away current item!
}

const AbSequence(String)&	AbstractOptionMenuButton::GetItems () const
{
	return (GetItems_ ());
}

String	AbstractOptionMenuButton::GetItem (CollectionSize i) const
{
	Require (i >= 1 and i <= GetItemCount ());
	return (GetItems_ ()[i]);
}

void	AbstractOptionMenuButton::SetItem (CollectionSize i, const String& item, Panel::UpdateMode updateMode)
{
	if (GetItem (i) != item) {
		SetItem_ (i, item, updateMode);
	}
}

void	AbstractOptionMenuButton::AppendItem (const String& item)
{
	CollectionSize	newItemCount	=	GetItemCount () + 1;
	SetItemCount (newItemCount);
	SetItem (newItemCount, item, eNoUpdate);
}

CollectionSize	AbstractOptionMenuButton::GetCurrentItem () const
{
	return (GetCurrentItem_ ());
}

void	AbstractOptionMenuButton::SetCurrentItem (CollectionSize i, Panel::UpdateMode updateMode)
{
	if (GetCurrentItem () != i) {
		SetCurrentItem_ (i, updateMode);
	}
}








/*
 ********************************************************************************
 ****************************** OptionMenuButton_MacUI *************************
 ********************************************************************************
 */
OptionMenuButton_MacUI::OptionMenuButton_MacUI ():
	AbstractOptionMenuButton ()
{
	SetFont (&kSystemFont);
	SetBackground (&kWhiteTile);
}

Point	OptionMenuButton_MacUI::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
	const Font&	f = GetEffectiveFont ();
	Coordinate	newWidth = TextWidth (GetLabel ());
	if (GetCurrentItem () == kBadSequenceIndex) {
		newWidth += 50;			// some reasonable default
	}
	else {
		newWidth += TextWidth (GetItem (GetCurrentItem ())) + GetItemOffset () +
			2 +		
			5;		// extra whitespace so name not pressed up against drop shadow...
	}
	Coordinate	newHeight	=	f.GetFontHeight () + 3;
	return (Point (newHeight, newWidth));
}

Coordinate	OptionMenuButton_MacUI::GetItemOffset () const
{
	if (GetLabel () == kEmptyString) {
		return (0);
	}
	else {
		return (2);	// leave some room after the text
	}
}

Boolean	OptionMenuButton_MacUI::GetAutoResize () const
{
	return (GetAutoResize_ ());
}

void	OptionMenuButton_MacUI::SetAutoResize (Boolean autoResize)
{
	SetAutoResize_ (autoResize);
}









/*
 ********************************************************************************
 ******************************* OptionMenuButton_MacUI_Portable ***************
 ********************************************************************************
 */

OptionMenuButton_MacUI_Portable::OptionMenuButton_MacUI_Portable (const String& label):
	OptionMenuButton_MacUI (),
	fLabel (kEmptyString),
	fItems (),
	fCurrentItem (kBadSequenceIndex),
	fAutoResize (True),
	fPopUp (Nil)
{
	SetLabel (label, eNoUpdate);
}

OptionMenuButton_MacUI_Portable::~OptionMenuButton_MacUI_Portable ()
{
}

Boolean	OptionMenuButton_MacUI_Portable::TrackPress (const MousePressInfo& mouseInfo)
{
	Require (GetLive ());
	
	PopUpMenuTitle	popUp (kEmptyString, Nil);

	CollectionSize oldCurrent = GetCurrentItem ();

#if		qCFront_ConversionOpOnFunctionResultBroken
	ForEach (String, it, GetItems ().operator Iterator(String)* ()) {
#else
	ForEach (String, it, GetItems ()) {
#endif
		popUp.GetMenu ()->AddStringMenuItem (CommandHandler::ePopupItem, it.Current ());
	}
	if (oldCurrent != kBadSequenceIndex) {
		popUp.SetCurrentItem (popUp.GetMenu ()->GetMenuItemByIndex (oldCurrent));
	}
	
	Rect	labelRect	=	GetLocalExtent ();
	Rect	menuRect	=	CalcMenuRect ();
	
	labelRect.SetSize (Point (labelRect.GetHeight (), TextWidth (GetLabel ())));
	if (GetCurrentItem () != kBadSequenceIndex) {
		labelRect.SetSize (Point (labelRect.GetHeight (), labelRect.GetWidth () + GetItemOffset ()));
	}
	Invert (Rectangle (), labelRect);
	
//	popUp.SetOrigin (mouseInfo.fPressAt, eNoUpdate);
	popUp.SetOrigin (menuRect.GetOrigin (), eNoUpdate);		// place popup so it pops up over where we had drawn the bogus menu...
	AddSubView (&popUp);
	(void)popUp.TrackPress (MousePressInfo (mouseInfo, popUp.EnclosureToLocal (mouseInfo.fPressAt)));
	RemoveSubView (&popUp);

	Invert (Rectangle (), labelRect);

	MenuItem* mItem = popUp.GetCurrentItem ();
	CollectionSize newCurrent = (mItem == Nil) ? kBadSequenceIndex : popUp.GetMenu ()->GetMenuItemIndex (mItem);
	if (oldCurrent != newCurrent) {
		SetCurrentItem (newCurrent);
		if (GetController () != Nil) {
			GetController ()->ButtonPressed (this);
		}
	}

	return (True);	// we handled the click...
}

void	OptionMenuButton_MacUI_Portable::Draw (const Region& /*update*/)
{
	DrawText (GetLabel (), kZeroPoint);
	if (GetCurrentItem () == kBadSequenceIndex) {
// look in HIG guidelines for what to do
//		OutLine (Rectangle (), GetLocalExtent ());
	}
	DrawPopUpBox ();
	if (not GetLive ()) {
		Gray ();
	}
}

void	OptionMenuButton_MacUI_Portable::EffectiveFontChanged (const Font& newFont, Panel::UpdateMode updateMode)
{
	/*
	 * On font changes, we invalidate parents layout since our CalcDefaultSize () changes,
	 * and Refresh () since our visual display depends on the current font.
	 */
	OptionMenuButton_MacUI::EffectiveFontChanged (newFont, updateMode);

	View*	parent	=	GetParentView ();
	if (parent != Nil) {
		parent->InvalidateLayout ();		// cuz our CalcDefaultSize () may change
	}

	Refresh (updateMode);					// cuz our display may change
}

String	OptionMenuButton_MacUI_Portable::GetLabel_ () const
{
	return (fLabel);
}

void	OptionMenuButton_MacUI_Portable::SetLabel_ (const String& label, Panel::UpdateMode updateMode)
{
	fLabel = label;
	Refresh (updateMode);
}

const AbSequence(String)&	OptionMenuButton_MacUI_Portable::GetItems_ () const
{
	return (fItems);
}

void	OptionMenuButton_MacUI_Portable::SetItemCount_ (CollectionSize itemCount)
{
	CollectionSize oldLength = fItems.GetLength ();
	for (int i = oldLength; i > itemCount; i--) {
		fItems.RemoveAt (i);
	}
	oldLength = fItems.GetLength ();
	Assert (oldLength <= itemCount);
	for (i = oldLength; i < itemCount; i++) {
		fItems.Append (kEmptyString);
	}
	Ensure (fItems.GetLength () == itemCount);
}

void	OptionMenuButton_MacUI_Portable::SetItem_ (CollectionSize i, const String& item, Panel::UpdateMode /*updateMode*/)
{
	fItems.SetAt (item, i);
}

CollectionSize	OptionMenuButton_MacUI_Portable::GetCurrentItem_ () const
{
	return (fCurrentItem);
}

void	OptionMenuButton_MacUI_Portable::SetCurrentItem_ (CollectionSize i, Panel::UpdateMode updateMode)
{
	fCurrentItem = i;
	Refresh (updateMode);
}

Boolean	OptionMenuButton_MacUI_Portable::GetAutoResize_ () const
{
	return (fAutoResize);
}

void	OptionMenuButton_MacUI_Portable::SetAutoResize_ (Boolean autoResize)
{
	fAutoResize = autoResize;
}

Rect	OptionMenuButton_MacUI_Portable::CalcMenuRect () const
{
	Rect		r			=	GetLocalExtent ();
	Coordinate	itemOffset	=	0;
	if (GetLabel () != kEmptyString) {
		itemOffset	=	TextWidth (GetLabel ()) + GetItemOffset ();
	}
	r.InsetBy (Point (1, 1));
	r.SetTLBR (r.GetTop (), r.GetLeft () + itemOffset, r.GetBottom (), r.GetRight ());
	return (r);
}

#if 1
class	Line1 : public Shape {
	public:
		Line1 (const Point& from = kZeroPoint, const Point& to = kZeroPoint);
		Line1 (LineRepresentation* lineRepresentation);

Line1 (const Line1& l);
const Line1& Line1::operator= (const Line1& rhs);
~Line1 ();


		nonvirtual	Point	GetFrom ()	const;
		nonvirtual	Point	GetTo ()	const;

		nonvirtual	Real	GetSlope ()	const;
		nonvirtual	Real	GetXIntercept ()	const;
		nonvirtual	Real	GetYIntercept ()	const;
		nonvirtual	Point	GetPointOnLine (Real percentFrom)	const;
		nonvirtual	Line	GetPerpendicular (const Point& throughPoint)	const;
		nonvirtual	Line	GetBisector () const;
		nonvirtual	Real	GetLength ()	const;

	private:
		LineRepresentation*	fRepresentation;
};
Line1::Line1 (const Line1& l):
	Shape (l),
	fRepresentation (l.fRepresentation)
	{
	}
const Line1& Line1::operator= (const Line1& rhs)
	{
		Shape::operator= (rhs);
		fRepresentation = rhs.fRepresentation;
		return (*this);
	}
Line1::~Line1 ()
	{
	}


		Line1::Line1 (LineRepresentation* lineRepresentation):
		Shape (lineRepresentation),
		fRepresentation (lineRepresentation)
	{
	}

		Line1::Line1 (const Point& from, const Point& to):
		Shape (Nil),
		fRepresentation (Nil)
	{
		fRepresentation = new LineRepresentation (from, to);
		*this = fRepresentation;
	}

		Point	Line1::GetFrom () const
	{
		AssertNotNil (fRepresentation);
		return (fRepresentation->GetFrom ());
	}

		Point	Line1::GetTo () const
	{
		AssertNotNil (fRepresentation);
		return (fRepresentation->GetTo ());
	}

		Real	Line1::GetSlope () const
	{
		AssertNotNil (fRepresentation);
		return (fRepresentation->GetSlope ());
	}

		Real	Line1::GetXIntercept () const
	{
		AssertNotNil (fRepresentation);
		return (fRepresentation->GetXIntercept ());
	}

		Real	Line1::GetYIntercept () const
	{
		AssertNotNil (fRepresentation);
		return (fRepresentation->GetYIntercept ());
	}

		Point	Line1::GetPointOnLine (Real percentFrom) const
	{
		AssertNotNil (fRepresentation);
		return (fRepresentation->GetPointOnLine (percentFrom));
	}

		Line	Line1::GetPerpendicular (const Point& throughPoint) const
	{
		AssertNotNil (fRepresentation);
		return (fRepresentation->GetPerpendicular (throughPoint));
	}

		Line	Line1::GetBisector () const
	{
		AssertNotNil (fRepresentation);
		return (fRepresentation->GetBisector ());
	}

		Real	Line1::GetLength () const
	{
		AssertNotNil (fRepresentation);
		return (fRepresentation->GetLength ());
	}
#endif


void		OptionMenuButton_MacUI_Portable::DrawPopUpBox ()
{
	Rect	menuRect	=	CalcMenuRect ();
	if (menuRect.GetSize () > kZeroPoint) {
		menuRect.InsetBy (Point (-1, -1));
		const	Coordinate	kRightSlop	=	1;
		const	Coordinate	kLeftSlop	=	5;
		const	Coordinate	kBotSlop	=	6;

		String		theItem	=	GetCurrentItemText ();
		Coordinate	wid		=	menuRect.GetWidth () - (kLeftSlop - kRightSlop);

		// shorten the item to fit...
		for (Coordinate newWid = TextWidth (theItem); (newWid > wid) and (theItem.GetLength () > 0);
			newWid = TextWidth (theItem)) {
			theItem.SetLength (theItem.GetLength () - 1);
			if (theItem.GetLength () != 0) {
				theItem.SetCharAt ('É', theItem.GetLength ());
			}
		}

		// Draw the text.
		DrawText (theItem, Point (menuRect.GetTop (), menuRect.GetLeft () + kLeftSlop));

		/*
		 * Draw the drop-shaddow
		 */
		menuRect = 	Rect (menuRect.GetOrigin (), menuRect.GetSize () - Point (1, 1));
		OutLine (Rectangle (), menuRect);

		OutLine (Line (menuRect.GetBotLeft () + Point (0, 3), menuRect.GetBotRight ()));
		OutLine (Line (menuRect.GetBotRight (), menuRect.GetTopRight () + Point (3, 0)));
	}
}

String		OptionMenuButton_MacUI_Portable::GetCurrentItemText () const
{
	if (GetCurrentItem () == kBadSequenceIndex) {
		return (kEmptyString);
	}
	return (GetItem (GetCurrentItem ()));
}









/*
 ********************************************************************************
 ******************************* OptionMenuButton_MotifUI **********************
 ********************************************************************************
 */

OptionMenuButton_MotifUI::OptionMenuButton_MotifUI ():
	AbstractOptionMenuButton ()
{
	// Must setup proper default font...
	SetFont (&kSystemFont);
}

Boolean	OptionMenuButton_MotifUI::HandleKeyStroke (const KeyStroke& keyStroke)
{
// what to do???
	return (False);
}

Boolean	OptionMenuButton_MotifUI::TrackPress (const MousePressInfo& mouseInfo)
{
	Boolean	refocused = False;
	if ((not GetEffectiveFocused ()) and CanBeFocused ()) {
		GrabFocus (eDelayedUpdate, kValidate);
		refocused = True;
	}

	return (Boolean (AbstractOptionMenuButton::TrackPress (mouseInfo) or refocused));
}

Point	OptionMenuButton_MotifUI::CalcDefaultSize_ (const Point& defaultSize) const
{
// way off - really bad!!!
	const Font&	f = GetEffectiveFont ();
	Coordinate	newWidth = TextWidth (GetLabel ());
	if (GetCurrentItem () == kBadSequenceIndex) {
		newWidth += 50;			// some reasonable default
	}
	else {
		newWidth += TextWidth (GetItem (GetCurrentItem ())) + GetItemOffset () +
			2 +
			5;		// extra whitespace so name not pressed up against drop shadow...
	}
newWidth += 100;
	Coordinate	newHeight	=	f.GetFontHeight () + 3;
//newHeight += 15;	// empirical sluff.. LGP April 23, 1992
newHeight += 20;	// empirical sluff.. LGP April 23, 1992/May 15
	return (Point (newHeight, newWidth));
}

Coordinate	OptionMenuButton_MotifUI::GetItemOffset () const
{
	if (GetLabel () == kEmptyString) {
		return (0);
	}
	else {
// how much???
		return (4);	// leave some room after the text
	}
}

void	OptionMenuButton_MotifUI::EffectiveLiveChanged (Boolean newLive, Panel::UpdateMode updateMode)
{
	AbstractOptionMenuButton::EffectiveLiveChanged (newLive, updateMode);
}










#if		qXmToolkit

/*
 ********************************************************************************
 **************************** OptionMenuButton_MotifUI_Native ******************
 ********************************************************************************
 */
OptionMenuButton_MotifUI_Native::OptionMenuButton_MotifUI_Native (const String& label):
	OptionMenuButton_MotifUI (),
	fLabel (kEmptyString),
	fItems (),
	fCurrentItem (kBadSequenceIndex),
	fOptionMenu (Nil)
{
	class	MyLabelWidget : public MotifOSControl {
		public:
			MyLabelWidget ():
				MotifOSControl ()
			{
			}

			override	osWidget*	DoBuild_ (osWidget* parent)
			{
				RequireNotNil (parent);
				osWidget*	w	=	::XmCreateOptionMenu (parent, "OptionMenu", Nil, 0);
				return (w);
			}	
	};
	AddSubView (fOptionMenu = new MyLabelWidget ());
	SetLabel_ (label, eNoUpdate);
}

OptionMenuButton_MotifUI_Native::~OptionMenuButton_MotifUI_Native ()
{
	if (fOptionMenu != Nil) {
		RemoveSubView (fOptionMenu);
		delete (fOptionMenu);
	}
}

static	Boolean	sButtonPressed = False;	
static	void	HandleButtonUpDuringTrackPressWait (osWidget* /*w*/, char* /*clientData*/, osEventRecord* /*e*/,
													osBoolean* /*continue_to_dispatch*/)
{
	Assert (sButtonPressed);
	sButtonPressed = False;
}

Boolean	OptionMenuButton_MotifUI_Native::TrackPress (const MousePressInfo& mouseInfo)
{
	AssertNotNil (fOptionMenu);
	osWidget*	optionButton	=	fOptionMenu->GetOSRepresentation ();
	osWidget*	menuWidget		=	Nil;
	AssertNotNil (optionButton);
	::XtVaGetValues (optionButton, XmNsubMenuId, &menuWidget, Nil);
	AssertNotNil (menuWidget);

	osEventRecord	er;
	EventManager::Get ().GetCurrentEvent (er);
	er.xbutton.type = ButtonPress;
	er.xbutton.display = XtDisplay (optionButton);
	er.xbutton.window = XtWindow (optionButton);
	AssertNotNil (er.xbutton.window);
	er.xbutton.root = DefaultRootWindow (er.xbutton.display);
	er.xbutton.x = (int)mouseInfo.fPressAt.GetH ();
	er.xbutton.y = (int)mouseInfo.fPressAt.GetV ();
	er.xbutton.x_root = (int)LocalToAncestor (mouseInfo.fPressAt, &DeskTop::Get ()).GetH ();
	er.xbutton.y_root = (int)LocalToAncestor (mouseInfo.fPressAt, &DeskTop::Get ()).GetV ();
	er.xbutton.same_screen = True;
	er.xbutton.button = Button1;

//	Application::Get ().SuspendIdleTasks ();
//	Application::Get ().SuspendCommandProcessing ();

	/*
	 * The below hack of installing an eventhandler and then removing it after a while loop is to provide
	 * the semantics that when you call TrackPress() it doesn't return until the user is done with the event
	 * (ie has finished tracking). This is not the greatest way of solving the problem. In particular, it seems
	 * important to put our handler at the HEAD of the list (thats why we dont call XtAddEventHandler) so that
	 * we dont get called til too late.
	 *
	 * The whole business is necessary, for example, in textedit, you want to inval the menus when the tracking is
	 * done so they get updated properly (thats when we first noticed the bug).
	 */
	::XtInsertEventHandler (optionButton, ButtonReleaseMask, FALSE, HandleButtonUpDuringTrackPressWait, Nil, XtListHead);
	::XtInsertEventHandler (menuWidget, ButtonReleaseMask, FALSE, HandleButtonUpDuringTrackPressWait, Nil, XtListHead);

	sButtonPressed = True;

	::XtDispatchEvent (&er);

	Exception* caught = Nil;
	Try {
		while (sButtonPressed) {
			EventManager::Get ().RunOnce ();
		}
	}
	Catch () {
		caught = _this_catch_;
	}
//	Application::Get ().ResumeIdleTasks ();
//	Application::Get ().ResumeCommandProcessing ();

//gDebugStream << "done with while loop and removing event handler" << newline;
	::XtRemoveEventHandler (menuWidget, ButtonReleaseMask, FALSE, HandleButtonUpDuringTrackPressWait, Nil);
	::XtRemoveEventHandler (optionButton, ButtonReleaseMask, FALSE, HandleButtonUpDuringTrackPressWait, Nil);
	if (caught != Nil) {
		caught->Raise ();
	}

// button changed
	CollectionSize whichButton = GetOSSelectedItem ();
	if (GetCurrentItem () != whichButton) {
		SetCurrentItem (whichButton);
		if (GetController () != Nil) {
			GetController ()->ButtonPressed (this);
		}
	}

	return (True);
}

void	OptionMenuButton_MotifUI_Native::Layout ()
{
	AssertNotNil (fOptionMenu);
	Assert (fOptionMenu->GetOrigin () == kZeroPoint);
	fOptionMenu->SetSize (GetSize ());
	View::Layout ();
}

String	OptionMenuButton_MotifUI_Native::GetLabel_ () const
{
	return (fLabel);
}

void	OptionMenuButton_MotifUI_Native::SetLabel_ (const String& label, UpdateMode updateMode)
{
	fLabel = label;
	AssertNotNil (fOptionMenu);
	fOptionMenu->SetStringResourceValue (XmNlabelString, label);
}

const AbSequence(String)&	OptionMenuButton_MotifUI_Native::GetItems_ () const
{
	return (fItems);
}

void	OptionMenuButton_MotifUI_Native::SetItemCount_ (CollectionSize itemCount)
{
	fItems.SetLength (itemCount, kEmptyString);
	RebuildOSMenu ();
	Ensure (GetItemCount () == itemCount);
}

void	OptionMenuButton_MotifUI_Native::SetItem_ (CollectionSize i, const String& item, UpdateMode updateMode)
{
	fItems.SetAt (item, i);
	RebuildOSMenu ();
}

CollectionSize	OptionMenuButton_MotifUI_Native::GetCurrentItem_ () const
{
	return (fCurrentItem);
}

void	OptionMenuButton_MotifUI_Native::SetCurrentItem_ (CollectionSize index, UpdateMode updateMode)
{
	fCurrentItem = index;
	osWidget*	optionButton	=	fOptionMenu->GetOSRepresentation ();
	if (optionButton != Nil) {
		osWidget*	menuWidget		=	Nil;
		::XtVaGetValues (optionButton, XmNsubMenuId, &menuWidget, Nil);
		AssertNotNil (menuWidget);

		if (index == kBadSequenceIndex) {
			::XtVaSetValues (optionButton, XmNmenuHistory, Nil, Nil);
		}
		else {
			WidgetList	children;
			Cardinal	numChildren		=	0;
			::XtVaGetValues (menuWidget, XtNnumChildren, &numChildren, XtNchildren, &children, Nil);

			CollectionSize	selectedItemIndex	=	0;
			for (int i = 0; i < numChildren; i++) {
				/* count only PushB */
				if ((XmIsPushButtonGadget (children[i])) || (XmIsPushButton(children[i]))) {
					selectedItemIndex++;
				}
				if (selectedItemIndex == index) {
					::XtVaSetValues (optionButton, XmNmenuHistory, children[i], Nil);
					return;
				}
			}
			AssertNotReached ();	// should find it in our list!!!
		}
	}
// Not sure necessary - probably redrwas self anyhow.. 	Refresh (updateMode);
}

void	OptionMenuButton_MotifUI_Native::Realize (osWidget* parent)
{
	OptionMenuButton_MotifUI::Realize (parent);
	RebuildOSMenu ();
	// hack to force update of widget values...
	SetLabel_ (GetLabel (), eNoUpdate);
	SetCurrentItem_ (GetCurrentItem (), eNoUpdate);
}

void	OptionMenuButton_MotifUI_Native::UnRealize ()
{
	AssertNotNil (fOptionMenu);
	osWidget*	optionButton	=	fOptionMenu->GetOSRepresentation ();
	if (optionButton != Nil) {
		osWidget*	menuWidget		=	Nil;
		::XtVaGetValues (optionButton, XmNsubMenuId, &menuWidget, Nil);
		if (menuWidget != Nil) {
// should reset pointer to nil but causes Xm to crash so leave pointer dangling.
// LGP April 23, 1992
//			::XtVaSetValues (optionButton, XmNsubMenuId, Nil, Nil);
			::XtDestroyWidget (menuWidget); menuWidget = Nil;
		}
	}
	OptionMenuButton_MotifUI::UnRealize ();
}

void	OptionMenuButton_MotifUI_Native::RebuildOSMenu ()
{
	/*
	 * Rebuild motif os menu from our saved data structures. Can be used from realization, or
	 * when someone changes our list of menu items, or text, while still managed.
	 */
	AssertNotNil (fOptionMenu);
	osWidget*	optionButton	=	fOptionMenu->GetOSRepresentation ();
	if (optionButton != Nil) {
		osWidget*	parent			=	XtParent (optionButton);
		RequireNotNil (parent);

		osWidget*	menuWidget		=	Nil;
		::XtVaGetValues (optionButton, XmNsubMenuId, &menuWidget, Nil);

		/*
		 * Remove old, if any...
		 */
		if (menuWidget != Nil) {
// should reset pointer to nil but causes Xm to crash so leave pointer dangling.
// Copied workaround from above - april 30, 1992
//			::XtVaSetValues (optionButton, XmNsubMenuId, Nil, Nil);
			::XtDestroyWidget (menuWidget); menuWidget = Nil;
		}

		/*
		 * Now build new.
		 */
		Assert (menuWidget == Nil);
		menuWidget = ::XmCreatePulldownMenu (parent, "Pulldown Menu INSIDE OptionMenu", Nil, 0);
		::XtVaSetValues (optionButton, XmNsubMenuId, menuWidget, Nil);

		/*
		 * Add pushbuttongadgets for each string item in our list.
		 */
		ForEach (String, it, fItems) {
			char	tmp[1000];
			it.Current ().ToCStringTrunc (tmp, sizeof (tmp));
			osWidget*	item	=	::XmCreatePushButtonGadget (menuWidget, tmp, Nil, 0);
			AssertNotNil (item);
			::XtManageChild (item);
		}
	}
}

CollectionSize	OptionMenuButton_MotifUI_Native::GetOSSelectedItem () const
{
	RequireNotNil (fOptionMenu);
	RequireNotNil (fOptionMenu->GetOSRepresentation ());
	osWidget*	optionButton	=	fOptionMenu->GetOSRepresentation ();

	osWidget*	selectedWidget	=	Nil;
	osWidget*	menuWidget		=	Nil;
	::XtVaGetValues (optionButton, XmNmenuHistory, &selectedWidget, XmNsubMenuId, &menuWidget, Nil);
	AssertNotNil (menuWidget);
	Assert (selectedWidget == Nil or XtParent (selectedWidget) == menuWidget);	// sanity test
	Assert (selectedWidget == Nil or XmIsPushButtonGadget (selectedWidget) or XmIsPushButton (selectedWidget));

	WidgetList	children;
	Cardinal	numChildren		=	0;
	::XtVaGetValues (menuWidget, XtNnumChildren, &numChildren, XtNchildren, &children, Nil);

	CollectionSize	selectedItemIndex	=	0;
	for (int i = 0; i < numChildren; i++) {
		/* count only PushB */
		if ((XmIsPushButtonGadget (children[i])) || (XmIsPushButton(children[i]))) {
			selectedItemIndex++;
		}
		if (selectedWidget == children[i]) {
			Ensure (selectedItemIndex != 0);
			return (selectedItemIndex);
		}
	}
	Ensure (selectedWidget == Nil);	// otherwise we should have found it???
	return (kBadSequenceIndex);
}

#endif	/*qXmToolkit*/









/*
 ********************************************************************************
 ********************************* OptionMenuButton *****************************
 ********************************************************************************
 */

OptionMenuButton::OptionMenuButton (const String& label, ButtonController* controller) :
#if		qMacUI
	OptionMenuButton_MacUI_Portable (label)
#elif	qMotifUI
#if		qXmToolkit
	OptionMenuButton_MotifUI_Native (label)
#else
	OptionMenuButton_MacUI_Portable (label)
#endif
#endif	/*GUI*/
{
	SetController (controller);
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***
