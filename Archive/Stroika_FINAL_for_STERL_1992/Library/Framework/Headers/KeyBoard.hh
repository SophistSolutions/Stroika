/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__KeyBoard__
#define	__KeyBoard__

/*
 * $Header: /fuji/lewis/RCS/KeyBoard.hh,v 1.3 1992/09/01 15:42:04 sterling Exp $
 *
 *	Description:
 *
 *
 *
 *	TODO:
 *			-		No longer advertise teh various keyCodes - use predefined KeyStrokes instead...
 *
 *
 *	Notes:
 *		At this point I don't know of any standard representation for X, so we'll just use the mac code.
 *		That should work fine. The only funny thing is calling it the OSRepresentation is GetOSReperesention ()
 *		isn't entirely appropriate for X.
 *
 *
 *	Changes:
 *		$Log: KeyBoard.hh,v $
 *		Revision 1.3  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/07/01  04:05:14  lewis
 *		Renamed Strings.hh String.hh
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.15  92/03/11  08:08:14  08:08:14  lewis (Lewis Pringle)
 *		Added iostream operators, and fixed spelling of keystroke modifiers.
 *		
 *		Revision 1.14  1992/03/10  13:08:46  lewis
 *		Make ReferernceCompose () public so we can access it in EventManager. Also, add kBackspace KeyStroke.
 *
 *		Revision 1.13  1992/03/09  23:50:31  lewis
 *		Lots of work on new KeyBoard/KeyStroke interface. Added KeyStrokeModifiers.
 *
 *		Revision 1.10  1992/03/06  22:18:36  lewis
 *		More work on KeyCompose and KeyStroke. Nearly ready to try using it in KeyHandler, and all the subclasses.
 *
 *		Revision 1.9  1992/03/06  18:24:52  lewis
 *		Started work on KeyComposeState, etc...
 *
 *		Revision 1.5  1992/01/03  21:33:43  lewis
 *		Use same stuff for XGDI as for mac.
 *
 *
 *
 *
 */


#include	"Set_BitString.hh"
#include	"String.hh"



/*
 * This class has a hidden represenation that is very system/language/script, etc, dependent.
 * It is used to maintain temporary state information between keys pressed/released at the
 * keyboard in composing a keystroke.
 */
class	KeyComposeState {
	public:
		KeyComposeState ();

#if		qXGDI
		nonvirtual	struct	_XComposeStatus&	ReferenceCompose ();
#endif

	private:
#if		qMacGDI
		UInt32	fCompose;
#elif	qXGDI
		char  	fCompose[8];		// hack so we can compile without knowing type
#endif
};




class	AbSet(KeyStrokeModifiers);

class	KeyStroke {
	public:
		enum	Modifiers {
			eAltKeyModifier,
			eComposeKeyModifier,
			eControlKeyModifier,
			eCommandKeyModifier,
			eMetaKeyModifier,
			eOptionKeyModifier,
			eShiftKeyModifier,
		};

		KeyStroke ();
		KeyStroke (UInt16 character);
		KeyStroke (const KeyStroke& from);
		~KeyStroke ();
		const KeyStroke& operator= (const KeyStroke& rhs);

		nonvirtual	UInt16	GetCharacter () const;		// not sure what this does for special characters??? like ARROWS - SYSDEPENDENT???
		nonvirtual	String	GetPrintRep () const;

		nonvirtual	const	AbSet (KeyStrokeModifiers)&	GetModifiers () const;
		nonvirtual	void								SetModifiers (const	AbSet (KeyStrokeModifiers)& modifiers);

		static	const	KeyStroke	kTab;
		static	const	KeyStroke	kSpace;
		static	const	KeyStroke	kBackspace;
		static	const	KeyStroke	kDelete;
		static	const	KeyStroke	kClear;
		static	const	KeyStroke	kEnter;
		static	const	KeyStroke	kReturn;
		static	const	KeyStroke	kEscape;
		static	const	KeyStroke	kHelp;
		static	const	KeyStroke	kLeftArrow;
		static	const	KeyStroke	kRightArrow;
		static	const	KeyStroke	kDownArrow;
		static	const	KeyStroke	kUpArrow;

	private:
		UInt16	fCharacter;		// Big enuf to hold any character - even double byte...
								// good enuf for mac, and for X too (I HOPE ON BOTH ACCOUNTS)...
		AbSet (KeyStrokeModifiers)*	fModifers;	// have to use pointer til we have real templates since tough to declare one here...
};

