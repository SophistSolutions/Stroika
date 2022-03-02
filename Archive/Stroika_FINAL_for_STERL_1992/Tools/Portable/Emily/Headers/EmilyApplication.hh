/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__EmilyApplication__
#define	__EmilyApplication__

/*
 * $Header: /fuji/lewis/RCS/EmilyApplication.hh,v 1.6 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: EmilyApplication.hh,v $
 *		Revision 1.6  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/07/22  00:34:55  lewis
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.18  92/05/13  18:38:19  18:38:19  lewis (Lewis Pringle)
 *		STERL.
 *		
 *		Revision 1.9  1992/02/15  07:56:02  lewis
 *		In Stroika, StandardMenu changed to Menu.
 *
 *
 *
 *
 */

#include	"Application.hh"
#include	"DebugMenu.hh"
#include	"MenuBar.hh"

#include	"Saveable.hh"

#include	"EmilyDocument.hh"
#include	"EmilyMenus.hh"



#if		!qRealTemplatesAvailable
	// copied from Stroika-Foundation-Globals.hh - see comment there for explanation...
	#include	"Collection.hh"
	#include	"Sequence.hh"
	#include	"Set.hh"
	
	#ifndef		_ContainersOfStringDeclared_
		#define		_ContainersOfStringDeclared_
		Declare (Iterator, String);
		Declare (Collection, String);
		Declare (AbSequence, String);
		Declare (AbSet, String);
		Declare (Array, String);
		Declare (Sequence_Array, String);
		Declare (Sequence, String);
		Declare (Set_Array, String);
		Declare (Set, String);
	#endif		/*_ContainersOfStringDeclared_*/
#endif		/*!qRealTemplatesAvailable*/



class	EmilyApplication : public Application, public Saveable {
	public:
		EmilyApplication (int argc, const char* argv []);
		~EmilyApplication ();
				
		override	void	DoSetupMenus ();
		override	Boolean	DoCommand (const CommandSelection& selection);

		override	void	ShowMessageForException (const Exception& exception);

		nonvirtual	void	AddDocument (EmilyDocument* doc);
		nonvirtual	void	RemoveDocument (EmilyDocument* doc);

		/* Application Preferences */
		nonvirtual	Point	GetGrid () const;
		nonvirtual	Boolean	GetGridVisible () const;
		nonvirtual	Boolean	GetConstrainTrackers () const;
		nonvirtual	String	GetHeaderSuffix () const;
		nonvirtual	String	GetSourceSuffix () const;
		nonvirtual	String	GetDataSuffix () const;
		nonvirtual	String	GetDefaultPrepend () const;
		nonvirtual	Boolean	GetCompileOnce () const;
		
		static	EmilyApplication&	Get ();
	
	protected:
		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;
		override	void	Startup ();
		
	private:
		nonvirtual	void	BuildSpacingMenus ();
		nonvirtual	String	GetPreferencesFileName () const;
		nonvirtual	void	EditModeChanged (Boolean newMode);
		nonvirtual	void	SetPreferences ();
		nonvirtual	void	CreateDocument ();
		nonvirtual	void	OpenDocument ();

		CommandNamesBuilder	fCommandNamesBuilder;
		MenuBar				fMenuBar;
		SystemMenu			fSystemMenu;
		EmilyFileMenu		fFileMenu;
		EmilyEditMenu		fEditMenu;
		EmilyFormatMenu		fFormatMenu;
		EmilyCustomizeMenu	fCustomizeMenu;
#if		qDebug
		DebugMenu			fDebugMenu;
#endif

		Sequence(EmilyDocumentPtr)	fDocuments;		
		Sequence(String)				fSpacings;
		
		Point			fGrid;
		Boolean			fGridVisible;
		Boolean			fConstrainTrackers;
		String			fHeaderSuffix;
		String			fSourceSuffix;
		String			fDataSuffix;
		String			fDefaultPrepend;
		Boolean			fCompileOnce;
		
		static	EmilyApplication*	sThe;
		static	String	kDefaultPrepend;
		
		friend	class	SetPreferencesInfo;
};

#endif	/* __EmilyApplication__ */
