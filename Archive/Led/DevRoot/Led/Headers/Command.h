/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__Command_h_
#define	__Command_h_	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/Command.h,v 2.38 2003/11/27 01:49:18 lewis Exp $
 */


/*
@MODULE:	Command
@DESCRIPTION:
		<p>Module for supporting UNDO. @'Command' objects encapsulate a sequence of operations the user has
	done, and the information needed to undo/redo them. These objects are organized, and kept track of by
	a @'CommandHandler' object. Two sorts of @'CommandHandler's are provided, @'SingleUndoCommandHandler', and
	@'MultiLevelUndoCommandHandler' - depending on whether you want to support multilevel undo or not.</p>
 */



/*
 * Changes:
 *	$Log: Command.h,v $
 *	Revision 2.38  2003/11/27 01:49:18  lewis
 *	SPR#1050: added CommandHandler::BreakInGroupedCommandsIfDifferentCommand () to help
 *	
 *	Revision 2.37  2003/11/26 20:04:29  lewis
 *	SPR#1564: major change to improve undo memory usage. Added SimplePlainTextInsertOptimization
 *	flag and when set (typing a single character) - we first try to just append the character to
 *	the 'afterRep', and if that fails (cuz its not of the right type) - then at least create a
 *	simple afterRep (PlainTextRep) for this special case. Note - we had to modify the smantics
 *	(backward compat code note!) of InteractiveUndoHelperMakeTextRep to return a bool -
 *	indicating if there was any change.
 *	
 *	Revision 2.36  2003/05/20 22:54:15  lewis
 *	SPR#1493: no default CTOR for SavedTextRep. Do/Undo/Redo/DoUndo etc methods now take TextInteractor* arg,
 *	and InteractiveReplaceCommand::CTOR no longer does - and no longer saves fTextInteractor. New
 *	SavedTextRep::ApplySelection () method instead of having code peek at (now private was public)
 *	fSelStart/fSelEnd fields of SavedTextRep.
 *	
 *	Revision 2.35  2002/05/06 21:33:24  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.34  2001/11/27 00:29:38  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.33  2001/10/17 20:42:48  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.32  2001/09/04 21:29:56  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.31  2001/08/29 23:00:14  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.30  2001/08/28 18:43:23  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.29  2000/04/15 14:32:33  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 2.28  2000/04/14 22:40:19  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.27  2000/03/15 16:50:35  lewis
 *	SPR# 0712. Added MultiLevelUndoCommandHandler::SetMaxUnDoLevels/Get. And allow maxUndoLevel=0 -
 *	fix Post() command so this works OK
 *	
 *	Revision 2.26  2000/01/22 16:31:34  lewis
 *	cosmetic cleanups and DOCCOMEMNTS
 *	
 *	Revision 2.25  1999/12/09 03:18:46  lewis
 *	SPR#0645 - preliminary qSDK_UNICODE (_UNICODE) support (mostly using new Led_SDK_String/Led_SDK_Char
 *	
 *	Revision 2.24  1999/11/13 16:32:17  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.23  1999/07/13 22:39:07  lewis
 *	Support CWPro5, and lose support for CWPro2,CWPro3 - and the various BugWorkArounds that were
 *	only needed for these old compilers
 *	
 *	Revision 2.22  1999/05/03 22:04:49  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.21  1999/05/03 21:41:16  lewis
 *	Misc minor COSMETIC cleanups - mostly deleting ifdefed out code
 *	
 *	Revision 2.20  1999/03/25 23:10:12  lewis
 *	need to qualify base class scope for InteractiveReplaceCommand::SavedTextRep on MWERKS compiler
 *	
 *	Revision 2.19  1999/03/25 22:25:18  lewis
 *	add extra SavedTextRep() CTOR
 *	
 *	Revision 2.18  1999/03/25 20:02:48  lewis
 *	cleanup and document
 *	
 *	Revision 2.17  1998/10/30 14:10:37  lewis
 *	use vector<> instead of Led_Array
 *	
 *	Revision 2.16  1998/05/05  00:29:36  lewis
 *	support MWERKSCWPro3 - and lose old bug workarounds.
 *
 *	Revision 2.15  1998/04/25  01:17:10  lewis
 *	Docs
 *
 *	Revision 2.14  1998/03/04  20:18:33  lewis
 *	*** empty log message ***
 *
 *	Revision 2.13  1997/12/24  04:36:55  lewis
 *	*** empty log message ***
 *
 *	Revision 2.12  1997/09/29  14:28:43  lewis
 *	Lose qLedFirstIndex support.
 *
 *	Revision 2.11  1997/07/27  15:56:47  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.10  1997/07/23  23:01:30  lewis
 *	revised docs.
 *
 *	Revision 2.9  1997/07/15  05:25:15  lewis
 *	AutoDoc content.
 *
 *	Revision 2.8  1997/07/12  19:56:17  lewis
 *	TextInteractor_ renamed TextInteractor.
 *	AutoDoc.
 *	fCmdName now string class.
 *
 *	Revision 2.7  1997/06/18  20:04:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.6  1997/03/04  20:02:43  lewis
 *	Added SnoopingCommandHandler.
 *
 *	Revision 2.5  1996/12/13  17:54:11  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.4  1996/12/05  19:08:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.3  1996/09/01  15:30:29  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.2  1996/04/18  15:01:29  lewis
 *	Fix command support to retain selStart/selEnd, and several other bugs
 *	like right command names, and ....
 *
 *	Revision 2.1  1996/02/26  18:31:12  lewis
 *	renamed TextInteracter_ --> TextInteractor_.
 *
 *	Revision 2.0  1996/01/22  05:05:42  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 *
 *
 */

