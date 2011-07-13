/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/FocusItem.cc,v 1.6 1992/09/08 15:34:00 lewis Exp $
 *
 * TODO:
 *
 *		Should do FocusOwner::DTOR which asserts all items removed...
 *
 *
 * Changes:
 *	$Log: FocusItem.cc,v $
 *		Revision 1.6  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/21  21:29:31  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.3  1992/07/03  00:25:14  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *		And, general cleanups - shorten lines.
 *
 *		Revision 1.2  1992/07/02  04:52:31  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *		Revision 1.1  1992/06/20  17:30:04  lewis
 *		Initial revision
 *
 *		Revision 1.30  1992/05/18  17:22:20  lewis
 *		Dont call SetCurrentFocus (Nil) in AbstractFocusItem::DTOR since many of items
 *		may already be destroyed - in fact this happens often. Probably should assert
 *		already nil - for now, just check when debug on, and print message.
 *
 *		Revision 1.29  92/04/15  13:42:50  13:42:50  sterling (Sterling Wight)
 *		changed from Option to Shift key for backwards tab
 *		
 *		Revision 1.28  92/03/26  15:15:20  15:15:20  lewis (Lewis Pringle)
 *		Changed EffectiveFocusedChanged () to EffectiveFocusChanged(), and got rid of
 *		oldFocused first argument.
 *		
 *		Revision 1.27  1992/03/13  16:36:23  lewis
 *		Moved FocusView to User.
 *
 *		Revision 1.24  1992/03/10  00:30:34  lewis
 *		Use new HandleKeyStroke interface, and change GetTab interface to use KeyStroike
 *		rather than KeyBoard.
 *
 *		Revision 1.23  1992/03/06  22:17:32  lewis
 *		Minor cleanups like getting rid of ifdefs out stuff, and unneeded dtors.
 *		Also, undid a patch sterl had done but seemed unwise - in TrackPress for FocusView...
 *
 *		Revision 1.22  1992/02/24  06:46:26  lewis
 *		Added if macui around focusitem stuff as hack for sterls/simones demo til we
 *		clean things up a bit - sterl - lewis says: FocusItemView bad idea to begin with...
 *
 *		Revision 1.19  1992/02/17  16:36:04  lewis
 *		Worked around assert for focusitem differently than before since that change broke things.
 *		Just comment out the assert. Must revisit the issue soon!!!
 *
 *		Revision 1.18  1992/02/16  18:06:39  lewis
 *		Change definition of FocusItem::GetEffectiveFocused ().... Discuss with sterl...
 *
 *		Revision 1.17  1992/02/15  05:32:34  sterling
 *		many changes, includeng adding EffectiveFocusedChanged
 *
 *		Revision 1.16  1992/01/31  21:11:03  lewis
 *		In FocusItem::CanBeFocused(), we add GetFocusOwner()!=Nil to conjunction.
 *		And in AbstractFocusOwner we just call GetLive() in CanBeFocused () instead
 *		of inherited::CanBeFocusued.
 *
 *		Revision 1.15  1992/01/31  16:43:17  sterling
 *		added grabfocus
 *
 *		Revision 1.14  1992/01/22  15:18:29  sterling
 *		fixed tabs
 *
 *		Revision 1.11  1992/01/08  06:25:13  lewis
 *		Sterl- lots of changes - mostly tabbing related.
 *
 *
 *
 */



#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"Shape.hh"

#include	"Adornment.hh"
#include	"Dialog.hh"
#include	"MenuOwner.hh"
#include	"View.hh"

#include	"FocusItem.hh"






#if		!qRealTemplatesAvailable
	Implement (Iterator, FocusItemPtr);
	Implement (Collection, FocusItemPtr);
	Implement (AbSequence, FocusItemPtr);
	Implement (Array, FocusItemPtr);
	Implement (Sequence_Array, FocusItemPtr);
	Implement (Sequence, FocusItemPtr);
#endif






/*
 ********************************************************************************
 ******************************* FocusException *********************************
 ********************************************************************************
 */
FocusException::FocusException ():
	fMessage (kEmptyString)
{
}

void	FocusException::Raise ()
{
	if (Exception::GetCurrent () != this) {		// if were not the current exception
		fMessage = kEmptyString;
	}
	Exception::Raise ();
}

void	FocusException::Raise (const String& message)
{
	fMessage  = message;
	Exception::Raise ();
}

String	FocusException::GetMessage () const
{
	return (fMessage);
}









