/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Button__
#define	__Button__

/*
 * $Header: /fuji/lewis/RCS/Button.hh,v 1.4 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 * A Button is a view designed for user interaction. There ability to interact with users
 * can be disabled, as they are EnableItems. Their visual appearance usually reflects 
 * their disabled status, for example in Macintosh GUI they are traditionally drawn in
 * gray. On GUIs supportings the mouseless interface (e.g Windows and Motif) buttons 
 * are typically FocusItems as well.
 * 
 * Buttons have an associated controller of type ButtonController. Typically programs will subclass
 * the ButtonController, rather than the button itself, to implement application-specific behavior.
 * ButtonController are recursively defined in that ButtonControllers can themselves have ButtonControllers.
 * The ButtonController has two callbacks: ButtonTracking and ButtonPressed. By default a ButtonController
 * propagates a callback on to its own controller.
 * 
 * The ButtonTracking method is called repeatedly while the user tracks within a button. 
 * The method is given the current TrackPhase, as well as a pointer to the button being tracked, 
 * and a boolean value stating whether the mouse is currently within the button. 
 * The ButtonTracking method will always be called once with a TrackPhase of
 * eTrackPress, followed by zero of more calls with phase eTrackMove, followed by one call with
 * phase eTrackRelease. The ButtonTracking method returns a Boolean, stating whether or not
 * the current tracking should be aborted (True == abort tracking). For example, a pallet of
 * buttons, in which a click should immediately turn a button on (and turn off some other button),
 * like the tools pallet in MacPaint, can be implemented by overriding the ButtonTracking method
 * to abort during the eTrackPress phase. It is then up to the implementer of the pallet to manually
 * turn off the old button, and turn on the new button.
 * 
 * The ButtonPressed is called whenever the user has successfully "pressed" the button. 
 * This means that the entire track sequence was called without aborting, 
 * and that the mouse button was released while the cursor was still within the button.
 * 
 * Buttons can have one or more possible states. Pushbuttons, for example, have only one state.
 * Many buttons, such as CheckBoxes and PixelMapButtons, have two states: on or off. These buttons
 * typically subclass from Toggle, which is itself a subclass of button. Other buttons have many
 * states, such as OptionMenuButtons, and PickLists. When multi-state buttons undergo a state
 * change, they call their controllers ButtonPressed method. For example, calling the "SetOn" method
 * of a CheckBox will call the ButtonPressed method of its controller exactly as if the user had
 * tracked within the CheckBox. In the rare case where this is not desired behavior there is usually
 * a protected method (such as SetOn_) that will change the state without notifying the controller.
 *
 * Creating a new subclass of Button largely consists of overriding two methods: Draw and Track.
 * Track is a special method of Buttons that is called during button tracking. It will be called
 * once with a TrackPhase of eTrackPress, zero of more times with a TrackPhase of eTrackMove, and
 * once with a TrackPhase of eTrackRelease (assuming the button's controller does not abort the
 * process). The Track method will be called during eTrackMove if and only if the mouse is moved
 * either within our outside of the button (i.e. only if the mouseInButton parameter has changed
 * value).
 * In the Track method, the button should draw itself, taking into account the TrackPhase
 * and whether or not the mouse is within the control. Some special behavior should be done if
 * phase eTrackRelease is reached with the mouse within the button. For example, Toggles call
 * their SetOn method, while PushButtons call their controllers ButtonPressed method directly.
 *
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: Button.hh,v $
 *		Revision 1.4  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/06/25  05:06:11  sterling
 *		Have button only inherit from AbstractButton, View and EnableItem (get rid of optional
 *		FocusItem - mixin in subclasses where appropriate - this was too deep a level). Also, View
 *		already had Bordered mixed in.
 *		Lots of minor changes designed to increase clarity about when Pressed/Tracking methods are called,
 *		and SIGNICICANT NEW DOCUMENTATION about these details. (LGP doing checkin - I didnt read it
 *		carefully yet but looked good).
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.16  1992/03/26  09:36:15  lewis
 *		Simplified args to EffectiveLive/Visibility/Font changed - no old value, just the new value passed.
 *
 *		Revision 1.15  1992/03/19  16:42:40  lewis
 *		Get rid of Button::EffectiveFocusChanged, and BuildFocusAdornment. Its up to subclasses to decide how to display
 *		focus status.
 *
 *		Revision 1.14  1992/03/13  16:08:10  lewis
 *		Get rid of Button inheriting from EnableView or with qMouselessInterface FocusView. Instead
 *		Inherit directly from EnableItem, and View, and only FocusItem if qMouselessInterface.
 *
 *		Revision 1.13  1992/03/09  23:41:16  lewis
 *		Use new HandleKeyStroke () interface.
 *
 *		Revision 1.12  1992/03/05  20:07:26  sterling
 *		used Bordered
 *
 *		Revision 1.11  1992/02/21  19:35:26  lewis
 *		Got rid of qMPW_SymbolTableOverflowProblem workaround. And got rid
 *		of support for compilers without nested types.
 *
 *
 *
 */

#include	"EnableItem.hh"
#include	"Tracker.hh"
#include	"View.hh"





class	ButtonController;
class	AbstractButton {
	protected:
		AbstractButton ();

	public:
		virtual	~AbstractButton ();

		nonvirtual	ButtonController*	GetController () const;
		nonvirtual	void				SetController (ButtonController* buttonController);

		// these methods dispatch to ButtonController (if any)
		nonvirtual	Boolean	Tracking (Tracker::TrackPhase phase, Boolean mouseInButton);
		nonvirtual	void	Pressed ();
		
	private:
		ButtonController*	fController;
};


class	ButtonController : public AbstractButton {
	protected:
		ButtonController ();

	public:
		virtual	Boolean	ButtonTracking (AbstractButton* button, Tracker::TrackPhase phase, Boolean mouseInButton);
		virtual	void	ButtonPressed (AbstractButton* button);
};


class	Button : public AbstractButton, public View, public EnableItem {
	protected:
		Button (ButtonController* controller);

	public:
		override	Boolean	GetLive () const;
		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);

	protected:
		override	void	EffectiveLiveChanged (Boolean newLive, UpdateMode updateMode);

		override	Boolean	GetEnabled_ () const;
		override	void	SetEnabled_ (Boolean enabled, UpdateMode updateMode);

		virtual		Boolean	Track (Tracker::TrackPhase phase, Boolean mouseInButton);

	private:
		Boolean		fEnabled;
		
		friend	class	ButtonTracker;	// calls Track method
};


class	ButtonTracker : public Tracker {
	public:
		ButtonTracker (Button& button);

	protected:
		override	void		TrackFeedback (TrackPhase phase, const Point& anchor, const Point& previous, const Point& next);
		override	Boolean		TrackResult (TrackPhase phase, const Point& anchor, const Point& previous, const Point& next);
		override	void		TrackConstrain (TrackPhase phase, const Point& anchor, const Point& previous, Point& next);

	private:
		Button&		fButton;
};






/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */
inline	AbstractButton::AbstractButton () :	fController (Nil)		{}
inline	AbstractButton::~AbstractButton ()							{}
inline	ButtonController*	AbstractButton::GetController () const	{	return (fController);	}


inline	ButtonController::ButtonController ()						{}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Button__*/
