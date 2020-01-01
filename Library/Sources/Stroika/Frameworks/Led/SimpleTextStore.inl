/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Framework_Led_SimpleTextStore_inl_
#define _Stroika_Framework_Led_SimpleTextStore_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Frameworks::Led {

    /*
     ********************************************************************************
     ******************************** SimpleTextStore *******************************
     ********************************************************************************
     */
    inline size_t SimpleTextStore::GetLength () const noexcept
    {
        return (fLength);
    }

}

#endif /*_Stroika_Framework_Led_SimpleTextStore_inl_*/
