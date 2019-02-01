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
    inline StringException::StringException (const Characters::String& reasonForError, optional<error_code> oErrCode)
        : fErrorMessage_ (reasonForError)
        , fErrorCode_{oErrCode.value_or (kDefaultErrorCode_)}
        , fSDKCharString_ (reasonForError.AsNarrowSDKString ())
    {
    }
    template <>
    inline wstring StringException::As () const
    {
        return fErrorMessage_.As<wstring> ();
    }
    template <>
    inline error_code StringException::As () const
    {
        return fErrorCode_;
    }
    template <>
    inline Characters::String StringException::As () const
    {
        return fErrorMessage_;
    }
    inline const char* StringException::what () const noexcept
    {
        return fSDKCharString_.c_str ();
    }
    inline error_code StringException::code () const
    {
        return fErrorCode_;
    }

}

#endif /*_Stroia_Foundation_Execution_StringException_inl_*/
