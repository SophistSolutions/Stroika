/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__EnableItem__
#define	__EnableItem__

/*
 * $Header: /fuji/lewis/RCS/EnableItem.hh,v 1.4 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 *		An EnableItem is a modifier of LiveItems that has an associated enabled state. If 
 *		enabled it is live, if disabled it is not live. A convenience class, EnableView, mixes 
 *		EnableItem into a View. EnableView forms the base class of many widgets either directly 
 *		(on the Macintosh) or indirectly (through FocusView under Windows or Motif)
 *		
 *		There is currently no EnableOwner (as with FocusOwner) thought this may prove
 *		a useful abstraction.
 *
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: EnableItem.hh,v $
 *		Revision 1.4  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/07/02  03:50:06  lewis
 *		Properly scoped name UpdateMode.
 *
 *		Revision 1.6  1992/03/13  16:09:25  lewis
 *		Made EnableItem ctor/dtor inline, and moved EnableView to User layer.
 *
 *		Revision 1.5  1992/02/15  04:50:37  sterling
 *		used EffectiveLiveChanged
 *
 *
 *
 */

#include	"Config-Framework.hh"

#include	"View.hh"




class	EnableItem {
	public:
		static	const	Boolean	kEnabled;

		virtual ~EnableItem ();

	protected:
		EnableItem ();

	public:
		nonvirtual	Boolean	GetEnabled () const;
		nonvirtual	void	SetEnabled (Boolean enabled, Panel::UpdateMode updateMode = Panel::eDelayedUpdate);
		
	protected:
		virtual	Boolean	GetEnabled_ () const = Nil;
		virtual	void	SetEnabled_ (Boolean enabled, Panel::UpdateMode updateMode) = Nil;
};




/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */

inline	EnableItem::EnableItem () 	{}
inline	EnableItem::~EnableItem ()	{}
inline	Boolean	EnableItem::GetEnabled () const		{	return (GetEnabled_ ());		}



// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***


#endif	/*__EnableItem__*/

