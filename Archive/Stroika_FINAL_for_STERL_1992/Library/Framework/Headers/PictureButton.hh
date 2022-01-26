/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__PictureButton__
#define	__PictureButton__

/*
 * $Header: /fuji/lewis/RCS/PictureButton.hh,v 1.6 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *		Do class instead of typedef for PictureButton.
 *
 * Notes:
 *
 * Changes:
 *	$Log: PictureButton.hh,v $
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
 *		Revision 1.2  1992/06/25  06:07:56  sterling
 *		PictureButton now a Toggle (new class) instead of Button. Perhaps that makes it
 *		a bad name?
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.10  1992/03/26  09:37:21  lewis
 *		Got rid of EffectiveFontChanged override since default action now is no-op.
 *
 *		Revision 1.8  1992/03/05  20:07:26  sterling
 *		used Bordered
 *
 *		Revision 1.7  1992/02/21  19:47:02  lewis
 *		Got rid of qMPW_SymbolTableOverflowProblem workaround. And noted that we
 *		must do class PictureButton instead of typdef. Could use general
 *		cleanups here.
 *
 *
 */

#include	"Picture.hh"

#include	"Toggle.hh"





class	AbstractPictureButton : public Toggle {
	protected:
		AbstractPictureButton (ButtonController* controller);

	public:
		virtual		Picture	GetPicture () const															=	Nil;
		virtual		void	SetPicture (const Picture& picture, UpdateMode updateMode = eDelayedUpdate)	=	Nil;
};





class	AbstractPictureButton_MacUI : public AbstractPictureButton {
	protected:
		AbstractPictureButton_MacUI (ButtonController* controller);
};





class	PictureButton_MacUI_Portable : public AbstractPictureButton_MacUI {
	public:
		PictureButton_MacUI_Portable (ButtonController* controller, const Picture& picture);
		PictureButton_MacUI_Portable (ButtonController* controller);

		override	void	Draw (const Region& update);
	
		override	Picture	GetPicture () const;
		override	void	SetPicture (const Picture& picture, UpdateMode updateMode = eDelayedUpdate);

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	Boolean	Track (Tracker::TrackPhase phase, Boolean mouseInButton);
		override	void	SetOn_ (Boolean on, UpdateMode updateMode);

	private:
		Picture	fPicture;
};







class	AbstractPictureButton_MotifUI : public AbstractPictureButton {
	protected:
		AbstractPictureButton_MotifUI (ButtonController* controller);
};




class	AbstractPictureButton_WinUI : public AbstractPictureButton {
	protected:
		AbstractPictureButton_WinUI (ButtonController* controller);
};





class	PictureButton_WinUI_Portable : public AbstractPictureButton_WinUI {
	public:
		PictureButton_WinUI_Portable (ButtonController* controller, const Picture& picture);
		PictureButton_WinUI_Portable (ButtonController* controller);

		override	void	Draw (const Region& update);
	
		override	Picture	GetPicture () const;
		override	void	SetPicture (const Picture& picture, UpdateMode updateMode = eDelayedUpdate);

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	Boolean	Track (Tracker::TrackPhase phase, Boolean mouseInButton);
		override	void	SetOn_ (Boolean on, UpdateMode updateMode);

	private:
		Picture	fPicture;
};


class	PictureButton_MacUI : public PictureButton_MacUI_Portable
{
	public:
		PictureButton_MacUI (ButtonController* controller, const Picture& picture);
		PictureButton_MacUI (ButtonController* controller);

#if		qSoleInlineVirtualsNotStripped
		virtual ~PictureButton_MacUI ();
#endif
};

class	PictureButton_MotifUI : public PictureButton_WinUI_Portable
{
	public:
		PictureButton_MotifUI (ButtonController* controller, const Picture& picture);
		PictureButton_MotifUI (ButtonController* controller);

#if		qSoleInlineVirtualsNotStripped
		virtual ~PictureButton_MotifUI ();
#endif
};

class	PictureButton_WinUI : public PictureButton_WinUI_Portable
{
	public:
		PictureButton_WinUI (ButtonController* controller, const Picture& picture);
		PictureButton_WinUI (ButtonController* controller);

#if		qSoleInlineVirtualsNotStripped
		virtual ~PictureButton_WinUI ();
#endif
};


class	PictureButton : public
#if		qMacUI
	PictureButton_MacUI
#elif	qMotifUI
	PictureButton_MotifUI
#elif	qWinUI
	PictureButton_WinUI
#endif
{
	public:
		PictureButton (ButtonController* controller, const Picture& picture);
		PictureButton (ButtonController* controller);

#if		qSoleInlineVirtualsNotStripped
		virtual ~PictureButton ();
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


#endif	/*__PictureButton__*/
