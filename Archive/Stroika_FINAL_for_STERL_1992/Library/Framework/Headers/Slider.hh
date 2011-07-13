/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Slider__
#define	__Slider__

/*
 * $Header: /fuji/lewis/RCS/Slider.hh,v 1.7 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: Slider.hh,v $
 *		Revision 1.7  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.6  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/07/21  20:55:17  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.4  1992/07/02  04:38:30  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *		Revision 1.2  1992/06/25  06:53:19  sterling
 *		SliderController no longer overrides Get/SetController.
 *		Redefines SliderChanging and SliderChanged - both to take SliderBase instead of Slider&,
 *		and SliderChaing also takes TrackPhase.
 *		SliderBase has SliderController field no instead of forcing subclasses to do it. Adds
 *		Change and Changing methods to call SliderController versions
 *		(LGP checked in for SSW so not sure I got it right).
 *		Also, Killed StepSize/PageSize from Slider. Reanmed SetMinMax to SetBounds. Slider
 *		No longer inherits from AbstractBordered.
 *
 *		Revision 1.11  1992/04/30  13:07:37  sterling
 *		use AbstractBordered
 *
 *		Revision 1.10  92/03/26  09:38:44  09:38:44  lewis (Lewis Pringle)
 *		Got rid of oldLive arg to EffecitveLiveChanged.
 *		
 *		Revision 1.9  1992/03/13  16:01:20  lewis
 *		Instead of inheriting from EnableView, inherit directly from EnableItem and View.
 *
 *		Revision 1.8  1992/03/05  20:07:26  sterling
 *		used Bordered
 *
 *		Revision 1.7  1992/02/21  19:58:57  lewis
 *		Got rid of qMPW_SymbolTableOverflowProblem workaround.
 *
 *		Revision 1.6  1992/02/03  21:50:17  sterling
 *		massive inteface changes using virtual protected methods and Invariants
 *
 *
 */

#include	"Sequence.hh"

#include	"EnableItem.hh"
#include	"Tracker.hh"
#include	"View.hh"


#if		!qRealTemplatesAvailable
	class	Slider;
	typedef	Slider*	SliderPtr;
	Declare (Iterator, SliderPtr);
	Declare (Collection, SliderPtr);
	Declare (AbSequence, SliderPtr);
	Declare (Array, SliderPtr);
	Declare (Sequence_Array, SliderPtr);
	Declare (Sequence, SliderPtr);
#endif


class	SliderController;
class	SliderBase {
	protected:
		SliderBase ();
	
	public:
		virtual	~SliderBase ();
		
		nonvirtual	SliderController*	GetSliderController () const;
		nonvirtual	void				SetSliderController (SliderController* sliderController);

		// these methods dispatch to SliderController (if any)
		nonvirtual	Boolean	Changing (Tracker::TrackPhase phase, Real oldValue, Real newValue);
		nonvirtual	void	Changed (Real oldValue, Real newValue);

	private:
		SliderController*	fController;
};

class	SliderController : public SliderBase {
	public:
		SliderController ();

		virtual	Boolean	SliderChanging (SliderBase& theSlider, Tracker::TrackPhase phase, Real oldValue, Real newValue);	// called while tracking
		virtual	void	SliderChanged (SliderBase& theSlider, Real oldValue, Real newValue);	// called when done, or changed from outside (eg setvalue())
};


class	Slider  : public SliderBase, public EnableItem, public View {
	protected:
		Slider (SliderController* sliderController);

	public:
		nonvirtual	Real	GetMin () const;
		nonvirtual	void	SetMin (Real newMin, UpdateMode update = eDelayedUpdate);
		nonvirtual	Real	GetMax () const;
		nonvirtual	void	SetMax (Real newMax, UpdateMode update = eDelayedUpdate);
		nonvirtual	Real	GetValue () const;
		nonvirtual	void	SetValue (Real newValue, UpdateMode update = eDelayedUpdate);
		nonvirtual	void	SetBounds (Real newMin, Real newMax, Real newValue, UpdateMode update = eDelayedUpdate);
		
		nonvirtual	void	Invariant () const;
		
	protected:
		virtual	Real	GetMin_ () const														= Nil;
		virtual	Real	GetMax_ () const														= Nil;
		virtual	void	SetBounds_ (Real newMin, Real newMax, Real newValue, UpdateMode update)	= Nil;
		virtual	Real	GetValue_ () const														= Nil;
		virtual	void	SetValue_ (Real newValue, UpdateMode update)							= Nil;

	public:
		override	Boolean	GetLive () const;
		
	protected:
		override	void	EffectiveLiveChanged (Boolean newLive, UpdateMode updateMode);

		override	Boolean	GetEnabled_ () const;
		override	void	SetEnabled_ (Boolean enabled, UpdateMode updateMode);

#if		qDebug
		virtual	void	Invariant_ () const;
#endif

	private:
		Boolean				fEnabled;
};




/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */
inline	void	Slider::Invariant () const
{
#if		qDebug
	Invariant_ ();
#endif
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Slider__*/

