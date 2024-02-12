/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Framework_Led_SimpleTextInteractor_inl_
#define _Stroika_Framework_Led_SimpleTextInteractor_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Frameworks::Led {

#if qStroika_Frameworks_Led_SupportGDI
    /*
     ********************************************************************************
     ***************************** Implementation Details ***************************
     ********************************************************************************
     */
    //  class   SimpleTextInteractor
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

#endif /*_Stroika_Framework_Led_SimpleTextInteractor_inl_*/
