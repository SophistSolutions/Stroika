/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/SimpleTextInteractor.cpp,v 2.15 2003/02/28 19:09:52 lewis Exp $
 *
 * Changes:
 *	$Log: SimpleTextInteractor.cpp,v $
 *	Revision 2.15  2003/02/28 19:09:52  lewis
 *	SPR#1316- CRTDBG_MAP_ALLOC/CRTDBG_MAP_ALLOC_NEW Win32/MSVC debug mem leak support
 *	
 *	Revision 2.14  2003/01/29 17:59:53  lewis
 *	SPR#1264- Get rid of most of the SetDefaultFont overrides and crap - and made InteractiveSetFont
 *	REALLY do the interactive command setfont - and leave SetDefaultFont to just setting the 'default font'
 *	
 *	Revision 2.13  2002/05/06 21:33:50  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.12  2001/11/27 00:29:55  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.11  2001/09/26 15:41:20  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.10  2001/08/28 18:43:38  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.9  2000/04/24 21:11:50  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.8  2000/04/14 22:40:26  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.7  1999/11/13 16:32:22  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.6  1999/05/03 22:05:14  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.5  1998/07/24 01:13:03  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.4  1997/12/24  04:40:13  lewis
 *	*** empty log message ***
 *
 *	Revision 2.3  1997/09/29  15:13:21  lewis
 *	revised handling for SetTopRowInWindow - based on re-org of how I handle UpdateModes -
 *	now using TmpSetUpdateMode stuff.
 *
 *	Revision 2.2  1997/07/27  15:58:28  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.1  1997/06/18  20:08:49  lewis
 *	*** empty log message ***
 *
 *	Revision 2.0  1997/06/18  03:00:29  lewis
 *	*** empty log message ***
 *
 *
 *
 *	<========== CODE MOVED HERE FROM TextInteractorMixins.cpp ==========>
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	"SimpleTextInteractor.h"



#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif



#if		qLedUsesNamespaces
namespace	Led {
#endif






/*
 ********************************************************************************
 ***************************** SimpleTextInteractor *****************************
 ********************************************************************************
 */
SimpleTextInteractor::SimpleTextInteractor ():
	InteractorImagerMixinHelper<SimpleTextImager> ()
{
}

void	SimpleTextInteractor::SetDefaultFont (const Led_IncrementalFontSpecification& defaultFont)
{
	SimpleTextImager::SetDefaultFont (defaultFont);
}

void	SimpleTextInteractor::SetTopRowInWindow (size_t newTopRow)
{
	PreScrollInfo	preScrollInfo;
	PreScrollHelper (eDefaultUpdate, &preScrollInfo);
	SimpleTextImager::SetTopRowInWindow (newTopRow);
	PostScrollHelper (preScrollInfo);
}

void	SimpleTextInteractor::SetTopRowInWindow (RowReference row)
{
	PreScrollInfo	preScrollInfo;
	PreScrollHelper (eDefaultUpdate, &preScrollInfo);
	SimpleTextImager::SetTopRowInWindow (row);
	PostScrollHelper (preScrollInfo);
}

void	SimpleTextInteractor::SetTopRowInWindowByMarkerPosition (size_t markerPos, UpdateMode updateMode)
{
	SetTopRowInWindow (GetRowReferenceContainingPosition (markerPos), updateMode);
}

void	SimpleTextInteractor::TabletChangedMetrics ()
{
	SimpleTextImager::TabletChangedMetrics ();
	Refresh ();
}

void	SimpleTextInteractor::ChangedInterLineSpace (PartitionMarker* pm)
{
	SimpleTextImager::ChangedInterLineSpace (pm);
	Refresh ();
}





#if		qLedUsesNamespaces
}
#endif




// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

