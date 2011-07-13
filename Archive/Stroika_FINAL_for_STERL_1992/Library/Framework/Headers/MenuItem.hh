/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__MenuItem__
#define	__MenuItem__

/*
 * $Header: /fuji/lewis/RCS/MenuItem.hh,v 1.7 1992/09/01 15:42:04 sterling Exp $
 *
 * Description:
 *
 *		MenuItem	-- Button with associated CommandNumber, accelerator, mneumonic
 *
 *
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: MenuItem.hh,v $
 *		Revision 1.7  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.6  1992/07/21  20:55:17  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.5  1992/07/21  19:59:01  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.4  1992/07/02  04:31:51  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *		Revision 1.3  1992/07/01  04:05:14  lewis
 *		Renamed Strings.hh String.hh
 *
 *		Revision 1.2  1992/06/25  05:53:13  sterling
 *		MenuItem is a Toggle (new class) instead of Button.
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.34  1992/04/07  10:15:30  lewis
 *		Wrapped things in #if qRealTemplatesAvailable.
 *
 *		Revision 1.32  1992/03/09  23:52:59  lewis
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *
 *		Revision 1.31  1992/02/19  17:15:49  lewis
 *		Use class Accelerator as field not string...
 *
 *		Revision 1.30  1992/02/14  23:24:51  lewis
 *		Move accelerator definition to a seperate file.
 *
 *		Revision 1.29  1992/02/14  02:57:58  lewis
 *		Got rid of fNativeOwner - no longer needed - use GetOwner ().
 *
 *		Revision 1.28  1992/02/12  06:40:06  lewis
 *		Chaned from using AbstractMenu to Menu, and other cleanups.
 *
 *		Revision 1.27  1992/02/11  00:56:38  lewis
 *		Move towards just 1 kind of menu.
 *
 *		Revision 1.26  1992/02/04  17:01:55  lewis
 *		Provide IsNativeItem method to MenuItem on mactoolkit so we can tell in AbstractMenu if each item is native.
 *		This will be used in an optimization there to use the native mdef automatically.
 *
 *		Revision 1.25  1992/02/04  04:30:14  lewis
 *		General cleanups.
 *
 *		Revision 1.24  1992/02/03  22:27:12  lewis
 *		Broke up so various kinds of menuitems go into seperate files.
 *
 *
 *
 *
 */

#if		!qRealTemplatesAvailable
#include	"Sequence.hh"
#endif
#include	"String.hh"

#include	"Accelerator.hh"
#include	"Changeable.hh"
#include	"Command.hh"
#include	"MenuCommandHandler.hh"
#include	"Toggle.hh"






class	Menu;


#if		qMotifUI
struct	osWidget;
#endif 	/*GUI*/



class	MenuItem : public CommandSelection, public Toggle, public Changeable {
	protected:
		MenuItem (CommandNumber commandNumber);

	public:
		~MenuItem ();

	public:
#if		qSupportMneumonics		
		nonvirtual	String			GetMneumonic () const;
		nonvirtual	void			SetMneumonic (const String& mneumonic);
#endif		

		nonvirtual	String			GetDefaultName () const;
		nonvirtual	void			SetDefaultName (const String& name);
		
		nonvirtual	Accelerator		GetAccelerator () const;	// no set cuz implied by the accelerator mapping (see MenuOwner)
		nonvirtual	Menu*			GetOwner () const;			// no set cuz implied by owning menu

		nonvirtual	void	Reset ();

		nonvirtual	void	UpdateOSRep ();	// should be only called by Menu who owns menuItem

	protected:
		override	void	SetOn_ (Boolean on, UpdateMode updateMode);
		override	void	SetCommandNumber_ (CommandNumber commandNumber);		
		override	void	SetName_ (const String& name);
		override	void	SetEnabled_ (Boolean enabled, UpdateMode updateMode);
		
		override	CollectionSize	GetIndex_ () const;
		
		virtual		void	SetDefaultName_ (const String& name);
		virtual		void	UpdateOSRep_ ();
	
#if		qSupportMneumonics		
		virtual	void	SetMneumonic_ (const String& mneumonic);
#endif		

		virtual	void	Reset_ ();		

	private:
		String			fDefaultName;

#if 	qSupportMneumonics
		String			fMneumonic;
#endif

		Menu*			fOwner;	// maintained by the Menu
		CollectionSize	fIndex;	// maintained by the Menu		

		Boolean			fOldOn;
		Boolean			fOldEnabled;
		String			fOldName;
		Accelerator		fCachedAccelerator;
		Boolean			fOSInitialized;

#if		qXmToolkit
	public:		// just make these guys public... so can be accessed by Menu - is there a better way???
				// leave them private/protected and do all realization/unrealize in Menu, then we are already freind...
		override 	void		Realize (osWidget* parent)	{ View::Realize (parent); }
		override	void		UnRealize ()				{ View::UnRealize (); }
		override	osWidget*	GetWidget () const			{ return View::GetWidget (); }
#endif


#if		qMacToolkit
		virtual		Boolean	IsNativeItem () const;			// hack - override and say true of native items, and inherited returns
															// false - so Menu can know it can use default menu proc...
#endif
	friend	class	Menu;
};


#if		!qRealTemplatesAvailable
	typedef	class	MenuItem*		MenuItemPtr;
	Declare (Iterator, MenuItemPtr);
	Declare (Collection, MenuItemPtr);
	Declare (AbSequence, MenuItemPtr);
	Declare (Array, MenuItemPtr);
	Declare (Sequence_Array, MenuItemPtr);
	Declare (Sequence, MenuItemPtr);
#endif



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__MenuItem__*/


