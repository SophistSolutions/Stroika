/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef __LedLineItDocFrame_h__
#define __LedLineItDocFrame_h__ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#pragma warning(push)
#pragma warning(disable : 4459)
#include <afxwin.h>
#pragma warning(pop)

#include "Stroika/Frameworks/Led/Support.h"

#include "LedLineItConfig.h"

class LedLineItDocFrame : public CMDIChildWnd {
private:
    using inherited = CMDIChildWnd;

public:
    LedLineItDocFrame ();

protected:
    DECLARE_DYNCREATE (LedLineItDocFrame)

public:
    afx_msg int OnCreate (LPCREATESTRUCT lpCreateStruct);

protected:
    DECLARE_MESSAGE_MAP ()
};

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
inline LedLineItDocFrame::LedLineItDocFrame ()
    : inherited ()
{
}

#endif /*__LedLineItDocFrame_h__*/
