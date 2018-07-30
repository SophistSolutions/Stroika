/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileBusyException_inl_
#define _Stroika_Foundation_IO_FileBusyException_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::IO {

    /*
     ********************************************************************************
     ******************************** FileBusyException *****************************
     ********************************************************************************
     */
    inline String FileBusyException::GetFileName () const
    {
        return fFileName_;
    }

}

#endif /*_Stroika_Foundation_IO_FileBusyException_inl_*/
