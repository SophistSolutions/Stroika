/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__WindowPlane__
#define	__WindowPlane__

/*
 * $Header: /fuji/lewis/RCS/WindowPlane.hh,v 1.5 1992/09/01 15:42:04 sterling Exp $
 *
 * Description:
 *
 *		Implement window layers...
 *
 * TODO:
 *
 * Notes:
 *
 * Changes:
 *	$Log: WindowPlane.hh,v $
 *		Revision 1.5  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/21  20:55:17  sterling
 *		Use Sequence instead of obsolete Sequence_DoubleLinkListPtr.
 *
 *		Revision 1.3  1992/07/02  04:47:33  lewis
 *		Renamed Sequence_DoublyLLOfPointers->Sequence_DoubleLinkListPtr.
 *
 *		Revision 1.2  1992/07/01  01:38:44  lewis
 *		Got rid of qNestedTypesAvailable flag - assume always true.
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.12  1992/03/10  00:01:01  lewis
 *		Use new DispatchKeyEvent () interface instead of old HandleKey () interface.
 *
 *		Revision 1.11  1992/01/29  04:38:10  sterling
 *		changed window hints support
 *
 *		Revision 1.10  1992/01/19  21:27:41  lewis
 *		Fixed arg to ReOrderWindow to use SequnceIndex, and added class AlertWindowPlane.
 *
 *		Revision 1.9  1992/01/18  09:21:35  lewis
 *		re-did (partly) algorithm for placement of windows.
 *
 *
 *
 *
 */

#include	"Sequence.hh"

#include	"KeyHandler.hh"
#include	"MenuCommandHandler.hh"
#include	"Panel.hh"

#include	"Window.hh"						// Include only for Sequence_DoubleLinkListPtr(WindowPtr) macro definition...



#if		!qRealTemplatesAvailable
	typedef	class WindowPlane*	WPlnPtr;
	Declare (Iterator, WPlnPtr);
	Declare (Collection, WPlnPtr);
	Declare (AbSequence, WPlnPtr);
	Declare (Array, WPlnPtr);
	Declare (Sequence_Array, WPlnPtr);
	Declare (Sequence, WPlnPtr);
#endif

class	Window;
class	WindowPlane : public Panel, public MenuCommandHandler, public KeyHandler  {
	public:
		enum ActivationKind {
			eAllActive,
			eOneActive,
			eNoneActive,
		};

		static	const	Boolean	kModal;

		WindowPlane (Boolean modal, ActivationKind activationKind);
		virtual	~WindowPlane ();
		
		override	Tablet*	GetTablet () const;
		override	Panel*	GetParentPanel () const;

		override	void		DoSetupMenus ();
		override	Boolean		DoCommand (const CommandSelection& selection);
		override	Boolean		DispatchKeyEvent (KeyBoard::KeyCode code, Boolean isUp, const KeyBoard& keyBoardState,
												  KeyComposeState& composeState);

		nonvirtual	Boolean			GetModal ()	const;
		nonvirtual	ActivationKind	GetActivationKind ()	const;

	public:
		virtual		void	ReorderWindow (Window* w, CollectionSize index = 1);
		nonvirtual	void	ReorderWindow (Window* w, Window* behindWindow);

		// find frontmost visible window containing point where (in global coordintes)
		virtual		Window*	FindWindow (const Point& where)	const;

		nonvirtual	SequenceIterator(WindowPtr)*	MakeWindowIterator (SequenceDirection d = eSequenceForward) const;
		
		// find frontmost/backmost visible window
		nonvirtual	Window*	GetFrontWindow ()	const;
		nonvirtual	Window*	GetBackWindow ()	const;

		/*
		 * Window ordering, and layout
		 */
		virtual		void	Tile ();
		virtual		void	Stack ();




		virtual	void	AdjustWindowShellHints (WindowShellHints& originalHints);


		virtual		void	SynchronizeOSWindows (Window* placeBehind);

		virtual		Boolean	ShouldBeActive (const Window* w) const;

	private:
		friend	class	Window;			// these accessed in its ctor/dtor
		virtual		void	AddWindow (Window* w);
		virtual		void	RemoveWindow (Window* w);

	private:
		Boolean				fModal;
		ActivationKind		fActivationKind;
		Sequence(WindowPtr)	fWindows;
};



class	AlertWindowPlane : public WindowPlane {
	public:
		AlertWindowPlane ();

		override	void	AdjustWindowShellHints (WindowShellHints& originalHints);
};



class	DialogWindowPlane : public WindowPlane {
	public:
		DialogWindowPlane ();

		override	void	AdjustWindowShellHints (WindowShellHints& originalHints);
};



class	StandardWindowPlane : public WindowPlane {
	public:
		StandardWindowPlane ();

		override	void	Tile ();
		override	void	Stack ();

		override	void	AdjustWindowShellHints (WindowShellHints& originalHints);
};




/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */

inline	Boolean						WindowPlane::GetModal () const				{	return (fModal);	}
inline	WindowPlane::ActivationKind	WindowPlane::GetActivationKind () const		{	return (fActivationKind);	}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__WindowPlane__*/

