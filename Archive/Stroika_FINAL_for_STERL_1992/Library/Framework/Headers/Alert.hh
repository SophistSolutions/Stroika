/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Alert__
#define	__Alert__

/*
 * $Header: /fuji/lewis/RCS/Alert.hh,v 1.3 1992/09/01 15:42:04 sterling Exp $
 *
 * Description:
 *		An Alert is an Application-Modal (and optionally system modal) Dialog that allows
 * for the display of some message, and has a GUI dependent ICON, to indicate the type of message
 * and some buttons that are GUI dependent - typically an OK button, but perhaps also a cancel,
 * and help.
 *
 *		For now, Alert() is a concrete class, but it should probably be abstract, and you should be
 * forced to use NoteAlert, StopAlert, or whatever. We also need to do the GUI-dependent branching...
 *
 *
 * <Describe/redesign StagedAlert - bizare that they need to be done statically - at least expain why???>
 *
 *
 * TODO:
 *		Do the stuff listed above in Description, and cleanup Look of dialogs, and icons etc.
 *		Examine Mac HIN for exact specification of how mac versions should look.
 *
 * Notes:
 *
 * Changes:
 *	$Log: Alert.hh,v $
 *		Revision 1.3  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/06/25  04:37:39  sterling
 *		React to name change CalcDefaultSize to CalcDefaultSize_
 *
 *		Revision 1.9  1992/05/11  21:32:55  lewis
 *		Comment...
 *
 *		Revision 1.8  92/03/24  02:28:50  02:28:50  lewis (Lewis Pringle)
 *		Made CalcDefaultSize () const.
 *		
 *		Revision 1.7  1992/01/31  16:21:24  sterling
 *		got of backwards compativility
 *
 *		Revision 1.6  1992/01/29  04:37:10  sterling
 *		eliminated fMeessage variable, no longer used
 *
 *
 *		Revision 1.1  1992/01/05  06:18:47  lewis
 *		Initial revision
 *		Moved from Dialog.hh/cc and added support for using mainview.
 *
 *
 */

#include	"Dialog.hh"
#include	"TextView.hh"
#include	"PixelMapButton.hh"




class	Alert : public Dialog {
	public:
		Alert (const String& message, const String& okButtonTitle, const String& cancelButtonTitle = kEmptyString);
		~Alert ();
	
		nonvirtual	String	GetMessage () const;
		virtual		void	SetMessage (const String& message);
		
		nonvirtual	AbstractPixelMapButton*	GetIcon () const;
		virtual		void					SetIcon (AbstractPixelMapButton* icon);

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

	private:
		class	AlertMainView*	fAlertMainView;
		AbstractPixelMapButton*	fIcon;
		Boolean					fSizeInvalid;
};

class	StopAlert : public Alert {
	public:
		StopAlert (const String& message, const String& okButtonTitle, const String& cancelButtonTitle = kEmptyString);
};

class	NoteAlert : public Alert {
	public:
		NoteAlert (const String& message, const String& okButtonTitle, const String& cancelButtonTitle = kEmptyString);
};

class	CautionAlert : public Alert {
	public:
		CautionAlert (const String& message, const String& okButtonTitle, const String& cancelButtonTitle = kEmptyString);
};

// to work properly, these must be declared statically
class	StagedAlert {
	public:
		static	const	String	kDefaultButtonTitle;
		StagedAlert (const String& message, Int16 threshold, const String& buttonTitle = kDefaultButtonTitle);
		
		virtual	Boolean	Pose ();
		
		nonvirtual	String	GetMessage () const;
		virtual		void	SetMessage (const String& message);
		
		nonvirtual	Int16	GetPosedCount ();
		virtual		void	SetPosedCount (Int16 count);

	private:
		Int16	fPosedCount;
		Int16	fThreshold;
		String	fMessage;
		String	fButtonTitle;
};



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Alert__*/
