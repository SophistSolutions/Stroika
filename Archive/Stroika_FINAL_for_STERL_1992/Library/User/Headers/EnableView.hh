/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__EnableView__
#define	__EnableView__

/*
 * $Header: /fuji/lewis/RCS/EnableView.hh,v 1.1 1992/06/20 17:33:49 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: EnableView.hh,v $
 *		Revision 1.1  1992/06/20  17:33:49  lewis
 *		Initial revision
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

#include	"EnableItem.hh"
#include	"View.hh"



#if 	qCanFreelyUseVirtualBaseClasses
class	EnableView : public virtual EnableItem, public virtual View {
#else
class	EnableView : public EnableItem, public View {
#endif

	protected:
		EnableView (Boolean enabled = kEnabled);
	
	public:
		override	Boolean	GetLive () const;
		
	protected:
		override	void	EffectiveLiveChanged (Boolean newLive, UpdateMode updateMode);

		override	Boolean	GetEnabled_ () const;
		override	void	SetEnabled_ (Boolean enabled, UpdateMode updateMode);
		
	private:
		Boolean		fEnabled;
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


#endif	/*__EnableView__*/