#include	"LedSupport.h"



#if		qLedUsesNamespaces
namespace	Led {
#endif


class	TextInteractor;


/*
@CLASS:			Command
@DESCRIPTION:	<p>An abstraction for a user action, which is undoable. These objects, when created,
	are posted to a @'CommandHandler' via @'CommandHandler::Post' ().</p>
		<p>It is only legal to call Do when Not GetDone(), and only first time and before any of Do/Undo/Redo
	called and can only call Undo when GetDone() is true, and only call ReDo when GetDone () is false.</p>
		<p>Must call inherited Command::Do() etc methods to get fDone flag set properly.</p>
*/
class	Command {
	public:
		Command (bool done = false);
		virtual ~Command ();

	public:
		virtual	void	Do (TextInteractor& interactor);
		virtual	void	UnDo (TextInteractor& interactor);
		virtual	void	ReDo (TextInteractor& interactor);

	public:
		virtual	bool	UpdateSimpleTextInsert (size_t insertAt, Led_tChar c);

	public:
		virtual	const Led_SDK_Char*	GetName () const;

	public:
		nonvirtual	bool	GetDone () const;	// true if DONE or REDONE

	private:
		bool	fDone;
};






/*
@CLASS:			CommandHandler
@DESCRIPTION:	<p>This is the guy you post new @'Command's to, and ask for the text of the
			undo message, etc. You call undo/redo on him to get actual undo/redo etc to
			happen. It is responsable for committing commands, and disposing of them.
			Typically, you would have one of this in your TextInteractor to handle undo.</p>
*/
class	CommandHandler {
	public:
		CommandHandler ();
	public:
		/*
		@METHOD:		CommandHandler::Post
		@DESCRIPTION:	<p>Notify the CommandHandler of a newly created (and done) @'Command'. This command object is now
			owned by the CommandHandler. And it will call delete on it, when it is through with the object. The lifetime
			of this object is totally under the control of the CommandHandler. It can be deleted at any time. No assumptions should
			be made about this.</p>
		*/
		virtual	void	Post (Command* newCommand)	=	0;
		/*
		@METHOD:		CommandHandler::PostUpdateSimpleTextInsert
		@DESCRIPTION:	<p>(prelim docs)Part of optimized undo-info for strings of basic text inserts. Returns true if OK.</p>
		*/
		virtual	bool	PostUpdateSimpleTextInsert (size_t insertAt, Led_tChar c);
		/*
		@METHOD:		CommandHandler::BreakInGroupedCommands
		@DESCRIPTION:	<p>Some commands are plausibly chunked together. These are mostly tying, insert-character commands.
			But, in principle, we could bundle together other commands as well. Call this method to signify that previous
			commands should <em>not</em> be chunked together with commads about to be posted (@'CommandHandler::Post').</p>
		*/
		virtual	void	BreakInGroupedCommands ()	=	0;
		/*
		@METHOD:		CommandHandler::BreakInGroupedCommandsIfDifferentCommand
		@DESCRIPTION:	<p>Like @'CommandHandler::BreakInGroupedCommands', except that it only calls BreakInGroupedCommands
					if the given command name is different than the previous command posted (@'CommandHandler::Post').</p>
		*/
		virtual	void	BreakInGroupedCommandsIfDifferentCommand (const Led_SDK_String& cmdName)	=	0;
		/*
		@METHOD:		CommandHandler::DoUndo
		@DESCRIPTION:	<p>Perform a single undo action. This will undo all the command objects in the current group of commands
			(or all, depending on the subtype of this CommandHandler; see @'SingleUndoCommandHandler' and @'MultiLevelUndoCommandHandler'
			for details).</p>
		*/
		virtual	void	DoUndo (TextInteractor& interactor)	=	0;
		/*
		@METHOD:		CommandHandler::DoRedo
		@DESCRIPTION:	<p>Redo the last undone action. See @'CommandHandler::DoUndo' () for more details.</p>
		*/
		virtual	void	DoRedo (TextInteractor& interactor)	=	0;
		/*
		@METHOD:		CommandHandler::Commit
		@DESCRIPTION:	<p>Commit all currently done actions. This means no further undo will be possible (deletes the
			currently done/redone objects). This can be called when low on memory, to retrive some. Or when some
			logical user-action happens which would prevent further undos (save?).</p>
		*/
		virtual	void	Commit ()	=	0;			// Commit all commands currently owned - something has happened to invalidate them...
		/*
		@METHOD:		CommandHandler::CanUndo
		@DESCRIPTION:	<p>Are there commands posted which can be undone?</p>
		*/
		virtual	bool	CanUndo ()	=	0;
		/*
		@METHOD:		CommandHandler::CanRedo
		@DESCRIPTION:	<p>Are there commands which have been undone, which can now be redone?</p>
		*/
		virtual	bool	CanRedo ()	=	0;
		/*
		@METHOD:		CommandHandler::GetUndoRedoWhatMessageText
		@DESCRIPTION:	<p>Retreives the text associated with each command to denote in a UI what command is being undone,
			or redone.</p>
		*/
		virtual	size_t	GetUndoRedoWhatMessageText (char* buf, size_t bufSize);	// return nbytes - not NUL term string...
		virtual	const Led_SDK_Char*	GetUndoCmdName ()	=	0;		// don't free result - we keep
		virtual	const Led_SDK_Char*	GetRedoCmdName ()	=	0;		// don't free result - we keep
};





/*
@CLASS:			SingleUndoCommandHandler
@BASES:			@'CommandHandler'
@DESCRIPTION:
	<p>CommandHandler which only allows for undo the last command.</p>
*/
class	SingleUndoCommandHandler : public CommandHandler {
	public:
		SingleUndoCommandHandler ();

