/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
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
