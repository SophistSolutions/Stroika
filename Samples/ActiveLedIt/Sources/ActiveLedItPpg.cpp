/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <afxctl.h>

#include "ActiveLedIt.h"

#include "ActiveLedItPpg.h"

/*
 ********************************************************************************
 *********** ActiveLedItPropPage::ActiveLedItPropPageFactory ********************
 ********************************************************************************
 */
// ActiveLedItPropPage::ActiveLedItPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for ActiveLedItPropPage
BOOL ActiveLedItPropPage::ActiveLedItPropPageFactory::UpdateRegistry (BOOL bRegister)
{
    if (bRegister)
        return AfxOleRegisterPropertyPageClass (AfxGetInstanceHandle (), m_clsid, IDS_ACTIVELEDIT_PPG);
    else
        return AfxOleUnregisterClass (m_clsid, NULL);
}

/*
 ********************************************************************************
 ******************************** ActiveLedItPropPage ***************************
 ********************************************************************************
 */
IMPLEMENT_DYNCREATE (ActiveLedItPropPage, COlePropertyPage)

BEGIN_MESSAGE_MAP (ActiveLedItPropPage, COlePropertyPage)
END_MESSAGE_MAP ()

IMPLEMENT_OLECREATE_EX (ActiveLedItPropPage, "ACTIVELEDIT.ActiveLedItPropPage.1",
                        0x9a013a77, 0xad34, 0x11d0, 0x8d, 0x9b, 0, 0xa0, 0xc9, 0x8, 0xc, 0x73)

ActiveLedItPropPage::ActiveLedItPropPage ()
    : COlePropertyPage (IDD, IDS_ACTIVELEDIT_PPG_CAPTION)
{
}

void ActiveLedItPropPage::DoDataExchange (CDataExchange* pDX)
{
    DDP_PostProcessing (pDX);
}