/*
 ********************************************************************************
 ********************************* FocusItem ************************************
 ********************************************************************************
 */
const	Boolean	FocusItem::kFocused = True;
const	Boolean	FocusItem::kValidate = True;

FocusItem::FocusItem ():
	KeyHandler (),
	MenuCommandHandler (),
	LiveItem (),
	fFocused (not kFocused),
	fOwner (Nil)
{
}

void	FocusItem::SetFocused (Boolean focused, Panel::UpdateMode updateMode, Boolean validate)
{
	if (GetFocused () != focused) {
		SetFocused_ (focused, updateMode, validate);
	}
	Ensure (GetFocused () == focused);
}

Boolean	FocusItem::GetFocused_ () const
{
	return (fFocused);
}

void	FocusItem::SetFocused_ (Boolean focused, Panel::UpdateMode updateMode, Boolean validate)
{
	Require (focused != fFocused);
	
	Boolean	oldFocused = GetEffectiveFocused ();
	if (validate and (not focused)) {
		Validate ();
	}
	fFocused = focused;
	
	if (oldFocused != GetEffectiveFocused ()) {
		EffectiveFocusChanged (not oldFocused, updateMode);
	}
}

Boolean	FocusItem::GetEffectiveFocused () const
{
	AbstractFocusOwner* owner = GetFocusOwner ();
	return (Boolean (GetFocused () and ((owner == Nil) or (owner->GetEffectiveFocused ()))));
}

void	FocusItem::EffectiveFocusChanged (Boolean /*newFocused*/, Panel::UpdateMode /*updateMode*/)
{
	MenuOwner::SetMenusOutOfDate ();
}

AbstractFocusOwner*	FocusItem::GetFocusOwner () const
{
	return (fOwner);
}

void	FocusItem::SetFocusOwner (AbstractFocusOwner* focusOwner)
{
	Require ((fOwner == Nil) or (focusOwner == Nil));
	fOwner = focusOwner;
}

Boolean	FocusItem::CanBeFocused ()
{
	/*
	 * By default, focus items can be focused if they are live.
	 */
	return (GetEffectiveLive ());
}

void	FocusItem::GrabFocus (Panel::UpdateMode updateMode, Boolean validate)
{
	AbstractFocusOwner* owner = GetFocusOwner ();

	if (owner == Nil) {
		SetFocused (kFocused, updateMode);
	}
	else {
		if (validate) {
			// set owners focus to Nil first to ensure validates get called early on
			owner->SetCurrentFocus ((FocusItem*)Nil, updateMode, validate);
		}
		owner->GrabFocus (updateMode, validate);
		owner->SetCurrentFocus (this, updateMode, validate);
	}
}

void	FocusItem::TabbingFocus (SequenceDirection /*d*/,
								Panel::UpdateMode updateMode)
{
	GrabFocus (updateMode, kValidate);
}

Boolean	FocusItem::HandleTab (SequenceDirection /*d*/,
								Boolean /*wrapping*/)
{
	return (False);
}

void	FocusItem::Validate ()
{
}









/*
 ********************************************************************************
 ***************************** AbstractFocusOwner *******************************
 ********************************************************************************
 */
const	Boolean	AbstractFocusOwner::kWrap = True;

AbstractFocusOwner::AbstractFocusOwner ():
	fFocus (Nil)
{
}

AbstractFocusOwner::~AbstractFocusOwner ()
{
//	Require (GetCurrentFocus () == Nil);
#if		qDebug
	if (GetCurrentFocus () != Nil) {
		gDebugStream << "destroying abstract focus ownerwith current focus set!!!" << newline;
	}
#endif
#if 0
// LGP May 18, 1992  - if anything, assert that current focus is Nil - when we are being destroyed, our children
// may well be already!!! In fact, that should be quite common!!!
	SetCurrentFocus ((FocusItem*)Nil, Panel::eNoUpdate, False);
#endif
}

void	AbstractFocusOwner::EffectiveFocusChanged (Boolean newFocused, Panel::UpdateMode updateMode)
{
	FocusItem::EffectiveFocusChanged (newFocused, updateMode);
	
	if (GetCurrentFocus () != Nil) {
		GetCurrentFocus ()->EffectiveFocusChanged (newFocused, updateMode);
	}
}

void	AbstractFocusOwner::Validate ()
{
	if (GetCurrentFocus () != Nil) {
		GetCurrentFocus ()->Validate ();
	}
}

