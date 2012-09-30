/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef __LedLineItDocFrame_h__
#define __LedLineItDocFrame_h__ 1

#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <afxwin.h>
#include    <afxext.h>

#include    "Stroika/Frameworks/Led/Support.h"

#include    "LedLineItConfig.h"


class   LedLineItDocFrame : public CMDIChildWnd {
private:
    typedef CMDIChildWnd    inherited;

public:
    LedLineItDocFrame ();
protected:
    DECLARE_DYNCREATE (LedLineItDocFrame)

public:
    afx_msg int     OnCreate (LPCREATESTRUCT lpCreateStruct);

protected:
    DECLARE_MESSAGE_MAP()
};


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
inline  LedLineItDocFrame::LedLineItDocFrame ():
    inherited ()
{
}



#endif  /*__LedLineItDocFrame_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

