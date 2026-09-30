/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Frameworks::Led {
#if qStroika_Platform_Windows && defined(__cplusplus)
    //  class   LedComboBoxWidget
    inline vector<Led_tString> LedComboBoxWidget::GetPopupItems () const
    {
        return fPopupItems;
    }
#endif
}
