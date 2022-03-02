/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__FocusView__
#define	__FocusView__

/*
 * $Header: /fuji/lewis/RCS/FocusView.hh,v 1.1 1992/06/20 17:33:49 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: FocusView.hh,v $
 *		Revision 1.1  1992/06/20  17:33:49  lewis
 *		Initial revision
 *
 *		Revision 1.3  1992/03/26  15:16:32  lewis
 *		Changed EffectiveFocusedChanged () to EffectiveFocusChanged(), and got rid of
 *		oldFocused first argument.
 *
 *		Revision 1.2  1992/03/26  09:24:26  lewis
 *		Got rid of oldLive first argument to EffectiveLiveChanged () method.
 *
 *		Revision 1.1  1992/03/13  16:00:55  lewis
 *		Initial revision
 *
 *
 *
 */

#include	"FocusItem.hh"
#include	"EnableView.hh"




class	FocusView : public FocusItem, public EnableView {
	protected:
		FocusView ();

	public:
		override	Boolean	GetLive () const;			// only overridden to disambiguate with EnableView version and FocusItemVersion
		override	Boolean	GetEffectiveLive () const;  // only overridden to disambiguate with EnableView version and FocusItemVersion
				
		// overrite trackpress to say grab focus first (right now, only on mac??)...
		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);

	protected:
		override	void	EffectiveLiveChanged (Boolean newLive, UpdateMode updateMode); 		// only overridden to disambiguate
		  																						// with EnableView version and FocusItemVersion
		override	void	EffectiveFocusChanged (Boolean newFocused, UpdateMode updateMode);
		
		virtual	Adornment*	BuildFocusAdornment_ ();

	private:
		Adornment*			fFocusAdornment;
};






/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__FocusItem__*/
