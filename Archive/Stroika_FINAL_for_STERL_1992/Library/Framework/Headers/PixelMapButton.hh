/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__PixelMapButton__
#define	__PixelMapButton__

/*
 * $Header: /fuji/lewis/RCS/PixelMapButton.hh,v 1.6 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *			(1)		Do native implementation for motif.
 *
 * Notes:
 *
 * Changes:
 *	$Log: PixelMapButton.hh,v $
 *		Revision 1.6  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/21  19:59:01  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.3  1992/07/16  16:25:55  sterling
 *		renamed GUI to UI, made Mac_UI and Motif_UI variants
 *
 *		Revision 1.2  1992/06/25  06:12:20  sterling
 *		Inherit from Toggle instead of Button, and explicitly inherit from FocusItem for
 *		motif GUI version. Also renamed CalcDefaultSize to CalcDefaultSize_.
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.18  1992/03/26  09:37:21  lewis
 *		Got rid of EffectiveFontChanged override since default action now is no-op.
 *
 *		Revision 1.16  1992/03/05  20:07:26  sterling
 *		used Bordered
 *
 *		Revision 1.15  1992/02/21  19:49:03  lewis
 *		Got rid of qMPW_SymbolTableOverflowProblem workaround.
 *
 *		Revision 1.14  1992/02/15  04:50:37  sterling
 *		used EffectiveLiveChanged
 *
 *		Revision 1.13  1992/02/03  19:42:53  sterling
 *		got rid of typedef, switched constru tor args
 *
 *
 */

#include	"Toggle.hh"
#include	"FocusItem.hh"





class	AbstractPixelMapButton : public Toggle {
	protected:
		AbstractPixelMapButton (ButtonController* controller);

	public:	
		~AbstractPixelMapButton ();
		
		virtual		const PixelMap&	GetPixelMap () const																=	Nil;
		virtual		void			SetPixelMap (const PixelMap& pixelMap, UpdateMode updateMode = eDelayedUpdate)		=	Nil;
	
	protected:
		static	const	PixelMap	kDefaultPixelMap;
};



class	AbstractPixelMapButton_MacUI : public AbstractPixelMapButton {
	protected:
		AbstractPixelMapButton_MacUI (ButtonController* controller);

	public:
		~AbstractPixelMapButton_MacUI ();
};


class	PixelMapButton_MacUI_Portable : public AbstractPixelMapButton_MacUI {
	public:
		PixelMapButton_MacUI_Portable (const PixelMap& pixelMap, ButtonController* controller = Nil);
		PixelMapButton_MacUI_Portable (ButtonController* controller = Nil);
		~PixelMapButton_MacUI_Portable ();

		override	void	Draw (const Region& update);

		override	const PixelMap&	GetPixelMap () const;
		override	void			SetPixelMap (const PixelMap& pixelMap, UpdateMode updateMode = eDelayedUpdate);
		
	protected:
		override	Boolean	Track (Tracker::TrackPhase phase, Boolean mouseInButton);
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	SetOn_ (Boolean on, UpdateMode updateMode);

	private:
		PixelMap	fPixelMap;
};


class	AbstractPixelMapButton_MotifUI : public AbstractPixelMapButton, public FocusItem {
	protected:
		AbstractPixelMapButton_MotifUI (ButtonController* controller);

	public:
		~AbstractPixelMapButton_MotifUI ();
		
	public:
		override	Boolean	HandleKeyStroke (const KeyStroke& keyStroke);
		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);

	protected:
		override	void	EffectiveLiveChanged (Boolean newLive, UpdateMode updateMode);
		override	Boolean	GetEffectiveLive () const;		// override and call view version to disambiguate base class versions
		override	Boolean	GetLive () const;
};


class	PixelMapButton_MotifUI_Portable : public AbstractPixelMapButton_MotifUI {
	public:
		PixelMapButton_MotifUI_Portable (const PixelMap& pixelMap, ButtonController* controller = Nil);
		PixelMapButton_MotifUI_Portable (ButtonController* controller = Nil);
		~PixelMapButton_MotifUI_Portable ();

		override	void	Draw (const Region& update);

		override	const PixelMap&	GetPixelMap () const;
		override	void			SetPixelMap (const PixelMap& pixelMap, UpdateMode updateMode = eDelayedUpdate);
		
	protected:
		override	Boolean	Track (Tracker::TrackPhase phase, Boolean mouseInButton);
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	SetOn_ (Boolean on, UpdateMode update);

	private:
		PixelMap	fPixelMap;
};




class	AbstractPixelMapButton_WinUI : public AbstractPixelMapButton, public FocusItem {
	protected:
		AbstractPixelMapButton_WinUI (ButtonController* controller);

	public:
		~AbstractPixelMapButton_WinUI ();
		
	public:
		override	Boolean	HandleKeyStroke (const KeyStroke& keyStroke);
		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);

	protected:
		override	void	EffectiveLiveChanged (Boolean newLive, UpdateMode updateMode);
		override	Boolean	GetEffectiveLive () const;		// override and call view version to disambiguate base class versions
		override	Boolean	GetLive () const;
};


class	PixelMapButton_WinUI_Portable : public AbstractPixelMapButton_WinUI {
	public:
		PixelMapButton_WinUI_Portable (const PixelMap& pixelMap, ButtonController* controller = Nil);
		PixelMapButton_WinUI_Portable (ButtonController* controller = Nil);
		~PixelMapButton_WinUI_Portable ();

		override	void	Draw (const Region& update);

		override	const PixelMap&	GetPixelMap () const;
		override	void			SetPixelMap (const PixelMap& pixelMap, UpdateMode updateMode = eDelayedUpdate);
		
	protected:
		override	Boolean	Track (Tracker::TrackPhase phase, Boolean mouseInButton);
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	SetOn_ (Boolean on, UpdateMode update);

	private:
		PixelMap	fPixelMap;
};



class	PixelMapButton_MacUI : public PixelMapButton_MacUI_Portable 
{
	public:
		PixelMapButton_MacUI (const PixelMap& pixelMap, ButtonController* controller);
		PixelMapButton_MacUI (ButtonController* controller);

#if		qSoleInlineVirtualsNotStripped
		virtual ~PixelMapButton_MacUI ();
#endif
};

class	PixelMapButton_MotifUI : public PixelMapButton_MotifUI_Portable 
{
	public:
		PixelMapButton_MotifUI (const PixelMap& pixelMap, ButtonController* controller);
		PixelMapButton_MotifUI (ButtonController* controller);

#if		qSoleInlineVirtualsNotStripped
		virtual ~PixelMapButton_MotifUI ();
#endif
};

class	PixelMapButton_WinUI : public PixelMapButton_WinUI_Portable 
{
	public:
		PixelMapButton_WinUI (const PixelMap& pixelMap, ButtonController* controller);
		PixelMapButton_WinUI (ButtonController* controller);

#if		qSoleInlineVirtualsNotStripped
		virtual ~PixelMapButton_WinUI ();
#endif
};


class	PixelMapButton : public
#if		qMacUI
	PixelMapButton_MacUI
#elif		qMotifUI
	PixelMapButton_MotifUI
#elif		qWinUI
	PixelMapButton_WinUI
#endif
{
	public:
		PixelMapButton (const PixelMap& pixelMap, ButtonController* controller = Nil);
		PixelMapButton (ButtonController* controller = Nil);

#if		qSoleInlineVirtualsNotStripped
		virtual ~PixelMapButton ();
#endif
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


#endif	/*__PixelMapButton__*/

