/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/ActiveLedIt/Sources/ActiveLedItPpg.cpp,v 2.6 2002/05/06 21:34:30 lewis Exp $
 *
 * Description:
 *		Implementation of the ActiveLedItPropPage property page class.
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: ActiveLedItPpg.cpp,v $
 *	Revision 2.6  2002/05/06 21:34:30  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.5  2001/11/27 00:32:37  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.4  2001/08/30 00:35:57  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.3  1997/12/24 04:44:53  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.2  1997/07/27  16:00:23  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.1  1997/06/28  17:45:52  lewis
 *	*** empty log message ***
 *
 *	Revision 2.0  1997/06/18  03:25:57  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 *
 */

#include	<afxctl.h>

#include	"ActiveLedIt.h"

#include	"ActiveLedItPpg.h"




#ifdef _DEBUG
	#define new DEBUG_NEW
#endif



/*
 ********************************************************************************
 *********** ActiveLedItPropPage::ActiveLedItPropPageFactory ********************
 ********************************************************************************
 */
// ActiveLedItPropPage::ActiveLedItPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for ActiveLedItPropPage
BOOL	ActiveLedItPropPage::ActiveLedItPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(), m_clsid, IDS_ACTIVELEDIT_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}








/*
 ********************************************************************************
 ******************************** ActiveLedItPropPage ***************************
 ********************************************************************************
 */
IMPLEMENT_DYNCREATE(ActiveLedItPropPage, COlePropertyPage)

BEGIN_MESSAGE_MAP(ActiveLedItPropPage, COlePropertyPage)
END_MESSAGE_MAP()

IMPLEMENT_OLECREATE_EX(ActiveLedItPropPage, "ACTIVELEDIT.ActiveLedItPropPage.1",
	0x9a013a77, 0xad34, 0x11d0, 0x8d, 0x9b, 0, 0xa0, 0xc9, 0x8, 0xc, 0x73)

ActiveLedItPropPage::ActiveLedItPropPage() :
	COlePropertyPage(IDD, IDS_ACTIVELEDIT_PPG_CAPTION)
{
}

void	ActiveLedItPropPage::DoDataExchange (CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}



// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

