/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   Stroia_Foundation_Execution_Exceptions_USE_NOISY_TRACE_IN_THIS_MODULE_       1

namespace Stroika::Foundation::Execution {

    namespace Private_::SystemErrorExceptionPrivate_ {
        Characters::String    mkMsg_ (error_code errCode);
        Characters::String    mkCombinedMsg_ (error_code errCode, const Characters::String& message);
        void                  TranslateException_ (error_code errCode);
        unique_ptr<exception> TranslateExceptionQuietly_ (error_code errCode);
    }

    // forward declare for use below....to avoid #include of Thread.h
    namespace Thread {
        void CheckForInterruption ();
    }

    /*
     ********************************************************************************
     ******************************** ExceptionStringHelper *************************
     ********************************************************************************
     */
    inline ExceptionStringHelper::ExceptionStringHelper (const Characters::String& reasonForError)
        : ExceptionStringHelper{reasonForError, CaptureCurrentActivities ()}
    {
    }
    inline Characters::String ExceptionStringHelper::GetBasicErrorMessage () const
    {
        return fRawErrorMessage_;
    }
    inline Characters::String ExceptionStringHelper::GetFullErrorMessage () const
    {
        return fFullErrorMessage_;
    }
    inline Containers::Stack<Activity<>> ExceptionStringHelper::GetActivities () const
    {
        return fActivities_;
    }
    template <>
    inline wstring ExceptionStringHelper::As () const
    {
        return fFullErrorMessage_.As<wstring> ();
    }
    template <>
    inline Characters::String ExceptionStringHelper::As () const
    {
        return fFullErrorMessage_;
    }
    inline const char* ExceptionStringHelper::_PeekAtNarrowSDKString_ () const
    {
#if qCompilerAndStdLib_Debug32_asan_Poison_Buggy
        const_cast<char*> (fSDKCharString_.c_str ())[fSDKCharString_.length ()] = '\0';
#endif
        return fSDKCharString_.c_str ();
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
        return _PeekAtNarrowSDKString_ ();
    }

    /*
     ********************************************************************************
     **************************** RuntimeErrorException *****************************
     ********************************************************************************
     */
    template <typename BASE_EXCEPTION>
    inline RuntimeErrorException<BASE_EXCEPTION>::RuntimeErrorException (const Characters::String& msg)
        : Exception<BASE_EXCEPTION>{msg, ""}
    {
    }

    /*
     ********************************************************************************
     ***************************** SystemErrorException *****************************
     ********************************************************************************
     */
    template <typename BASE_EXCEPTION>
    inline SystemErrorException<BASE_EXCEPTION>::SystemErrorException (error_code errCode)
        : SystemErrorException{errCode, Private_::SystemErrorExceptionPrivate_::mkMsg_ (errCode)}
    {
    }
    template <typename BASE_EXCEPTION>
    inline SystemErrorException<BASE_EXCEPTION>::SystemErrorException (error_code errCode, const Characters::String& message)
        : inherited{Private_::SystemErrorExceptionPrivate_::mkCombinedMsg_ (errCode, message), errCode}
    {
    }
    template <typename BASE_EXCEPTION>
    inline SystemErrorException<BASE_EXCEPTION>::SystemErrorException (int ev, const std::error_category& ecat)
        : SystemErrorException{error_code{ev, ecat}}
    {
    }
    template <typename BASE_EXCEPTION>
    inline SystemErrorException<BASE_EXCEPTION>::SystemErrorException (int ev, const std::error_category& ecat, const Characters::String& message)
        : SystemErrorException{error_code{ev, ecat}, message}
    {
    }
    template <typename BASE_EXCEPTION>
    template <typename... BASE_EXCEPTION_ARGS>
    inline SystemErrorException<BASE_EXCEPTION>::SystemErrorException (const Characters::String& reasonForError, BASE_EXCEPTION_ARGS... baseExceptionArgs)
        : inherited{reasonForError, forward<BASE_EXCEPTION_ARGS> (baseExceptionArgs)...}
    {
    }

    /*
     ********************************************************************************
     ******************************** ThrowPOSIXErrNo *******************************
     ********************************************************************************
     */
    inline void ThrowPOSIXErrNo (errno_t errNo)
    {
#if Stroia_Foundation_Execution_Exceptions_USE_NOISY_TRACE_IN_THIS_MODULE_
        TraceContenxtBumper tctx{"Execution::ThrowPOSIXErrNo", "{}"_f, errNo};
#endif
        Require (errNo != 0);
#if qPlatform_POSIX
        error_code ec{errNo, system_category ()};
#else
        error_code ec{errNo, generic_category ()};
#endif
        Private_::SystemErrorExceptionPrivate_::TranslateException_ (ec);
        Throw (SystemErrorException{ec});
    }

    /*
     ********************************************************************************
     ************************* ThrowPOSIXErrNoIfNegative ****************************
     ********************************************************************************
     */
    template <typename INT_TYPE>
    inline INT_TYPE ThrowPOSIXErrNoIfNegative (INT_TYPE returnCode)
    {
        if (returnCode < 0) [[unlikely]] {
            ThrowPOSIXErrNo (errno);
        }
        return returnCode;
    }

    /*
     ********************************************************************************
     ******************************** ThrowSystemErrNo ******************************
     ********************************************************************************
     */
    inline void ThrowSystemErrNo (int sysErr)
    {
#if Stroia_Foundation_Execution_Exceptions_USE_NOISY_TRACE_IN_THIS_MODULE_
        TraceContenxtBumper tctx{"Execution::ThrowSystemErrNo", "{}"_f, sysErr};
#endif
        Require (sysErr != 0);
        error_code ec{sysErr, system_category ()};
        Private_::SystemErrorExceptionPrivate_::TranslateException_ (ec);
        Throw (SystemErrorException{ec});
    }
#if qPlatform_POSIX or qPlatform_Windows
    [[noreturn]] inline void ThrowSystemErrNo ()
    {
#if qPlatform_POSIX
        ThrowSystemErrNo (errno);
#elif qPlatform_Windows
        ThrowSystemErrNo (::GetLastError ());
#endif
    }
#endif

    /*
     ********************************************************************************
     ************************ Handle_ErrNoResultInterruption ************************
     ********************************************************************************
     */
    template <typename CALL>
    auto Handle_ErrNoResultInterruption (CALL call) -> decltype (call ())
    {
        decltype (call ()) ret; // intentionally uninitialized since always set at least once before read
        do {
            ret = call ();
            Execution::Thread::CheckForInterruption ();
        } while (ret < 0 and errno == EINTR);
        return ThrowPOSIXErrNoIfNegative (ret);
    }

    /*
     ********************************************************************************
     ****************************** ThrowPOSIXErrNoIfNull ***************************
     ********************************************************************************
     */
    inline void ThrowPOSIXErrNoIfNull (void* returnValue)
    {
        if (returnValue == nullptr) [[unlikely]] {
            ThrowPOSIXErrNo (errno);
        }
    }

    /*
     ********************************************************************************
     ************************** TranslateExceptionToOptional ************************
     ********************************************************************************
     */
    template <typename F>
    inline auto TranslateExceptionToOptional (F&& f) -> optional<remove_cvref_t<invoke_result_t<F>>>
    {
        try {
            return f ();
        }
        catch (...) {
            //using namespace Characters::Literals;
            //DbgTrace ("Mapping exception in TranslateExceptionToOptional to nullopt: {}"_f, current_exception ());
            return nullopt;
        }
    }

}
