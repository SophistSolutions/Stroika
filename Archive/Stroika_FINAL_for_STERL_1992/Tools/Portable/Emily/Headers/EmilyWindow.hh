/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__EmilyWindow__
#define	__EmilyWindow__

/*
 * $Header: /fuji/lewis/RCS/EmilyWindow.hh,v 1.3 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: EmilyWindow.hh,v $
 *		Revision 1.3  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *
 *
 *
 */

#include	"Printing.hh"
#include	"Saveable.hh"
#include	"Window.hh"

class	EmilyDocument;
class 	MainGroupItem;
class	EmilyWindow : public Window, public Printable, public Saveable {
	public:
		EmilyWindow (EmilyDocument& document);
		~EmilyWindow ();

		override	void		DoSetupMenus ();
		override	Boolean		DoCommand (const CommandSelection& selection);
		
		override	void		PrintPage (PageNumber pageNumber, class Printer& printer);
		override	void		CalcPages (PageNumber& userStart, PageNumber& userEnd, const Rect& pageRect);
		override	PageNumber	CalcAllPages (const Rect& pageRect);

		nonvirtual	EmilyDocument&	GetDocument ();

		nonvirtual	void	EditModeChanged (Boolean newEditMode);
				
		nonvirtual	String	CalcUniqueStringName ();

		nonvirtual	void	ReadHeaderFile (class istream& from);
		nonvirtual	void	WriteHeaderFile (class ostream& to);
		nonvirtual	void	ReadSourceFile (class istream& from);
		nonvirtual	void	WriteSourceFile (class ostream& to);

		static		CommandNumber	GetGUI ();
		nonvirtual	void			SetGUI (CommandNumber gui);
		static		CommandNumber	GetLanguage ();	
		nonvirtual	void			SetLanguage (CommandNumber language);
		static		Boolean			GetCustomizeOnly ();
		static		Boolean			GetFullEditing ();
		nonvirtual	void			SetCustomizeOnly (Boolean customizeOnly);

	protected:
		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;		
	
	private:
		nonvirtual	MainGroupItem&	GetMainGroup () const;

		EmilyDocument&	fDocument;
		String			fClassName;
		String			fBaseClass;
		MainGroupItem*	fMainGroup;
		
		String			fDataPrepend;
		String			fHeaderPrepend;
		String			fHeaderAppend;
		String			fSourcePrepend;
		String			fSourceAppend;
		
		UInt16			fStringCount;
		Point			fGrid;
		Boolean			fGridVisible;
		
		CommandNumber	fLanguage;
		CommandNumber	fGUI;

		static	CommandNumber	sCurrentGUI;
		static	CommandNumber	sCurrentLanguage;
		static	Boolean			sCustomizeOnly;

	friend	class	SetClassInfoCommand;
};

String	GetGUICompilationDirective (CommandNumber gui);
String	GetLanguageCompilationDirective (CommandNumber language);

#endif	/* __EmilyWindow__ */
