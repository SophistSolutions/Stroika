/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Frameworks::Led {

#if qStroika_Frameworks_Led_SupportGDI
    /*
     ********************************************************************************
     ****************************** SimpleTextInteractor ****************************
     ********************************************************************************
     */
    inline void SimpleTextInteractor::SetDefaultFont (const IncrementalFontSpecification& defaultFont, TextInteractor::UpdateMode updateMode)
    {
        TextInteractor::SetDefaultFont (defaultFont, updateMode);
    }
    inline void SimpleTextInteractor::SetTopRowInWindow (size_t newTopRow, UpdateMode updateMode)
    {
        TemporarilySetUpdateMode updateModeSetter (*this, updateMode);
        SetTopRowInWindow (newTopRow);
    }
    inline void SimpleTextInteractor::SetTopRowInWindow (RowReference row, UpdateMode updateMode)
    {
        TemporarilySetUpdateMode updateModeSetter (*this, updateMode);
        SetTopRowInWindow (row);
    }
#endif

}
