/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Toggle__
#define	__Toggle__

/*
 * $Header: /fuji/lewis/RCS/Toggle.hh,v 1.1 1992/06/25 07:25:05 sterling Exp $
 *
 * Description:
 * 
 * A Toggle is a Button with two states: On and Off. It has a SetOn method, which
 * will automatically call its controller's ButtonPressed method. Calling SetOn_
 * directly, rather than through the public interface, allows subclasses to change
 * their state without informing their controller. Toggles override the Track method
 * to toggle their state (i.e. SetOn (not GetOn ()) as a result of a successful track.
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 *
 * Changes:
 *	$Log: Toggle.hh,v $
 *		Revision 1.1  1992/06/25  07:25:05  sterling
 *		Initial revision
 *
 *
 */

#include	"Button.hh"

class	Toggle : public Button {
	public:
		static	const	Boolean	kOn;

	protected:
		Toggle (ButtonController* controller);

	public:
		nonvirtual	Boolean	GetOn () const;
		nonvirtual	void	SetOn (Boolean on, UpdateMode updateMode = eDelayedUpdate);

	protected:
		override	Boolean	Track (Tracker::TrackPhase phase, Boolean mouseInButton);

		virtual	void	SetOn_ (Boolean on, UpdateMode updateMode);
		
	private:
		Boolean		fOn;
};




/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */
inline	Boolean	Toggle::GetOn () const								
{
	return (fOn);				
}


#endif	/* __Toggle__ */