extern	Boolean	operator==	(const KeyStroke& lhs, const KeyStroke& rhs);
extern	Boolean	operator!=	(const KeyStroke& lhs, const KeyStroke& rhs);


/*
 * iostream support.
 */
class	istream;
class	ostream;
extern	ostream&	operator<< (ostream& out, const KeyStroke& keyStroke);
extern	istream&	operator>> (istream& in, KeyStroke& keyStroke);

#if		qCFront21
	typedef	KeyStroke::Modifiers	KeyStrokeModifiers;
#endif

#if		!qRealTemplatesAvailable
	Declare (Iterator, KeyStrokeModifiers);
	Declare (Collection, KeyStrokeModifiers);
	Declare (AbSet, KeyStrokeModifiers);
	Declare (Set_BitString, KeyStrokeModifiers);
#endif




/*
 * NB: There is a distinction between raw key codes and virtual key codes, (see IM V-190).
 * Here we only support virtual key codes, and ascii values.  If you want access to the lower
 * level values, you must do so on your own.
 */
class	KeyBoard {
	public:
		enum KeyCode {
#if		qMacGDI
			// Many more key codes can be found in the diagram in IM V-192 - type in later - as needed
			eMinKeyCode		=	0x0,
			eTabKey			=	0x30,
			eSpaceKey		=	0x31,
			eDeleteKey		=	0x33,
			eEscapeKey		=	0x35,
			eCommandKey		=	0x37,
			eShiftKey		=	0x38,
			eCapsLockKey	=	0x39,
			eOptionKey		=	0x3a,
			eControlKey		=	0x3b,
			eEnterKey		=	0x4c,
			eHelpKey		=	0x72,
			eLeftArrowKey	=	0x7b,
			eRightArrowKey	=	0x7c,
			eDownArrowKey	=	0x7d,
			eUpArrowKey		=	0x7e,
			eMaxKeyCode 	=	0x7f,
#elif	qWinOS
			eMinKeyCode		=	0x0,
			eMaxKeyCode 	=	0x7f,
#else	// should be different values, but what
			eMinKeyCode		=	0x0,
			eTabKey			=	0x30,
			eSpaceKey		=	0x31,
			eDeleteKey		=	0x33,
			eEscapeKey		=	0x35,
			eCommandKey		=	0x37,
			eShiftKey		=	0x38,
			eCapsLockKey	=	0x39,
			eOptionKey		=	0x3a,
			eControlKey		=	0x3b,
			eEnterKey		=	0x4c,
			eHelpKey		=	0x72,
			eLeftArrowKey	=	0x7b,
			eRightArrowKey	=	0x7c,
			eDownArrowKey	=	0x7d,
			eUpArrowKey		=	0x7e,
			eMaxKeyCode 	=	0x7f,
#endif	/*GDI*/
		};
		KeyBoard ();
		KeyBoard (KeyCode aKeyCode);
		KeyBoard (KeyCode keyCode1, KeyCode keyCode2);
		KeyBoard (const KeyBoard& keyBoard);
		const KeyBoard&	operator= (const KeyBoard& rhs);

		nonvirtual	void	ClearKeys ();

		nonvirtual	Boolean	GetKey (KeyCode keyCode) const;
		nonvirtual	void	SetKey (KeyCode keyCode, Boolean pressed);

#if		qMacGDI || qXGDI
		nonvirtual	long*			GetOSRepresentation ();
		nonvirtual	const	long*	PeekAtOSRepresentation () const;
#endif	/*qMacOS*/

	private:
#if		qMacGDI || qXGDI
		long	fKeyMap[4];		// really array [0.127] of Boolean in Pascal
#endif	/*GDI*/
};

extern	KeyBoard	GetKeyBoard ();			// snag current state of keyboard

extern	Boolean	operator== (const KeyBoard& lhs, const KeyBoard& rhs);
extern	Boolean	operator!= (const KeyBoard& lhs, const KeyBoard& rhs);








/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */
#if		qMacGDI || qXGDI
inline	long*			KeyBoard::GetOSRepresentation ()				{	return (fKeyMap);	}
inline	const	long*	KeyBoard::PeekAtOSRepresentation () const		{	return (fKeyMap);	}
#endif	/*GDI*/



// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***

#endif	/*__KeyBoard__*/