	public:
		override	void	Post (Command* newCommand);
		override	bool	PostUpdateSimpleTextInsert (size_t insertAt, Led_tChar c);
		override	void	BreakInGroupedCommands ();
		override	void	BreakInGroupedCommandsIfDifferentCommand (const Led_SDK_String& cmdName);
		override	void	DoUndo (TextInteractor& interactor);
		override	void	DoRedo (TextInteractor& interactor);
		override	void	Commit ();
		override	bool	CanUndo ();
		override	bool	CanRedo ();
		override	const Led_SDK_Char*	GetUndoCmdName ();
		override	const Led_SDK_Char*	GetRedoCmdName ();

		virtual		bool	GetDone () const;

	// only support one cmd
	private:
		Command*	fLastCmd;
		#if		qDebug
		bool		fDoingCommands;	// avoid common bug of posting new commands during execution of a command
		#endif
};





/*
@CLASS:			MultiLevelUndoCommandHandler
@BASES:			@'CommandHandler'
@DESCRIPTION:
	<p>CommandHandler which allows for undo the last command several commands. The number of undo levels
	is specified in a constructor argument.</p>
*/
class	MultiLevelUndoCommandHandler : public CommandHandler {
	public:
		MultiLevelUndoCommandHandler (size_t maxUndoLevels, size_t maxCmdsPerLevel = 100);
		virtual ~MultiLevelUndoCommandHandler ();

	public:
		override	void	Post (Command* newCommand);
		override	bool	PostUpdateSimpleTextInsert (size_t insertAt, Led_tChar c);
		override	void	BreakInGroupedCommands ();
		override	void	BreakInGroupedCommandsIfDifferentCommand (const Led_SDK_String& cmdName);
		override	void	DoUndo (TextInteractor& interactor);
		override	void	DoRedo (TextInteractor& interactor);
		override	void	Commit ();			// Commit all commands currently owned - something has happened to invalidate them...
		override	bool	CanUndo ();
		override	bool	CanRedo ();
		override	const Led_SDK_Char*	GetUndoCmdName ();
		override	const Led_SDK_Char*	GetRedoCmdName ();

	public:
		nonvirtual	size_t	GetMaxUnDoLevels ();
		nonvirtual	void	SetMaxUnDoLevels (size_t maxUndoLevels);