Boolean	AbstractFocusOwner::CanBeFocused ()
{
	if (GetEffectiveLive ()) {
		ForEach (FocusItemPtr, it, MakeFocusIterator ()) {
			AssertNotNil (it.Current ());
			if (it.Current ()->CanBeFocused ()) {
				return (True);
			}
		}	
	}
	return (False);
}

void	AbstractFocusOwner::DoSetupMenus ()
{
	if (GetCurrentFocus () != Nil) {
		GetCurrentFocus ()->DoSetupMenus ();
	}
}

Boolean	AbstractFocusOwner::DoCommand (const CommandSelection& selection)
{
	if (GetCurrentFocus () != Nil) {
		return (GetCurrentFocus ()->DoCommand (selection));
	}
	return (False);
}

void	AbstractFocusOwner::TabbingFocus (SequenceDirection d,
											Panel::UpdateMode updateMode)
{
	SetCurrentFocus ((FocusItem*)Nil, updateMode, kValidate);

	FocusItem*	nextFocus = CalcNextFocus (d, True);
	if (nextFocus == Nil) {
		FocusItem::TabbingFocus (d, updateMode);
	}
	else {
		nextFocus->TabbingFocus (d, updateMode);
	}
}

Boolean	AbstractFocusOwner::HandleTab (SequenceDirection d,
										Boolean wrapping)
{
	FocusItem*	currentFocus = GetCurrentFocus ();
	if (currentFocus != Nil) {
		if (currentFocus->HandleTab (d, wrapping)) {
			return (True);
		}
	}

	FocusItem*	nextFocus	=	CalcNextFocus (d, wrapping);
	SetCurrentFocus ((FocusItem*)Nil, View::eDelayedUpdate, kValidate);
	if (nextFocus != currentFocus) {
		if (nextFocus != Nil) {
			nextFocus->TabbingFocus (d, View::eDelayedUpdate);
		}
		return (True);
	}
	return (False);
}

Boolean	AbstractFocusOwner::HandleKeyStroke (const KeyStroke& keyStroke)
{
	SequenceDirection	direction;
	Boolean 					wrapping;

	if (GetTab (keyStroke, direction, wrapping)) {
		if (HandleTab (direction, wrapping)) {
			return (True);
		}
		else if (not wrapping) {
			return (HandleTab (direction, True));
		}
	}

	return (False);
}

Boolean	AbstractFocusOwner::DispatchKeyEvent (KeyBoard::KeyCode code, Boolean isUp,
												const KeyBoard& keyBoardState,
												KeyComposeState& composeState)
{
	/*
	 * See if we handle it for tabbing purposes, otherwise pass to our focus.
	 */
	if (HandleKeyEvent (code, isUp, keyBoardState, composeState)) {
		return (True);
	}
	else {
		if (GetCurrentFocus () != Nil) {
			return (GetCurrentFocus ()->DispatchKeyEvent (code, isUp, keyBoardState, composeState));
		}
	}
	return (False);
}

Boolean	AbstractFocusOwner::GetTab (const KeyStroke& keyStroke,
									SequenceDirection& d,
									Boolean& wrapping)
{
	KeyStroke	ks	=	keyStroke.GetCharacter ();

	if (ks == KeyStroke::kTab) {
		if (keyStroke.GetModifiers ().Contains (KeyStroke::eShiftKeyModifier)) {
			d = eSequenceBackward;
		}
		else {
			d = eSequenceForward;
		}
		wrapping = False;
		return (True);
	}
	else if (ks == KeyStroke::kDownArrow) {
		d = eSequenceForward;
		wrapping = True;
		return (True);
	}
	else if (ks == KeyStroke::kUpArrow) {
		d = eSequenceBackward;
		wrapping = True;
		return (True);
	}

	return (False);
}

FocusItem*	AbstractFocusOwner::GetCurrentFocus () const
{
	return (fFocus);
}

void	AbstractFocusOwner::SetCurrentFocus (FocusItem* newFocus,
											Panel::UpdateMode updateMode, Boolean validate)
{
	if (newFocus != GetCurrentFocus ()) {
		SetCurrentFocus_ (newFocus, updateMode, validate);
	}
	Ensure (GetCurrentFocus () == newFocus);
}

void	AbstractFocusOwner::SetCurrentFocus (SequenceDirection d,
											Panel::UpdateMode updateMode, Boolean validate)
{
	SetCurrentFocus (CalcNextFocus (d, True), updateMode, validate);
}

