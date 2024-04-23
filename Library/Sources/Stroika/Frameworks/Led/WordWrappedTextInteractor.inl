/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Frameworks::Led {

#if qStroika_Frameworks_Led_SupportGDI
    /*
     ********************************************************************************
     *************************** WordWrappedTextInteractor **************************
     ********************************************************************************
     */
    inline void WordWrappedTextInteractor::SetTopRowInWindow (size_t newTopRow, UpdateMode updateMode)
    {
        TemporarilySetUpdateMode updateModeSetter (*this, updateMode);
        SetTopRowInWindow (newTopRow);
    }
    inline void WordWrappedTextInteractor::SetTopRowInWindow (RowReference row, UpdateMode updateMode)
    {
        TemporarilySetUpdateMode updateModeSetter (*this, updateMode);
        SetTopRowInWindow (row);
    }
#endif

}
