/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroia_Foundation_Execution_Exceptions_inl_
#define _Stroia_Foundation_Execution_Exceptions_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ******************************** ExceptionStringHelper *************************
     ********************************************************************************
     */
    inline ExceptionStringHelper::ExceptionStringHelper (const Characters::String& reasonForError)
        : fErrorMessage_ (reasonForError)
        , fSDKCharString_ (reasonForError.AsNarrowSDKString ())
    {
    }
    template <>
    inline wstring ExceptionStringHelper::As () const
    {
        return fErrorMessage_.As<wstring> ();
    }
    template <>
    inline Characters::String ExceptionStringHelper::As () const
    {
        return fErrorMessage_;
    }

    /*
     ********************************************************************************
     ********************************** Exception ***********************************
     ********************************************************************************
     */
    template <typename BASE_EXCEPTION>
    inline Exception<BASE_EXCEPTION>::Exception (const Characters::String& reasonForError)
        : ExceptionStringHelper{reasonForError}
        , inherited{}
    {
    }
    template <typename BASE_EXCEPTION>
    template <typename... ARGS>
    inline Exception<BASE_EXCEPTION>::Exception (const Characters::String& reasonForError, ARGS... args)
        : ExceptionStringHelper{reasonForError}
        , inherited{forward<ARGS> (args)...}
    {
    }
    template <typename BASE_EXCEPTION>
    const char* Exception<BASE_EXCEPTION>::what () const noexcept
    {
        return fSDKCharString_.c_str ();
    }

}

#endif /*_Stroia_Foundation_Execution_Exceptions_inl_*/
