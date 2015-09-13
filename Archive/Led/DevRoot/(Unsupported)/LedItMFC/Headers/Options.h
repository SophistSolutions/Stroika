/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1998.  All rights reserved */
#ifndef	__Options_h__
#define	__Options_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItMFC/Headers/Options.h,v 1.5 2000/03/17 22:40:13 lewis Exp $
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
 *	Revision 1.5  2000/03/17 22:40:13  lewis
 *	SPR#0717- preliminary support - for showhiddentext testing and UI
 *	
 *	Revision 1.4  1999/12/28 17:19:01  lewis
 *	save prefrences for ShowPara/Tab/SpaceGlyphs.
 *	
 *	Revision 1.3  1999/02/08 22:31:34  lewis
 *	Support new optional 'wrap-to-window' feature. Used to be we ALWAYS did that. Now can be on/off. Next todo is to fix the OFF case to properly use ruler
 *	
 *	Revision 1.2  1997/12/24 04:46:26  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.1  1997/12/24  04:07:11  lewis
 *	Initial revision
 *
 *
 *
 *
 */
#include	<afxwin.h>

#include	"LedSupport.h"
#include	"TextStore.h"

#include	"LedItConfig.h"



class	Options {
	public:
		Options ();
		~Options ();

	public:
		nonvirtual	TextStore::SearchParameters	GetSearchParameters () const;
		nonvirtual	void						SetSearchParameters (const TextStore::SearchParameters& searchParameters);

	public:
		nonvirtual	const CDockState&	GetDocBarState () const;
		nonvirtual	void				SetDocBarState (const CDockState& dockState);

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

