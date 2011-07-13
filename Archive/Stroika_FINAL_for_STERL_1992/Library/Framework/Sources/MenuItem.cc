/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/MenuItem.cc,v 1.7 1992/09/08 15:34:00 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: MenuItem.cc,v $
 *		Revision 1.7  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.6  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/07/21  21:29:31  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.4  1992/07/02  04:55:02  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *		Revision 1.62  1992/05/18  16:22:50  lewis
 *		Cleanup compiler warnigns.
 *
 *		Revision 1.61  92/04/20  14:26:25  14:26:25  lewis (Lewis Pringle)
 *		Added typedef char* caddr_t; in #if qSnake && _POSIX_SOURCE since xutils.h the distribute not posix complaint.
 *		
 *		Revision 1.60  92/04/17  13:48:18  13:48:18  sterling (Sterling Wight)
 *		hack support for popupmenuitems (never disabled)
 *		
 *		Revision 1.59  92/04/02  13:07:26  13:07:26  lewis (Lewis Pringle)
 *		Instead of manually re-#defining symbols undefs in Intrinsic.h, we just re-include osrenamepre/post after that
 *		file. This is better because as we add more renames, its painfull having to update all these other places. Hopefully
 *		the entire hack can go away at some point.
 *		
 *		Revision 1.57  1992/03/10  00:08:56  lewis
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *
 *		Revision 1.55  1992/02/20  06:30:32  lewis
 *		Updated to reflect change in definition of accelerators.
 *
 *		Revision 1.54  1992/02/19  17:18:34  lewis
 *		Support new accelerator class.
 *
 *		Revision 1.53  1992/02/18  02:23:23  lewis
 *		Tried to hack accelerators further, but with no success.
 *
 *		Revision 1.52  1992/02/18  02:01:27  lewis
 *		Did cut at implementing accelerators, but quite inadquate. We must change our representation
 *		of accelerators, for one thing, to be more compatable with the diversity of accelerators under
 *		motif. Also, actual acceleration did not work. not sure why.
 *
 *		Revision 1.51  1992/02/14  03:15:17  lewis
 *		Got rid of fNativeOwner - no longer needed since 1 kind of menu.
 *
 *		Revision 1.50  1992/02/12  06:58:29  lewis
 *		Change from AbstractMenu to Menu.
 *
 *		Revision 1.48  1992/02/04  17:06:18  lewis
 *		Add virtual method to MenuItem for MacToolkit IsNativeItem() and have it return false by default.
 *		This is overridden by the native items to return true, and used by AbstractMenu to check if the
 *		native MDEF can be used.
 *
 *		Revision 1.47  1992/02/04  07:22:59  lewis
 *		comment out pluschangecount/minuschangecount calls since cause assertion failure. Whats going on there
 *		doesn't quite make sense to me anyhow.
 *
 *		Revision 1.46  1992/02/04  04:30:26  lewis
 *		General Cleanups.
 *
 *		Revision 1.45  1992/02/03  22:27:57  lewis
 *		Moved CommandSelection to MenuCommandHandler.cc.
 *		Use Gadgets instead of widgets.
 *		Move all the various kinds of menuitems into seperate files (eg StringMenuItem.cc).
 *
 *		Revision 1.44  1992/01/31  16:46:37  sterling
 *		minor cleanups and optimizations
 *
 *		Revision 1.42  1992/01/29  06:26:52  sterling
 *		minor cleanups
 *
 *		Revision ??? lewis
 *		Use portable representation of elipsis character for mac code (ie instead of ... in 1 char say \311).
 *
 *		Revision 1.31  1991/12/27  19:21:35  lewis
 *		merged in sterls changes.
 *
 *		Revision 1.30  1991/12/27  17:04:16  lewis
 *		Fixed XtSetArg calls to take arg[0] instead of arg[1].
 *
 *
 */



#include	<string.h>

#include	"OSRenamePre.hh"
#if		qMacOS
#include	<Events.h>				// just for cmdKey
#include	<Memory.h>
#include	<Menus.h>
#include	<OSUtils.h>				// for trap caching
#include	<Resources.h>			// to snag default mdef
#include	<Traps.h>				// for trap caching
#include	<ToolUtils.h>			// just for HiWord/LoWord
#include	<QuickDraw.h>
#include	<Windows.h>
#include	<SysEqu.h>				// for GrayRgn
#elif	qXGDI
#if		qSnake && _POSIX_SOURCE
typedef	char*	caddr_t;		// work around hp POSIX headers bug
#endif
#include	<X11/Intrinsic.h>
#include	"OSRenamePost.hh"		// they undef a bunch of things we define
#include	"OSRenamePre.hh"		// so blast back!!!
#include	<Xm/Xm.h>
#endif	/*GDI*/
#include	"OSRenamePost.hh"

