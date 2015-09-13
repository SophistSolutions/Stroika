/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__Options_h__
#define	__Options_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/LedIt/Headers/Options.h,v 1.9 2003/12/31 03:40:51 lewis Exp $
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
 *	Revision 1.9  2003/12/31 03:40:51  lewis
 *	SPR#1580: as small part of implementing combobox support - had to change TextStore::SearchParameters saving to TextInteractor::SearchParameters saving.
 *	
 *	Revision 1.8  2002/05/06 21:30:59  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 1.7  2001/11/27 00:28:09  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 1.6  2001/09/26 15:41:46  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.5  2001/09/17 14:42:16  lewis
 *	SPR#1032- added SetDefaultFont dialog and saved preferences (pref for new docs)
 *	
 *	Revision 1.4  2001/09/16 18:35:09  lewis
 *	SPR#1033- Added support to check/fix file assocs (depending on user prefs / dialog choices)
 *	to this app (Win32UIFileAssociationRegistrationHelper)
 *	
 *	Revision 1.3  2001/08/29 22:59:17  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.2  2001/05/16 16:03:18  lewis
 *	more massive changes. Make Options () stuff portable (though fake impl om mac/X). Got rid of
 *	AppWindow class - and now X-WIndows fully using LedDoc/LedView. Much more common code between
 *	implementations - but still a lot todo
 *	
 *	Revision 1.1  2001/05/14 20:54:44  lewis
 *	New LedIt! CrossPlatform app - based on merging LedItPP and LedItMFC and parts of LedTextXWindows
 *	
 *
 *
 *
 *	<<<***		Based on LedItMFC project from Led 3.0b6		***>>>
 *
 *
 *
 *
 */
#if		defined (WIN32)
	#include	<afxwin.h>
#endif

#include	"LedGDI.h"
#include	"LedSupport.h"
#include	"TextInteractor.h"

#include	"LedItConfig.h"



class	Options {
	public:
		Options ();
		~Options ();

	public:
		typedef	TextInteractor::SearchParameters	SearchParameters;
	public:
		nonvirtual	SearchParameters	GetSearchParameters () const;
		nonvirtual	void				SetSearchParameters (const SearchParameters& searchParameters);

#if		qWindows
	public:
		nonvirtual	const CDockState&	GetDocBarState () const;
		nonvirtual	void				SetDocBarState (const CDockState& dockState);
#endif

	public:
		nonvirtual	bool	GetSmartCutAndPaste () const;
		nonvirtual	void	SetSmartCutAndPaste (bool smartCutAndPaste);

	public:
		nonvirtual	bool	GetWrapToWindow () const;
		nonvirtual	void	SetWrapToWindow (bool wrapToWindow);

	public:
		nonvirtual	bool	GetShowHiddenText () const;
		nonvirtual	void	SetShowHiddenText (bool showHiddenText);

	public:
		nonvirtual	bool	GetShowParagraphGlyphs () const;
		nonvirtual	void	SetShowParagraphGlyphs (bool showParagraphGlyphs);

	public:
		nonvirtual	bool	GetShowTabGlyphs () const;
		nonvirtual	void	SetShowTabGlyphs (bool showTabGlyphs);

	public:
		nonvirtual	bool	GetShowSpaceGlyphs () const;
		nonvirtual	void	SetShowSpaceGlyphs (bool showSpaceGlyphs);
	
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