	private:
		size_t				fMaxUndoLevels;
		size_t				fMaxCmdsPerLevel;
		size_t				fUndoCursor;
		vector<Command*>	fCommands;
		unsigned			fCommandGroupCount;
		unsigned			fUndoneGroupCount;

		#if		qDebug
		bool				fDoingCommands;	// avoid common bug of posting new commands during execution of a command
		#endif

	private:
		nonvirtual	bool	GetLastCmdRangeBefore (size_t* startIdx, size_t* endIdx) const;	// return true if any cmds in range...
		nonvirtual	bool	GetLastCmdRangeAfter (size_t* startIdx, size_t* endIdx) const;	// return true if any cmds in range...
		nonvirtual	void	Commit_After (size_t after);
		nonvirtual	void	Commit_Before (size_t before);
};





/*
@CLASS:			SnoopingCommandHandler
@BASES:			@'CommandHandler'
@DESCRIPTION:
	<p>Useful for things like recording keyboard macros. Simply install this as your command handler,
	and hand it as argument the OLD one. Later, when you are done recording, re-install the old
	and extract the accumulated commands from this one.</p>

	<p>Basicily all this does is delegate to the given REAL command handler, and call the Snoop ()
	method YOU provide in your subclass. There - you can extract what information you like from
	the argument Command object.</p>
*/
class	SnoopingCommandHandler : public CommandHandler {
	protected:
		SnoopingCommandHandler (CommandHandler* realHandler);

	public:
		nonvirtual	CommandHandler* GetRealHandler () const;

	public:
		override	void	Post (Command* newCommand);
		override	void	BreakInGroupedCommands ();
		override	void	BreakInGroupedCommandsIfDifferentCommand (const Led_SDK_String& cmdName);
		override	void	DoUndo (TextInteractor& interactor);
		override	void	DoRedo (TextInteractor& interactor);
		override	void	Commit ();
		override	bool	CanUndo ();
		override	bool	CanRedo ();
		override	const Led_SDK_Char*	GetUndoCmdName ();
		override	const Led_SDK_Char*	GetRedoCmdName ();

	protected:
		virtual	void	Snoop (Command* newCommand)		=	0;

	private:
		CommandHandler* fRealHandler;
};





/*
@CLASS:			InteractiveReplaceCommand
@BASES:			@'Command'
@DESCRIPTION:
	<p>This subsumes basic typing and cut/paste type commands.
	Grab the text range BEFORE the command, and externize it.
	Do the command, and then grab the range of text AFTER the command
	and externalize that. The before/after text/ranges allow us to undo
	and redo. Note we 'externalize' before/after text so we get font info, and
	any embeddings...</p>
*/
class	TextInteractor;
class	InteractiveReplaceCommand : public Command {
	private:
		typedef	Command	inherited;

	public:
		class	SavedTextRep;
		class	PlainTextRep;

	public:
		InteractiveReplaceCommand (SavedTextRep* beforeRegion, SavedTextRep* afterRegion, size_t at, const Led_SDK_String& cmdName);
		~InteractiveReplaceCommand ();

	public:
		override	void	Do (TextInteractor& /*interactor*/);
		override	void	UnDo (TextInteractor& /*interactor*/);
		override	void	ReDo (TextInteractor& /*interactor*/);

	public:
		override	bool	UpdateSimpleTextInsert (size_t insertAt, Led_tChar c);

	public:
		override	const Led_SDK_Char*	GetName () const;

	protected:
		SavedTextRep*	fBeforeRegion;
		SavedTextRep*	fAfterRegion;
		size_t			fAt;
		Led_SDK_String	fCmdName;
};


/*
@CLASS:			InteractiveReplaceCommand::SavedTextRep
@DESCRIPTION:	<p>Abstraction for what to keep track of in a typical text editing command.</p>
			<p>Note: This fSelStart/fSelEnd refers to the users selection in the text. It has NOTHING todo
	 with the region of text which is saved.</p>
*/
class	InteractiveReplaceCommand::SavedTextRep {
	public:
		SavedTextRep (size_t selStart, size_t selEnd);
		virtual ~SavedTextRep ();

	public:
		/*
		@METHOD:		InteractiveReplaceCommand::SavedTextRep::GetLength
		@DESCRIPTION:	Return the length of the region preserved in this text saved object.
		*/
		virtual	size_t	GetLength () const	=	0;

	public:
		/*
		@METHOD:		InteractiveReplaceCommand::SavedTextRep::InsertSelf
		@DESCRIPTION:	Insert text saved in this object at position 'at' and replacing 'nBytesToOverwrite' Led_tChars
			worth of text.
		*/
		virtual	void	InsertSelf (TextInteractor* imager, size_t at, size_t nBytesToOverwrite)	=	0;