void 	AbstractFocusOwner::SetCurrentFocus_ (FocusItem* newFocus, Panel::UpdateMode updateMode,
												Boolean validate)
{
	Require (fFocus != newFocus);
	if (GetCurrentFocus () != Nil) {
		GetCurrentFocus ()->SetFocused (not kFocused, updateMode, validate);
	}

	fFocus = newFocus;

	if (GetCurrentFocus () != Nil) {
		GetCurrentFocus ()->SetFocused (kFocused, updateMode);
	}
}

FocusItem*	AbstractFocusOwner::CalcNextFocus (SequenceDirection d,
												Boolean allowWrapAround)
{
	Boolean	matched = Boolean (fFocus == Nil);
	{
		ForEach (FocusItemPtr, it, MakeFocusIterator (d)) {
			FocusItem*	c	=	it.Current ();
			AssertNotNil (c);
			if (matched) {
				Assert (c != fFocus);
				if (c->CanBeFocused ()) {
					return (c);
				}
			}
			else {
				matched = Boolean (c == fFocus);
			}
		}
	}
	if (matched and allowWrapAround) {
		// wrap around search
		ForEach (FocusItemPtr, it, MakeFocusIterator (d)) {
			FocusItem*	c	=	it.Current ();
			AssertNotNil (c);
			if (c == fFocus) {
				// wrap around failed, could find no new candidate
				break;
			}
			else {
				if (c->CanBeFocused ()) {
					return (c);
				}
			}
		}
	}

	return (fFocus);	// failed to find a new guy
}

void	AbstractFocusOwner::SetOwnerOfFocusItem (FocusItem* focus, AbstractFocusOwner* owner)
{
	Require ((owner == this) or (owner == Nil));
	RequireNotNil (focus);
	Require (focus->GetFocusOwner () != owner);
	focus->SetFocusOwner (owner);
}








/*
 ********************************************************************************
 ********************************* FocusOwner ***********************************
 ********************************************************************************
 */
FocusOwner::FocusOwner ():
	AbstractFocusOwner (),
	fFocusList ()
{
}

void	FocusOwner::AddFocus (FocusItem* focus, CollectionSize index)
{
	RequireNotNil (focus);
	Require (focus->GetFocusOwner () == Nil);
	Require (not focus->GetFocused ());
	
	if (index == eAppend) {
		index = fFocusList.GetLength () + 1;
	}

	fFocusList.InsertAt (focus, index);
	SetOwnerOfFocusItem (focus, this);
}
		
void	FocusOwner::AddFocus (FocusItem* focus, FocusItem* neighborFocus, AddMode addMode)
{
	RequireNotNil (neighborFocus);
	CollectionSize	index = fFocusList.IndexOf (neighborFocus);
	Require (index != kBadSequenceIndex);
	AddFocus (focus, (addMode == eAppend) ? index + 1 : index);
}

void	FocusOwner::RemoveFocus (FocusItem* focus)
{
	RequireNotNil (focus);
	Require (focus->GetFocusOwner () == this);

	if (focus == GetCurrentFocus ()) {
		SetCurrentFocus ((FocusItem*)Nil, Panel::eDelayedUpdate, not kValidate);
	}
	fFocusList.Remove (focus);
	SetOwnerOfFocusItem (focus, Nil);
}

void	FocusOwner::ReorderFocus (FocusItem* focus, CollectionSize index)
{
	RequireNotNil (focus);
	if (index == eAppend) {
		index = fFocusList.GetLength ();	//	not count + 1 cuz we will remove first,
											//	shrinking list temporarily
	}
	fFocusList.Remove (focus);
	fFocusList.InsertAt (focus, index);
	Ensure (fFocusList.IndexOf (focus) == index);
}

void	FocusOwner::ReorderFocus (FocusItem* focus, FocusItem* neighborFocus, AddMode addMode)
{
	RequireNotNil (neighborFocus);
	Require (focus != neighborFocus);
	
	CollectionSize	index = fFocusList.IndexOf (neighborFocus);
	CollectionSize	oldIndex = fFocusList.IndexOf (focus);

	Require (index != kBadSequenceIndex);
	Require (oldIndex != kBadSequenceIndex);
	
	if (oldIndex < index) {
		index--;
	}
	ReorderFocus (focus, (addMode == eAppend) ? index + 1 : index);
}

SequenceIterator(FocusItemPtr)* FocusOwner::MakeFocusIterator (SequenceDirection
																d)
{
	return (fFocusList.MakeSequenceIterator (d));
}

Boolean	FocusOwner::GetLive () const
{
	return (True);
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***
