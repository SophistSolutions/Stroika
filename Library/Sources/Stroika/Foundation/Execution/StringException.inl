/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroia_Foundation_Execution_StringException_inl_
#define _Stroia_Foundation_Execution_StringException_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Debug/Trace.h"

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ******************************** StringException *******************************
     ********************************************************************************
     */
    inline StringException::StringException (const Characters::String& reasonForError)
        : fError_ (reasonForError)
        , fSDKCharString_ (reasonForError.AsNarrowSDKString ())
    {
    }
    template <>
    inline wstring StringException::As () const
    {
        return fError_.As<wstring> ();
    }
    template <>
    inline Characters::String StringException::As () const
    {
        return fError_;
    }
    inline const char* StringException::what () const noexcept
    {
        return fSDKCharString_.c_str ();
    }

}

#endif /*_Stroia_Foundation_Execution_StringException_inl_*/