#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"Menu.hh"
#include	"MenuOwner.hh"

#include	"MenuItem.hh"




// SSW needed for popuphack
#include "Command.hh"




/*
 ********************************************************************************
 ********************************** MenuItem ************************************
 ********************************************************************************
 */

static	const	String	kUnknownMenuItemName	=	String (String::eReadOnly, "Unknown menu item");

#if		!qRealTemplatesAvailable
	Implement (Iterator, MenuItemPtr);
	Implement (Collection, MenuItemPtr);
	Implement (AbSequence, MenuItemPtr);
	Implement (Array, MenuItemPtr);
	Implement (Sequence_Array, MenuItemPtr);
	Implement (Sequence, MenuItemPtr);
#endif

MenuItem::MenuItem (CommandNumber commandNumber):
	Toggle (Nil),
	CommandSelection (commandNumber),
#if 	qSupportMneumonics
	fMneumonic (kEmptyString),
#endif
	fDefaultName (kUnknownMenuItemName),
	fOldEnabled (kEnabled),
	fOldOn (not Toggle::kOn),
	fOldName (kUnknownMenuItemName),
	fCachedAccelerator (MenuOwner::kNoAccelerator),
	fOSInitialized (False),
	fOwner (Nil),
	fIndex (kBadSequenceIndex)
{
	if (commandNumber != CommandHandler::eNoCommand) {
		String	name = kUnknownMenuItemName;
		CommandNameTable::Get ().Lookup (commandNumber, &name);
		SetDefaultName (name);
		SetName (name);
	}
}

MenuItem::~MenuItem ()
{
#if		qXtToolkit
	Assert (GetWidget () == Nil);
#endif
}
	
#if		qSupportMneumonics
String	MenuItem::GetMneumonic () const
{
	return (fMneumonic);
}

void	MenuItem::SetMneumonic (const String& mneumonic)
{
	if (GetMneumonic () != mneumonic) {
		SetMneumonic_ (mneumonic);
	}
	Ensure (GetMneumonic () == mneumonic);
}
#endif	/*qSupportMneumonics*/	
		
void	MenuItem::SetOn_ (Boolean on, UpdateMode /*updateMode*/)
{
	/*
	 * Never any need to pay attention to updateMode since we are always contained in a menu. We assume this does
	 * not get called during a menu selection - if it did, we would have to change this. Its only a performance
	 * tweak anyway.
	 */


// Ask sterl to comment this better???? He thinks the #if 0 case is RIGHT...
#if 0
	if (fOldOn == on) {
		MinusChangeCount ();
	}
	else {
		PlusChangeCount ();
	}
#else
	PlusChangeCount ();
#endif
	Toggle::SetOn_ (on, eNoUpdate);
}

String	MenuItem::GetDefaultName () const
{
	return (fDefaultName);
}

void	MenuItem::SetDefaultName (const String& name)
{
	if (GetDefaultName () != name) {
		SetDefaultName_ (name);
	}
	Ensure (GetDefaultName () == name);
}

void	MenuItem::Reset ()
{
	ResetChangeCount ();
	if (not fOSInitialized) {
		SetDirty ();
	}

	fOldOn = GetOn ();
	fOldEnabled = GetEnabled ();
	fOldName = GetName ();
	Reset_ ();
	
	Ensure (GetName () == GetDefaultName ());
// SSW popupitem hack -- see other comment
	Ensure (not GetEnabled () or (GetCommandNumber () == CommandHandler::ePopupItem));
//	Ensure (not GetEnabled ());
	Ensure (not GetOn ());
}
		
void	MenuItem::SetEnabled_ (Boolean enabled, UpdateMode /*updateMode*/)
{
	if (fOldEnabled == enabled) {
		MinusChangeCount ();
	}
	else {
		PlusChangeCount ();
	}
	Button::SetEnabled_ (enabled, eNoUpdate);
}

void	MenuItem::SetDefaultName_ (const String& name)
{
	fDefaultName = name;
	PlusChangeCount ();
}
	
