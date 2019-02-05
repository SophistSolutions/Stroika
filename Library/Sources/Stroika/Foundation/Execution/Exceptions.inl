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

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   Stroia_Foundation_Execution_Exceptions_USE_NOISY_TRACE_IN_THIS_MODULE_       1

namespace Stroika::Foundation::Execution {

    namespace Private_::SystemErrorExceptionPrivate_ {
        Characters::String mkMsg_ (error_code errCode);
        Characters::String mkCombinedMsg_ (error_code errCode, const Characters::String& message);
        void               TranslateException_ (error_code errCode);
    }

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
    template <typename... BASE_EXCEPTION_ARGS>
    inline Exception<BASE_EXCEPTION>::Exception (const Characters::String& reasonForError, BASE_EXCEPTION_ARGS... baseExceptionArgs)
        : ExceptionStringHelper{reasonForError}
        , inherited{forward<BASE_EXCEPTION_ARGS> (baseExceptionArgs)...}
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
    template <typename BASE_EXCEPTION>
    inline SystemErrorException<BASE_EXCEPTION>::SystemErrorException (error_code errCode)
        : SystemErrorException (errCode, Private_::SystemErrorExceptionPrivate_::mkMsg_ (errCode))
    {
    }
    template <typename BASE_EXCEPTION>
    inline SystemErrorException<BASE_EXCEPTION>::SystemErrorException (error_code errCode, const Characters::String& message)
        : inherited (Private_::SystemErrorExceptionPrivate_::mkCombinedMsg_ (errCode, message), errCode)
    {
    }
    template <typename BASE_EXCEPTION>
    inline SystemErrorException<BASE_EXCEPTION>::SystemErrorException (int ev, const std::error_category& ecat)
        : SystemErrorException (error_code{ev, ecat})
    {
    }
    template <typename BASE_EXCEPTION>
    inline SystemErrorException<BASE_EXCEPTION>::SystemErrorException (int ev, const std::error_category& ecat, const Characters::String& message)
        : SystemErrorException (error_code{ev, ecat}, message)
    {
    }
    template <typename BASE_EXCEPTION>
    template <typename... BASE_EXCEPTION_ARGS>
    inline SystemErrorException<BASE_EXCEPTION>::SystemErrorException (const Characters::String& reasonForError, BASE_EXCEPTION_ARGS... baseExceptionArgs)
        : inherited{reasonForError, forward<BASE_EXCEPTION_ARGS> (baseExceptionArgs)...}
    {
    }
    template <typename BASE_EXCEPTION>
    inline void SystemErrorException<BASE_EXCEPTION>::ThrowPOSIXErrNo (errno_t errNo)
    {
#if Stroia_Foundation_Execution_Exceptions_USE_NOISY_TRACE_IN_THIS_MODULE_
        TraceContenxtBumper tctx (L"SystemErrorException<>::ThrowPOSIXErrNo (%d)", errNo);
#endif
        Require (errNo != 0);
#if qPlatform_POSIX
        error_code ec{errNo, system_category ()};
#else
        error_code ec{errNo, generic_category ()};
#endif
        Private_::SystemErrorExceptionPrivate_::TranslateException_ (ec);
        Throw (SystemErrorException (ec));
    }
    template <typename BASE_EXCEPTION>
    void SystemErrorException<BASE_EXCEPTION>::ThrowSystemErrNo (int sysErr)
    {
#if Stroia_Foundation_Execution_Exceptions_USE_NOISY_TRACE_IN_THIS_MODULE_
        TraceContenxtBumper tctx (L"SystemErrorException<>::ThrowSystemErrNo (%d)", sysErr);
#endif
        Require (sysErr != 0);
        error_code ec{sysErr, system_category ()};
        Private_::SystemErrorExceptionPrivate_::TranslateException_ (ec);
        Throw (SystemErrorException (ec));
    }

}

#endif /*_Stroia_Foundation_Execution_Exceptions_inl_*/
