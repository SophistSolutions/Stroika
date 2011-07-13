/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Command__
#define	__Command__

/*
 * $Header: /fuji/lewis/RCS/Command.hh,v 1.6 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: Command.hh,v $
 *		Revision 1.6  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/21  20:55:17  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.3  1992/07/02  03:46:37  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *		Revision 1.2  1992/07/01  04:05:14  lewis
 *		Renamed Strings.hh String.hh
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.13  92/03/05  17:13:44  17:13:44  lewis (Lewis Pringle)
 *		Made a few CTOR/DTORs inline, and comment on dict. Got rid of Clear method.
 *		
 *		Revision 1.11  1992/02/21  21:06:57  lewis
 *		Use macro INHERRIT_FROM_SINGLE_OBJECT instead of class SingleObject so
 *		that crap doesnt appear in non-mac inheritance graphs.
 *
 *
 *
 */

#include	"String.hh"
#include	"Sequence.hh"

#include	"Config-Framework.hh"


#if		qMPW_MacroOverflowProblem
#define	CommandNumber		CmdN
#define	CmdNameTablePairPtr	CmTP
#endif

typedef	UInt32	CommandNumber;				// SB scoped, but a pain with CFront 2.1 - wait a while...

class	Command INHERRIT_FROM_SINGLE_OBJECT {
	public:
		static	const	Boolean	kUndoable;
		static	const	Boolean	kNotUndoable;
		
		Command (CommandNumber commandNumber, Boolean undoable = kNotUndoable);
		virtual	~Command ();

	public:
		virtual	void	DoIt ();
		virtual	void	UnDoIt ();
		virtual	void	ReDoIt ();
		
		virtual		CommandNumber	GetCommandNumber () const;
		virtual		void			SetCommandNumber (CommandNumber commandNumber);
		virtual		Boolean			GetUndoable () const;
		virtual		void			SetUndoable (Boolean undoable);
		virtual		Boolean			GetDone () const;
		virtual		String			GetName () const;
		virtual		void			SetName (const String& name);

		virtual		Boolean			CommitsLastCommand () const;

	protected:
		virtual		void			SetDone (Boolean done);

	private:
		CommandNumber	fCommandNumber;
		Boolean			fUndoable;
		Boolean			fDone;
		String			fName;
};


struct	CmdNameTablePair {
	CmdNameTablePair ();
	CmdNameTablePair (CommandNumber commandNumber, const String& name);

	~CmdNameTablePair ();

	CommandNumber	fCommandNumber;
	String			fName;
};


extern	Boolean	operator== (const CmdNameTablePair& lhs, const CmdNameTablePair& rhs);
extern	Boolean	operator!= (const CmdNameTablePair& lhs, const CmdNameTablePair& rhs);


// Why use CmdNameTablePairPtr???? Probably to avoid compiler bugs, etc.. ????
// Try strait CmdNameTablePair's sometime - LGP March 4, 1992
// really should be redone as a Mapping_HashTable? indexed by commandNumber??? Right???
#if		!qRealTemplatesAvailable
	typedef	CmdNameTablePair*	CmdNameTablePairPtr;
	Declare (Iterator, CmdNameTablePairPtr);
	Declare (Collection, CmdNameTablePairPtr);
	Declare (AbSequence, CmdNameTablePairPtr);
	Declare (Array, CmdNameTablePairPtr);
	Declare (Sequence_Array, CmdNameTablePairPtr);
	Declare (Sequence, CmdNameTablePairPtr);
#endif	

class	CommandNameTable {
	public:
		CommandNameTable ();
		virtual ~CommandNameTable ();

		nonvirtual	void	AddPair (CommandNumber commandNumber, const String& name);
		nonvirtual	void	AddPair (const CmdNameTablePair& pair);
		nonvirtual	void	AddPairs (const CmdNameTablePair* pairs, CollectionSize count);
		nonvirtual	Boolean	Lookup (CommandNumber commandNumber, String* name);
		nonvirtual	Boolean	Lookup (String name, CommandNumber* commandNumber);

		static	CommandNameTable&	Get ();

	private:
		Sequence(CmdNameTablePairPtr)	fList;
};




typedef	Command*	CommandPtr;

class	CompositeCommand : public Command {
	public:
		CompositeCommand (Command* command, Command* next = Nil);
		~CompositeCommand ();
		
		override	void	DoIt ();
		override	void	UnDoIt ();

		override	CommandNumber	GetCommandNumber () const;
		override	Boolean			GetUndoable () const;		// only if all command are undoable
		override	String			GetName () const;
		override	Boolean			GetDone () const;

	private:	
		Command*	fCommand;
		Command*	fNextCommand;
};

class	CommandHandler {
	public:
		enum	{
			eNoCommand	=	0,
		
			eFirstFrameworkCommand		=	1000,
			eLastFrameworkCommand		=	9999,
			eFirstUserCommand			=	10000,
			eLastUserCommand			=	19999,
			eFirstApplicationCommand	=	50000,
			eLastApplicationCommand		=	59999,

			/*
			 * Framework Commands
			 */
			eFirstDebugMenuCommand		=	eFirstFrameworkCommand,
			eLastDebugMenuCommand		=	eFirstDebugMenuCommand+99,


// Sterl - what the hell is the point of these two 12/13/19 LGP
			ePopupItem,
			eCascadeItem,
			
			eUndo,
		
			/*
			 * Edit Commands
			 */
			eKeyPress,
			eCut,
			eCopy,
			ePaste,
			eClear,
			eSelectAll,
			eDuplicate,
			eToggleClipboardShown,

			/*
			 * System commands.
			 */
			eAboutApplication,

			/*
			 * Quit.
			 */
			eQuit,
		
			/*
			 * Print Related.
			 */
			ePrint,
			ePrintOne,
			ePageSetup,
		
			/*
			 * File Related.
			 */
			eNew,
			eOpen,
			eClose,
			eSave,
			eSaveAs,
			eRevertToSaved,
		};
		CommandHandler ();
		virtual ~CommandHandler ();

		nonvirtual	void	PostCommand (Command* newCommand);
};


/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */

inline	Command::~Command ()				{}

inline	CommandHandler::CommandHandler ()	{}
inline	CommandHandler::~CommandHandler ()	{}





// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***

#endif	/*__Command__*/