	public:
		/*
		@METHOD:		InteractiveReplaceCommand::SavedTextRep::ApplySelection
		@DESCRIPTION:	Apply the saved selection to the given imager. This must be called <em>after</em> the
					call to @'InteractiveReplaceCommand::SavedTextRep::InsertSelf' so that the saved selection values
					are legitimate.
		*/
		virtual	void	ApplySelection (TextInteractor* imager);

	protected:
		size_t	fSelStart;
		size_t	fSelEnd;
};


/*
@CLASS:			InteractiveReplaceCommand::PlainTextRep
@BASES:			@'InteractiveReplaceCommand::SavedTextRep'
@DESCRIPTION:	<p>Keep track of only the text, and no font, or misc embedding info. On its
			@'InteractiveReplaceCommand::SavedTextRep::InsertSelf' call, it uses @'TextInteractor::Replace'</p>
				<p>Note that this class uses @'LED_DECLARE_USE_BLOCK_ALLOCATION' - so be carefull when subclass.
			</p>
*/
class	InteractiveReplaceCommand::PlainTextRep : public InteractiveReplaceCommand::SavedTextRep {
	private:
		typedef	SavedTextRep	inherited;

	public:
		PlainTextRep (size_t selStart, size_t selEnd, const Led_tChar* text, size_t textLen);
		~PlainTextRep ();

	public:
		LED_DECLARE_USE_BLOCK_ALLOCATION(PlainTextRep);

	public:
		override	size_t	GetLength () const;
		override	void	InsertSelf (TextInteractor* imager, size_t at, size_t nBytesToOverwrite);

	public:
		nonvirtual	bool	AppendCharToRep (size_t insertAt, Led_tChar c);

	private:
		Led_tChar*	fText;
		size_t		fTextLength;
};








/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//	class	SnoopingCommandHandler;
	/*
	@METHOD:		Command::Command
	@DESCRIPTION:	<p>Construct a Command object. The constructor takes an optional bool arg <code>done</code> because
		sometimes commands are created already done (but this defaults to false).</p>
	*/
	inline	Command::Command (bool done):
		fDone (done)
		{
		}
	inline	Command::~Command ()
		{
		}
	/*
	@METHOD:		Command::Do
	@DESCRIPTION:	<p>Perform the actual commands action.</p>
	*/
	inline	void	Command::Do (TextInteractor& /*interactor*/)
		{
			fDone = true;
		}
	/*
	@METHOD:		Command::UnDo
	@DESCRIPTION:	<p>Undo an already done command.</p>
	*/
	inline	void	Command::UnDo (TextInteractor& /*interactor*/)
		{
			fDone = false;
		}
	/*
	@METHOD:		Command::ReDo
	@DESCRIPTION:	<p>Redo a command which has been done (or perhaps redone), and more recently undone.</p>
	*/
	inline	void	Command::ReDo (TextInteractor& /*interactor*/)
		{
			fDone = true;
		}
	/*
	@METHOD:		Command::GetDone
	@DESCRIPTION:	<p>The done, and redone states are considered identical. This returns true if the command has either
		been done, or redone (not most recently undone).</p>
	*/
	inline	bool	Command::GetDone () const
		{
			return fDone;
		}

//	class	SnoopingCommandHandler;
	inline	CommandHandler* SnoopingCommandHandler::GetRealHandler () const
		{
			return fRealHandler;
		}


//	class	MultiLevelUndoCommandHandler;
	/*
	@METHOD:		MultiLevelUndoCommandHandler::GetMaxUnDoLevels
	@DESCRIPTION:	<p>Gets the maximum number of undo levels the @'MultiLevelUndoCommandHandler' will support.
		Zero is a valid # - disabling the keeping of undo information. One is a valid number - implying
		basicly the same behavior as @'SingleUndoCommandHandler' - except that Undo isn't treated as Redo
		after an Undo.</p>
			<p>See @'MultiLevelUndoCommandHandler::SetMaxUnDoLevels'</p>
		
	*/
	inline	size_t	MultiLevelUndoCommandHandler::GetMaxUnDoLevels ()
		{
			return fMaxUndoLevels;
		}




//	class	InteractiveReplaceCommand::SavedTextRep
	inline	InteractiveReplaceCommand::SavedTextRep::SavedTextRep (size_t selStart, size_t selEnd):
		fSelStart (selStart),
		fSelEnd (selEnd)
		{
		}
	inline	InteractiveReplaceCommand::SavedTextRep::~SavedTextRep ()
		{
		}

#if		qLedUsesNamespaces
}
#endif


#endif	/*__Command_h_*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
