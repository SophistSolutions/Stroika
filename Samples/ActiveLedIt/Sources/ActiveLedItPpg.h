/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef __ActiveLedItPpg_h__
#define __ActiveLedItPpg_h__ 1

/*
 * Description:
 *      Declaration of the ActiveLedItPropPage property page class.
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <afxctl.h>

#include "Resource.h"

class ActiveLedItPropPage : public COlePropertyPage {
    DECLARE_DYNCREATE (ActiveLedItPropPage)
    DECLARE_OLECREATE_EX (ActiveLedItPropPage)

    // Constructor
public:
    ActiveLedItPropPage ();

    //{{AFX_DATA(ActiveLedItPropPage)
    enum { IDD = IDD_PROPPAGE_ACTIVELEDIT };
    // NOTE - ClassWizard will add data members here.
    //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_DATA

protected:
    virtual void DoDataExchange (CDataExchange* pDX); // DDX/DDV support

protected:
    //{{AFX_MSG(ActiveLedItPropPage)
    // NOTE - ClassWizard will add and remove member functions here.
    //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP ()
};

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*__ActiveLedItPpg_h__*/
