/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroia_Foundation_Execution_Exceptions_h_
#define _Stroia_Foundation_Execution_Exceptions_h_ 1

#include "../StroikaPreComp.h"

#include <cerrno>
#include <exception>
#include <optional>
#include <string>
#include <system_error>

#include "../Characters/String.h"

#include "Throw.h"

/**
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  \note \em Design Note
 *      (essentially) All exceptions thrown by Stroika (except where needed by quirks of underlying library
 *      integrated with) inherit from std::exception, or Stroika::Foundation::Execution::SilentException.
 *
 *      This means that any code which wishes to report an exception can catch these two types, and use
 *      the 'what()' method to report the text of the exception message.
 *
 *      Sadly, there is no documentation (I'm aware of) or specification of the characterset/code page reported
 *      back by the what () on an exception. It tends to be ascii. Stroika guarantees that all exceptions it throws
 *      will use the current SDK characters (@see SDKString). But - its best to check for inheriting from
 *      StringException, since the SDK characterset might not allow representing some unicode characters.
 *
 * TODO:
 *          @todo   And make FileAccessException subclass from SystemErrorr? instead of StringExcpetion?
 */

namespace Stroika::Foundation::Execution {
    /**
  */
    class ExceptionStringHelper {
    public:
        ExceptionStringHelper ()                             = delete;
        ExceptionStringHelper (const ExceptionStringHelper&) = default;
        ExceptionStringHelper (const Characters::String& reasonForError);

    public:
        /**
         * Only implemented for
         *      o   wstring
         *      o   String
         */
        template <typename T>
        nonvirtual T As () const;

    private:
    public:
        Characters::String fErrorMessage_;
        string             fSDKCharString_;
    };
    template <>
    wstring ExceptionStringHelper::As () const;
    template <>
    Characters::String ExceptionStringHelper::As () const;

    /**
     */
    template <typename BASE_EXCEPTION = exception>
    class Exception : public ExceptionStringHelper, public BASE_EXCEPTION {
    private:
        using inherited = BASE_EXCEPTION;

    public:
        /**
         */
        Exception ()                 = delete;
        Exception (const Exception&) = default;
        Exception (const Characters::String& reasonForError);

    protected:
        /**
         * For BASE_EXCEPTION classes with constructors OTHER than just 'message' - you cannot use a 'using X = Exception(x)' but a subclass
         * which uses this delegating hmethod.
         */
        template <typename... ARGS>
        Exception (const Characters::String& reasonForError, ARGS... args);

    public:
        /**
         *  Provide a 'c string' variant of the exception message. Convert the UNICODE
         *  string argument to a narrow-string (multibyte) in the SDK code page.
         *  @see GetDefaultSDKCodePage()
         */
        virtual const char* what () const noexcept override;
    };

    /**
     */
    using RuntimeException = Exception<runtime_error>;

    /**
     */
    class SystemException : public Exception<system_error> {
        using inherited = Exception<system_error>;

    private:
        static Characters::String mkMsg_ (error_code errCode)
        {
            return Characters::String::FromNarrowSDKString (errCode.message ());
        }
        static Characters::String mkMsg_ (error_code errCode, const Characters::String& message)
        {
            return message + L": " + mkMsg_ (errCode);
        }

    public:
        SystemException (error_code _Errcode)
            : inherited (mkMsg_ (_Errcode), _Errcode)
        {
        }
        SystemException (error_code errCode, const Characters::String& message)
            : inherited (mkMsg_ (errCode, message), errCode)
        {
        }
        SystemException (int ev, const std::error_category& ecat)
            : inherited (mkMsg_ (error_code (ev, ecat)), error_code (ev, ecat))
        {
        }
        SystemException (int ev, const std::error_category& ecat, const Characters::String& message)
            : inherited (mkMsg_ (error_code (ev, ecat), message), error_code (ev, ecat))
        {
        }
    };

#if 0
    /**
     *  This takes a 'String' argument, and maps it to the 'what()' in std::exception.
     *  This maps using the default native SDK characterset.
     *
     *  \note This object as a code () method returing error_code but CTOR taking optional<error_code>. We could have
     *        required an error code be specified (but frequently callers wont have a good one so they will pass a bad value).
     *        Or we could have code() return optional. BUt that would then require much more special case handling when comparing
     *        with error condition categories. So we may 'none specified' in the constructor to a special error that probably wont
     *        match any error conditions checked for.
     */
    class StringException : public exception {
    private:
        using inherited = exception;

    public:
        /**
         *  Its best to pass in an error code so places that capture the exception can compare (if e.As<error_code> () == some-error-category).
         *  But sometimes this will not be provided.
         */
        StringException () = delete;
        StringException (const StringException&) = default;
        StringException (const Characters::String& reasonForError, optional<error_code> oErrCode = {});

    public:
        /**
         * Only implemented for
         *      o   wstring
         *      o   String
         *      o   error_code       (see code ())
         */
        template <typename T>
        nonvirtual T As () const;

    public:
        /**
         * \note called this code () instead of GetErrorCode () to mimic system_error::code()
         *
         *
        TODO - writeup and test theory

        from   https://stackoverflow.com/questions/32232295/use-cases-for-stderror-code

        portable list of error conditions: https://en.cppreference.com/w/cpp/error/errc

        void handle_error(error_code code) {
           if     (code == error_condition1) do_something();
           else if(code == error_condition2) do_something_else();
           else                              do_yet_another_thing();
        }

         So example use would be catch (StringException e) {
            if (e.As<error_code> () == condition1) {
            // and this would MATCH somehow differnt codes via categories to see if it matches the condition
            }
         */
        nonvirtual error_code code () const;

    public:
        /**
         *  Provide a 'c string' variant of the exception message. Convert the UNICODE
         *  string argument to a narrow-string (multibyte) in the SDK code page.
         *  @see GetDefaultSDKCodePage()
         */
        virtual const char* what () const noexcept override;

    private:
        Characters::String fErrorMessage_;
        error_code         fErrorCode_{};
        string             fSDKCharString_;

    private:
        class UnspecifiedErrorCategory_ : public error_category {
        public:
            virtual const char* name () const noexcept
            {
                return "Unspecified Error Category";
            }
            virtual string message ([[maybe_unused]] int errVal) const
            {
                return "Unspecified Error";
            }
        };
#if qCompilerAndStdLib_constexpr_error_category_ctor_Buggy
        static inline const error_code kDefaultErrorCode_{ 1, UnspecifiedErrorCategory_ () };
#else
        static constexpr error_code kDefaultErrorCode_{ 1, UnspecifiedErrorCategory_ () };
#endif
    };

    template <>
    wstring StringException::As () const;
    template <>
    Characters::String StringException::As () const;
    template <>
    error_code StringException::As () const;
#endif
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Exceptions.inl"

#endif /*_Stroia_Foundation_Execution_Exceptions_h_*/
