/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__Options_h__
#define	__Options_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/LedLineItMFC/Headers/Options.h,v 2.10 2003/12/31 03:47:09 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Options.h,v $
 *	Revision 2.10  2003/12/31 03:47:09  lewis
 *	SPR#1580: as small part of implementing combobox support - had to change TextStore::SearchParameters saving to TextInteractor::SearchParameters saving.
 *	
 *	Revision 2.9  2002/05/06 21:31:15  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 2.8  2001/11/27 00:28:20  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.7  2001/09/17 14:43:06  lewis
 *	SPR#1032- added SetDefaultFont dialog and saved preferences (pref for new docs)
 *	
 *	Revision 2.6  2001/09/16 18:35:21  lewis
 *	SPR#1033- Added support to check/fix file assocs (depending on user prefs / dialog choices)
 *	to this app (Win32UIFileAssociationRegistrationHelper)
 *	
 *	Revision 2.5  2001/08/30 01:02:00  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.4  2000/06/08 21:49:49  lewis
 *	SPR#0774- added option to treat TAB as indent-selection command - as MSDEV does
 *	
 *	Revision 2.3  1999/12/25 04:15:51  lewis
 *	Add MENU COMMANDS for the qSupportSyntaxColoring option. And debug it so it worked
 *	properly turning on/off and having multiple windows onto the same doc at the same
 *	time (scrolled to same or differnet) places
 *	
 *	Revision 2.2  1999/12/09 17:35:26  lewis
 *	lots of cleanups/fixes for new (partial/full) UNICODE support - SPR#0645
 *	
 *	Revision 2.1  1999/02/22 13:12:03  lewis
 *	add Options module so we can preserve in registry flags like autoindent and smartcutandpaste
 *	
 *
 *
 *	<< Based on code from LedItMFC- 990222 >>
 *
 */

#include	<afxwin.h>

#include	"LedGDI.h"
#include	"LedSupport.h"
#include	"TextInteractor.h"

#include	"LedLineItConfig.h"



class	Options {
	public:
		Options ();
		~Options ();

	public:
		nonvirtual	TextInteractor::SearchParameters	GetSearchParameters () const;
		nonvirtual	void								SetSearchParameters (const TextInteractor::SearchParameters& searchParameters);

	public:
		nonvirtual	const CDockState&	GetDocBarState () const;
		nonvirtual	void				SetDocBarState (const CDockState& dockState);

	public:
		nonvirtual	bool	GetSmartCutAndPaste () const;
		nonvirtual	void	SetSmartCutAndPaste (bool smartCutAndPaste);

	public:
		nonvirtual	bool	GetAutoIndent () const;
		nonvirtual	void	SetAutoIndent (bool autoIndent);

	public:
		nonvirtual	bool	GetTreatTabAsIndentChar () const;
		nonvirtual	void	SetTreatTabAsIndentChar (bool tabAsIndentChar);

#if		qSupportSyntaxColoring
	public:
		enum SyntaxColoringOption { eSyntaxColoringNone = 1, eSyntaxColoringCPlusPlus = 2, eSyntaxColoringVB = 3 };	//NB CANNOT CHANGE VALUES LIGHTLY - WRITTEN TO REGISTRY!
		nonvirtual	SyntaxColoringOption	GetSyntaxColoringOption () const;
		nonvirtual	void					SetSyntaxColoringOption (SyntaxColoringOption syntaxColoringOption);
#endif

#if		qWindows
	public:
		nonvirtual	bool	GetCheckFileAssocsAtStartup () const;
		nonvirtual	void	SetCheckFileAssocsAtStartup (bool checkFileAssocsAtStartup);
#endif

	public:
		nonvirtual	Led_FontSpecification	GetDefaultNewDocFont () const;
		nonvirtual	void					SetDefaultNewDocFont (const Led_FontSpecification& defaultNewDocFont);
};



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */


#endif	/*__Options_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