#if		qSupportMneumonics	
void	MenuItem::SetMneumonic_ (const String& mneumonic)
{
	fMneumonic = mneumonic;
	PlusChangeCount ();
}
#endif	/*qSupportMneumonics*/
		

Accelerator		MenuItem::GetAccelerator () const
{
	if (not fOSInitialized) {
		Accelerator	accelerator = MenuOwner::kNoAccelerator;
		MenuOwner::GetAccelerators ().Lookup (GetCommandNumber (), &accelerator);
		MenuItem&	hackThis = *(MenuItem*)this;
		hackThis.fCachedAccelerator = accelerator;
	}
	return (fCachedAccelerator);
}

void	MenuItem::SetCommandNumber_ (CommandNumber commandNumber)
{
	CommandSelection::SetCommandNumber_ (commandNumber);
	PlusChangeCount ();
}

void	MenuItem::SetName_ (const String& name)
{
	CommandSelection::SetName_ (name);
// LGP changed Feb 4 cuz asserted out otherwise with (changeocnt < 0) Ask sterl about this???
#if 0
	if (fOldName == name) {
		MinusChangeCount ();
	}
	else {
		PlusChangeCount ();
	}
#else
	PlusChangeCount ();
#endif
}

CollectionSize	MenuItem::GetIndex_ () const
{
	return (fIndex);
}

Menu*	MenuItem::GetOwner () const
{
	return (fOwner);
}

void	MenuItem::Reset_ ()
{
	SetOn (not Toggle::kOn, View::eNoUpdate);
// SSW quickee hack to get popup items to work, not sure what the 'right' solution to
// this is, maybe always have a magic command number (eAlwaysEnabled) and then retarget
// things like ePopupItem
SetEnabled (Boolean (GetCommandNumber () == CommandHandler::ePopupItem), View::eNoUpdate);
//	SetEnabled (not kEnabled, View::eNoUpdate);
	SetName (GetDefaultName ());
}

void	MenuItem::UpdateOSRep ()
{
	UpdateOSRep_ ();
	fOSInitialized = True;
}

void	MenuItem::UpdateOSRep_ ()
{
#if		qMacToolkit
	RequireNotNil (fOwner);		// must be in a menu...
	if (GetEnabled ()) {
		::osEnableItem (fOwner->GetOSRepresentation (), (short)GetIndex ());
	}
	else {
		::DisableItem (fOwner->GetOSRepresentation (), (short)GetIndex ());
	}

	if (not fOSInitialized) {
		// IM V-241 warns explicitly against doing this, but we know better...
		Accelerator accelerator = GetAccelerator ();
		if (accelerator.Empty ()) {
			::SetItemCmd (fOwner->GetOSRepresentation (), short (GetIndex ()), 0);
		}
		else {
			::SetItemCmd (fOwner->GetOSRepresentation (), short (GetIndex ()), accelerator.GetAccChar ().GetAsciiCode ());
		}
	}
#elif	qXmToolkit
	RequireNotNil (GetWidget ());

	Arg	args [4];
	XtSetArg (args[0], XmNset, GetOn ());
	XtSetArg (args[1], XmNsensitive, GetEnabled ());
	Accelerator accelerator = GetAccelerator ();
	char	acceleratorValue [1024];
	char	acceleratorLabel [1024];
	if (accelerator.Empty ()) {
		acceleratorValue [0] = '\0';
		acceleratorLabel [0] = '\0';
	}
	else {
String x= kEmptyString; x += accelerator.GetAccChar ();
		String v = String ("Ctrl<Key>") + tolower (x);
		(void)v.ToCStringTrunc (acceleratorValue, sizeof (acceleratorValue));
		String l = String ("Ctrl+") + x;
		(void)l.ToCStringTrunc (acceleratorLabel, sizeof (acceleratorLabel));
	}
	XtSetArg (args[2], XmNaccelerator, acceleratorValue);
	XmString mStrTmp = XmStringCreate (acceleratorLabel, XmSTRING_DEFAULT_CHARSET);
	XtSetArg (args[3], XmNacceleratorText, mStrTmp);
	::XtSetValues (GetWidget (), args, 4);
	::XmStringFree (mStrTmp);
#endif	/*Toolkit*/
}

#if		qMacToolkit
Boolean	MenuItem::IsNativeItem () const
{
	return (False);		// default is false - only the native controls override and say TRUE
}
#endif







// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***
