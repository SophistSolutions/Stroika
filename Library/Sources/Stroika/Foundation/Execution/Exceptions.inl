/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   Stroia_Foundation_Execution_Exceptions_USE_NOISY_TRACE_IN_THIS_MODULE_       1

namespace Stroika::Foundation::Execution {

    namespace Private_::SystemErrorExceptionPrivate_ {
        Characters::String mkMsg_ (error_code errCode);
        Characters::String mkCombinedMsg_ (error_code errCode, const Characters::String& message);
        void               TranslateException_ (error_code errCode);
        void               TranslateException_ (error_code errCode, const Characters::String& message);
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
        return fSDKCharString_.c_str ();
    }

    /*
     ********************************************************************************
     ********************************** Exception ***********************************
     ********************************************************************************
     */
    template <derived_from<exception> BASE_EXCEPTION>
    inline Exception<BASE_EXCEPTION>::Exception (const Characters::String& reasonForError)
        requires (default_initializable<BASE_EXCEPTION>)
        : ExceptionStringHelper{reasonForError}
        , inherited{}
    {
    }
    template <derived_from<exception> BASE_EXCEPTION>
    inline Exception<BASE_EXCEPTION>::Exception (const Characters::String& reasonForError)
        requires (not default_initializable<BASE_EXCEPTION> and constructible_from<BASE_EXCEPTION, const char*>)
        : ExceptionStringHelper{reasonForError}
        , inherited{""} // BASE needs a what_arg; its value is irrelevant since what () is overridden
    {
    }
    template <derived_from<exception> BASE_EXCEPTION>
    template <typename... BASE_EXCEPTION_ARGS>
    inline Exception<BASE_EXCEPTION>::Exception (const Characters::String& reasonForError, BASE_EXCEPTION_ARGS... baseExceptionArgs)
        : ExceptionStringHelper{reasonForError}
        , inherited{forward<BASE_EXCEPTION_ARGS> (baseExceptionArgs)...}
    {
    }
    template <derived_from<exception> BASE_EXCEPTION>
    const char* Exception<BASE_EXCEPTION>::what () const noexcept
    {
        return _PeekAtNarrowSDKString_ ();
    }

    /*
     ********************************************************************************
     ******************* RuntimeErrorException [[deprecated]] ***********************
     ********************************************************************************
     */
    /**
     *  \deprecated Since Stroika v3.0d25 - say Execution::Exception<runtime_error>, or whichever std exception type you
     *              actually mean (Exception<logic_error>, Exception<out_of_range>, ...). Since v3.0d25
     *              Exception<> supplies the base's what_arg itself, so this added nothing but a name - and a
     *              misleading one whenever BASE_EXCEPTION was not in fact a runtime_error.
     */
    template <derived_from<exception> BASE_EXCEPTION = runtime_error>
    class [[deprecated ("Since Stroika v3.0d25 - use Execution::Exception<runtime_error>")]] RuntimeErrorException
        : public Exception<BASE_EXCEPTION> {
    public:
        RuntimeErrorException (const Characters::String& msg)
            : Exception<BASE_EXCEPTION>{msg}
        {
        }
    };

    /*
     ********************************************************************************
     ******************************* NestedException ********************************
     ********************************************************************************
     */
    inline NestedException::NestedException (const Characters::String& msg, const exception_ptr& basedOnException)
        : Execution::Exception<runtime_error>{msg}
        , fBasedOnException{basedOnException}
    {
    }

    /*
     ********************************************************************************
     ***************************** SystemErrorException *****************************
     ********************************************************************************
     */
    inline SystemErrorException::SystemErrorException (error_code errCode)
        : SystemErrorException{errCode, Private_::SystemErrorExceptionPrivate_::mkMsg_ (errCode)}
    {
    }
    inline SystemErrorException::SystemErrorException (error_code errCode, const Characters::String& message)
        : inherited{Private_::SystemErrorExceptionPrivate_::mkCombinedMsg_ (errCode, message), errCode}
    {
    }
    inline SystemErrorException::SystemErrorException (int ev, const std::error_category& ecat)
        : SystemErrorException{error_code{ev, ecat}}
    {
    }
    inline SystemErrorException::SystemErrorException (int ev, const std::error_category& ecat, const Characters::String& message)
        : SystemErrorException{error_code{ev, ecat}, message}
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
#if qStroika_Foundation_Common_Platform_POSIX
        error_code ec{errNo, system_category ()};
#else
        error_code ec{errNo, generic_category ()};
#endif
        ThrowError (ec);
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
        ThrowError (error_code{sysErr, system_category ()});
    }

    /*
     ********************************************************************************
     ************************************ ThrowError ********************************
     ********************************************************************************
     */
    [[noreturn]] inline void ThrowError (error_code ec)
    {
        Require (ec != error_code{});
        Private_::SystemErrorExceptionPrivate_::TranslateException_ (ec); // [[noreturn]] for the promoted conditions
        Throw (SystemErrorException{ec});
    }
    [[noreturn]] inline void ThrowError (error_code ec, const Characters::String& message)
    {
        Require (ec != error_code{});
        Private_::SystemErrorExceptionPrivate_::TranslateException_ (ec, message); // [[noreturn]] for the promoted conditions
        Throw (SystemErrorException{ec, message});
    }
    [[noreturn]] inline void ThrowError (errc ec)
    {
        Require (ec != errc{}); // errc has no zero enumerator, so errc{} is never a real error
        ThrowError (make_error_code (ec));
    }
    [[noreturn]] inline void ThrowError (errc ec, const Characters::String& message)
    {
        Require (ec != errc{});
        ThrowError (make_error_code (ec), message);
    }

    /*
     ********************************************************************************
     ****************************** ThrowSystemErrNo () *****************************
     ********************************************************************************
     */
#if qStroika_Foundation_Common_Platform_POSIX or qStroika_Foundation_Common_Platform_Windows
    [[noreturn]] inline void ThrowSystemErrNo ()
    {
#if qStroika_Foundation_Common_Platform_POSIX
        ThrowSystemErrNo (errno);
#elif qStroika_Foundation_Common_Platform_Windows
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
