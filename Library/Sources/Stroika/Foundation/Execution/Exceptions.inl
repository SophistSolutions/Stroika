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
    inline const string& ExceptionStringHelper::_PeekAtSDKString_ () const
    {
        return fSDKCharString_;
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
        return _PeekAtSDKString_ ().c_str ();
    }

    /*
     ********************************************************************************
     ***************************** SystemErrorException *****************************
     ********************************************************************************
     */
    inline SystemErrorException::SystemErrorException (error_code errCode)
        : SystemErrorException (errCode, mkMsg_ (errCode))
    {
    }
    inline SystemErrorException::SystemErrorException (error_code errCode, const Characters::String& message)
        : inherited (mkCombinedMsg_ (errCode, message), errCode)
    {
    }
    inline SystemErrorException::SystemErrorException (int ev, const std::error_category& ecat)
        : SystemErrorException (error_code{ev, ecat})
    {
    }
    inline SystemErrorException::SystemErrorException (int ev, const std::error_category& ecat, const Characters::String& message)
        : SystemErrorException (error_code{ev, ecat}, message)
    {
    }
#if qPlatform_POSIX
    inline void SystemErrorException::ThrowPOSIXErrNo (errno_t errNo)
    {
        ThrowSystemErrNo (errNo);
    }
#endif

}

#endif /*_Stroia_Foundation_Execution_Exceptions_inl_*/
