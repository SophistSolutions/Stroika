/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
