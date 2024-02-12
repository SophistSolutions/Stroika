/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Framework_Led_WordWrappedTextInteractor_inl_
#define _Stroika_Framework_Led_WordWrappedTextInteractor_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
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

#endif /*_Stroika_Framework_Led_WordWrappedTextInteractor_inl_*/
