/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/SeparatorMenuItem.cc,v 1.7 1992/09/08 15:34:00 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: SeparatorMenuItem.cc,v $
 *		Revision 1.7  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.6  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/07/21  20:12:43  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.4  1992/07/14  19:58:43  lewis
 *		Cleanups, and rename Seperator->Separator.
 *
 *		Revision 1.3  1992/07/03  02:15:48  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.2  1992/06/25  08:37:10  sterling
 *		Renamed CalcDefaultSize to CalcDefaultSize_.
 *
 *		Revision 1.15  1992/04/20  14:26:16  lewis
 *		Added typedef char* caddr_t; in #if qSnake && _POSIX_SOURCE since xutils.h the distribute not posix complaint.
 *
 *		Revision 1.14  92/04/02  13:07:34  13:07:34  lewis (Lewis Pringle)
 *		Instead of manually re-#defining symbols undefs in Intrinsic.h, we just re-include osrenamepre/post after that
 *		file. This is better because as we add more renames, its painfull having to update all these other places. Hopefully
 *		the entire hack can go away at some point.
 *		
 *		Revision 1.11  1992/03/09  23:46:09  lewis
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *
 *		Revision 1.10  1992/03/02  22:12:53  lewis
 *		Fix mac includes.
 *
 *		Revision 1.9  1992/02/21  20:18:02  lewis
 *		Got rid of qMPW_SymbolTableOverflowProblem workaround. Also, used String::eNoDelete hack.
 *
 *		Revision 1.8  1992/02/14  03:26:35  lewis
 *		Use GetOwner () instead of (now defunct) fNativeOwner.
 *
 *		Revision 1.6  1992/02/06  21:22:40  lewis
 *		Added workaround for qMPW_SymbolTableOverflowProblem bug.
 *
 *		Revision 1.5  1992/02/04  17:08:45  lewis
 *		Override IsNativeItem () in SeperatorMenuItem_MacUI_Native to say that we are a native item.
 *
 *		Revision 1.4  1992/02/04  05:04:36  lewis
 *		Added destructor ~SeperatorMenuItem_MotifUI_Native () to assert we've been unrealized. And got rid of
 *		unneeded includes.
 *
 *		Revision 1.3  1992/02/04  02:32:17  lewis
 *		Added SeperatorMenuItem class implementation.
 *
 *		Revision 1.2  1992/02/03  23:34:48  lewis
 *		Added abstract base classes, and changed to _Native for real ones, and added real class
 *		SeperatorMenuItem instead of #define or typedef.
 *
 *		Revision 1.1  1992/02/03  22:30:35  lewis
 *		Initial revision
 *
 */


#include	<string.h>

#include	"OSRenamePre.hh"
#if		qMacToolkit
#include	<Menus.h>
#elif	qXtToolkit
#if		qSnake && _POSIX_SOURCE
typedef	char*	caddr_t;		// work around hp POSIX headers bug
#endif
#include	<X11/Intrinsic.h>
#include	"OSRenamePost.hh"		// they undef a bunch of things we define
#include	"OSRenamePre.hh"		// so blast back!!!
#include	<Xm/SeparatoG.h>
#endif	/*GDI*/
#include	"OSRenamePost.hh"

#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"Shape.hh"

#include	"Menu.hh"

#include	"SeparatorMenuItem.hh"











/*
 ********************************************************************************
 ************************** AbstractSeparatorMenuItem ***************************
 ********************************************************************************
 */
AbstractSeparatorMenuItem::AbstractSeparatorMenuItem ():
	MenuItem (CommandHandler::eNoCommand)
{
	SetDefaultName (String (String::eReadOnly, "-"));
	SetName (String (String::eReadOnly, "-"));
}

void	AbstractSeparatorMenuItem::SetEnabled_ (Boolean enabled, Panel::UpdateMode updateMode)
{
	Assert (not enabled);
	MenuItem::SetEnabled_ (not kEnabled, updateMode);
}






/*
 ********************************************************************************
 ************************** SeparatorMenuItem_MacUI ****************************
 ********************************************************************************
 */
SeparatorMenuItem_MacUI::SeparatorMenuItem_MacUI ():
	AbstractSeparatorMenuItem ()
{
}





/*
 ********************************************************************************
 ************************** SeparatorMenuItem_MotifUI **************************
 ********************************************************************************
 */
SeparatorMenuItem_MotifUI::SeparatorMenuItem_MotifUI ()
{
}







#if		qMacToolkit

/*
 ********************************************************************************
 ************************ SeparatorMenuItem_MacUI_Native ***********************
 ********************************************************************************
 */
SeparatorMenuItem_MacUI_Native::SeparatorMenuItem_MacUI_Native ():
	SeparatorMenuItem_MacUI ()
{
}
		
Point	SeparatorMenuItem_MacUI_Native::CalcDefaultSize_ (const Point& defaultSize) const
{
	return (Point (1, defaultSize.GetH ()));
}

void	SeparatorMenuItem_MacUI_Native::Draw (const Region& /*update*/)
{
	Line	line = Line (kZeroPoint, Point (0, GetSize ().GetH ()));
	OutLine (line);	// need to draw dotted line instead -- how??
}

void	SeparatorMenuItem_MacUI_Native::UpdateOSRep_ ()
{
	RequireNotNil (GetOwner ());
	AbstractSeparatorMenuItem::UpdateOSRep_ ();
	osStr255	tmpStr;
	tmpStr[0] = 1;
	tmpStr[1] = '-';
	::SetItem (GetOwner ()->GetOSRepresentation (), short (GetIndex ()), tmpStr);
}

Boolean	SeparatorMenuItem_MacUI_Native::IsNativeItem () const
{
	return (True);
}

#elif	qXmToolkit


/*
 ********************************************************************************
 *********************** SeparatorMenuItem_MotifUI_Native **********************
 ********************************************************************************
 */
SeparatorMenuItem_MotifUI_Native::SeparatorMenuItem_MotifUI_Native ():
	   SeparatorMenuItem_MotifUI (),
	   fWidget (Nil)
{
}

SeparatorMenuItem_MotifUI_Native::~SeparatorMenuItem_MotifUI_Native ()
{
	Require (fWidget == Nil);
}

void	SeparatorMenuItem_MotifUI_Native::Realize (osWidget* parent)
{
	RequireNotNil (parent);
	Require (fWidget == Nil);

	fWidget = ::XmCreateSeparatorGadget (parent, "", Nil, 0);
	::XtManageChild (fWidget);
}

void	SeparatorMenuItem_MotifUI_Native::UnRealize ()
{
	if (fWidget != Nil) {
		::XtDestroyWidget (fWidget);
		fWidget = Nil;
	}
}

osWidget*	SeparatorMenuItem_MotifUI_Native::GetWidget () const
{
	return (fWidget);
}

#endif		/*Toolkit*/







/*
 ********************************************************************************
 ******************************** SeparatorMenuItem *****************************
 ********************************************************************************
 */
SeparatorMenuItem::SeparatorMenuItem ():
#if		qMacToolkit
	SeparatorMenuItem_MacUI_Native ()
#elif	qXmToolkit
	SeparatorMenuItem_MotifUI_Native ()
#endif
{
}





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

