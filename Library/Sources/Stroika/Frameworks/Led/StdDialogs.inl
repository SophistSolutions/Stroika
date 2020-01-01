/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Framework_Led_StdDialogs_inl_
#define _Stroika_Framework_Led_StdDialogs_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Frameworks::Led {
#if qSupportLedDialogWidgets && defined(__cplusplus)
    //  class   LedComboBoxWidget
    inline vector<Led_tString> LedComboBoxWidget::GetPopupItems () const
    {
        return fPopupItems;
    }
#endif
}

#endif /*_Stroika_Framework_Led_StdDialogs_inl_*/
