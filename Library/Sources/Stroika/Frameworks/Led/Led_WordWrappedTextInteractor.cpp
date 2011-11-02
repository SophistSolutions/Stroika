/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	"Led_WordWrappedTextInteractor.h"



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

