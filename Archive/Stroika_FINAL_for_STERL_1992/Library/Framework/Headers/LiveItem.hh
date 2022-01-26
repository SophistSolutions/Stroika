/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__LiveItem__
#define	__LiveItem__

/*
 * $Header: /fuji/lewis/RCS/LiveItem.hh,v 1.4 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 *		A LiveItem is a modifier of MouseHandler that has an associated live state. Only live MouseHandler should respond
 *		to track presses or track moves. It should be mixed into mousehandlers that have a continuous screen presence 
 *		(i.e. Views, not Trackers). It can also be mixed into containers of MouseHandlers, like FocusOwner.
 *
 * TODO:
 *
 * Notes:
 *
 * Changes:
 *	$Log: LiveItem.hh,v $
 *		Revision 1.4  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.7  1992/03/26  09:36:15  lewis
 *		Simplified args to EffectiveLive/Visibility/Font changed - no old value, just the new value passed.
 *
 *		Revision 1.6  1992/02/15  04:50:37  sterling
 *		added EffectiveLiveChanged
 *
 *
 *
 *
 */

#include	"Config-Foundation.hh"
#include	"MouseHandler.hh"

class	LiveItem {
	public:
		static	const	Boolean	kLive;

	protected:
		LiveItem ();

	private:
		LiveItem (const LiveItem&);						// unimplemented since never called (prevent copy)
		const LiveItem& operator= (const LiveItem&);	// unimplemented since never called (prevent copy)

	public:
		virtual	~LiveItem ();

		virtual	Boolean	GetLive () const = Nil;
		virtual	Boolean	GetEffectiveLive () const;
	
	protected:
		virtual	void	EffectiveLiveChanged (Boolean newLive, Panel::UpdateMode updateMode);
};
 

// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__LiveItem__*/

