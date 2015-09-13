/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/WordWrappedTextInteractor.cpp,v 2.17 2003/02/28 19:08:21 lewis Exp $
 *
 * Changes:
 *	$Log: WordWrappedTextInteractor.cpp,v $
 *	Revision 2.17  2003/02/28 19:08:21  lewis
 *	SPR#1316- CRTDBG_MAP_ALLOC/CRTDBG_MAP_ALLOC_NEW Win32/MSVC debug mem leak support
 *	
 *	Revision 2.16  2002/05/06 21:34:01  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.15  2002/04/23 14:51:38  lewis
 *	SPR#1080- fix SHIFT-ENTER processing
 *	
 *	Revision 2.14  2001/11/27 00:30:05  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.13  2001/10/20 13:39:00  lewis
 *	tons of DocComment changes
 *	
 *	Revision 2.12  2001/08/28 18:43:45  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.11  2000/04/15 14:32:38  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 2.10  2000/04/14 22:40:29  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.9  1999/12/19 16:37:33  lewis
 *	For SPR#0667- override WordWrappedTextInteractor::OnTypedNormalCharacter () to handle SHIFT-RETURN
 *	
 *	Revision 2.8  1999/11/13 16:32:25  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.7  1999/05/03 22:05:25  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.6  1998/04/25 01:45:34  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.5  1997/12/24  04:40:13  lewis
 *	*** empty log message ***
 *
 *	Revision 2.4  1997/09/29  15:52:54  lewis
 *	React to changed implemenation of UpdateMode adding (see TextInteractor/DefaultUpdateMode stuff).
 *
 *	Revision 2.3  1997/07/27  15:58:28  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.2  1997/07/12  20:14:23  lewis
 *	*** empty log message ***
 *
 *	Revision 2.1  1997/06/18  20:08:49  lewis
 *	*** empty log message ***
 *
 *	Revision 2.0  1997/06/18  03:11:35  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 *
 *	<========== CODE MOVED HERE FROM TextInteractorMixins.cpp ==========>
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	"WordWrappedTextInteractor.h"



#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif




#if		qLedUsesNamespaces
	namespace	Led {
#endif






/*
 ********************************************************************************
 ***************************** WordWrappedTextInteractor ************************
 ********************************************************************************
 */
WordWrappedTextInteractor::WordWrappedTextInteractor ():
	inherited ()
{
}

/*
@METHOD:		WordWrappedTextInteractor::OnTypedNormalCharacter
@DESCRIPTION:	<p>Override @'TextInteractor::OnTypedNormalCharacter' to map 'shiftPressed' + NL to a soft-line break.</p>
*/
void	WordWrappedTextInteractor::OnTypedNormalCharacter (Led_tChar theChar, bool optionPressed, bool shiftPressed, bool commandPressed, bool controlPressed, bool altKeyPressed)
{
	if (theChar == '\n' and shiftPressed) {
		/*
		 *	Map 'shiftPressed' + NL to a soft-line break, but shutoff controlchar checking since that would
		 *	generate an exception for this character (SPR#1080).
		 */
		bool	savedSupressFlag	=	GetSupressTypedControlCharacters ();
		SetSupressTypedControlCharacters (false);
		try {
			inherited::OnTypedNormalCharacter (WordWrappedTextImager::kSoftLineBreakChar, optionPressed, shiftPressed, commandPressed, controlPressed, altKeyPressed);
		}
		catch (...) {
			SetSupressTypedControlCharacters (savedSupressFlag);
			throw;
		}
		SetSupressTypedControlCharacters (savedSupressFlag);
	}
	else {
		inherited::OnTypedNormalCharacter (theChar, optionPressed, shiftPressed, commandPressed, controlPressed, altKeyPressed);
	}
}

void	WordWrappedTextInteractor::SetTopRowInWindow (size_t newTopRow)
{
	PreScrollInfo	preScrollInfo;
	PreScrollHelper (eDefaultUpdate, &preScrollInfo);
	WordWrappedTextImager::SetTopRowInWindow (newTopRow);
	PostScrollHelper (preScrollInfo);
}

void	WordWrappedTextInteractor::SetTopRowInWindow (RowReference row)
{
	PreScrollInfo	preScrollInfo;
	PreScrollHelper (eDefaultUpdate, &preScrollInfo);
	WordWrappedTextImager::SetTopRowInWindow (row);
	PostScrollHelper (preScrollInfo);
}

void	WordWrappedTextInteractor::SetTopRowInWindowByMarkerPosition (size_t markerPos, UpdateMode updateMode)
{
	SetTopRowInWindow (GetRowReferenceContainingPosition (markerPos), updateMode);
}



#if		qLedUsesNamespaces
}
#endif









// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

