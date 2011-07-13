/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__MenuCommandHandler__
#define	__MenuCommandHandler__

/*
 * $Header: /fuji/lewis/RCS/MenuCommandHandler.hh,v 1.3 1992/09/01 15:42:04 sterling Exp $
 *
 * Description:
 *		A CommandSelection is an abstraction representing a command requested by
 * the user for execution. Its attribute of main interest is its command number which
 * identifies the sort of request made. CommandSelection is both a concrete class,
 * and a class designed for subclassing (for example, a MenuItem is a CommandSelection).
 * CommandSelection's are used as arguments to MenuCommandHandler's.
 *
 * 		NB: They do not have to be used in response to a Menu command per-se. A VERY
 * plausible use would be as generated from some scripting language, like AppleScript,
 * or Apple Events.
 *
 *		A MenuCommandHandler is an abstract interface to handling CommandSelections.
 * Subclass from this class if you expect to be in the "target-chain", or somehow
 * to register interest in, and recieve CommandSelections.
 *
 *		The typical response to the recipt of a CommandSelection is to post a Command
 * object representing the action the CommandSelection requested.
 *
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: MenuCommandHandler.hh,v $
 *		Revision 1.3  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/07/01  04:05:14  lewis
 *		Renamed Strings.hh String.hh
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.7  1992/03/05  17:42:34  lewis
 *		Made 	MenuCommandHandler () ctor protected and inline.
 *		Made the non-virtual methods of MenuCommandHandler static so they can be called
 *		from other than subclasses (eg DebugMenu), and because the "could be".
 *		Commented the purpose of the various classes in the Description part of the headers.
 *
 *		Revision 1.6  1992/02/21  19:44:23  lewis
 *		Got rid of qMPW_SymbolTableOverflowProblem workaround.
 *
 *
 *
 */

#include	"String.hh"

#include	"Command.hh"






class	CommandSelection {
	public:
		CommandSelection (CommandNumber commandNumber);
		CommandSelection (CommandNumber commandNumber, const String& name);
		virtual	~CommandSelection ();

		nonvirtual	CommandNumber	GetCommandNumber () const;
		nonvirtual	void			SetCommandNumber (CommandNumber commandNumber);

		nonvirtual	String			GetName () const;
		nonvirtual	void			SetName (const String& name);
		
		nonvirtual	CollectionSize	GetIndex () const;			// no set cuz usually implied by order of menu sequence

	protected:
		virtual		void	SetCommandNumber_ (CommandNumber commandNumber);		
		virtual		void	SetName_ (const String& name);
		
		virtual		CollectionSize	GetIndex_ () const;
		
	private:
		CommandNumber	fCommandNumber;
		String			fName;
};



class	MenuItem;
#if		qCanFreelyUseVirtualBaseClasses
class	MenuCommandHandler : public virtual CommandHandler
#else
class	MenuCommandHandler : public CommandHandler
#endif
{
	protected:
		MenuCommandHandler ();

	public:
		/*
		 * Override DoSetupMenus () to nable whatever commands this handler will respond to.
		 *
		 * Override DoCommand () to handle a command selection. Return true if handled, False otherwise.
		 * As a side effect, perhaps post a command.
		 *
		 * These two are usually overridden at the same time, and whatever is enabled in the DoSetupMenus override
		 * is usually handled in the DoCommand override.
		 */
		virtual		void	DoSetupMenus ();
		virtual		Boolean	DoCommand (const CommandSelection& selection);

		static	MenuItem*	GetMenuItem (CommandNumber commandNumber);

		static	void	EnableCommand (CommandNumber commandNumber, Boolean enableIt);
		static	void	EnableCommand (CommandNumber commandNumber);
		static	void	DisableCommand (CommandNumber commandNumber);
		
		static	void	SetCommandName (CommandNumber commandNumber, const String& newName);
		static	void	SetOn (CommandNumber commandNumber, Boolean on);
};



/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */

inline	MenuCommandHandler::MenuCommandHandler ()		{}


// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***


#endif	/*__MenuCommandHandler__*/
