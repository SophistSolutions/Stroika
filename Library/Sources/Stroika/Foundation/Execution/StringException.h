/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroia_Foundation_Execution_StringException_h_
#define _Stroia_Foundation_Execution_StringException_h_ 1

#include "../StroikaPreComp.h"

#include <cerrno>
#include <exception>
#include <optional>
#include <string>
#include <system_error>

#include "../Characters/String.h"

#include "Exceptions.h"

/*
 * TODO:
 *          @todo   Consider if StringException should be templated, so it can be used to wrap as a C++ StringException
 *                  any base std::exception subclass.
 *
 *          @todo   Consider renaming StringException to Exception - the stroika exception class.
 *                  adds assoication with an error_code (especially if I can find a way to provide a generic one with the right message,
 *                  or maybe the generic one is 'no error code').
 */

namespace Stroika::Foundation::Execution {

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
        StringException ()                       = delete;
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
        static inline const error_code kDefaultErrorCode_{1, UnspecifiedErrorCategory_ ()};
#else
        static constexpr error_code kDefaultErrorCode_{1, UnspecifiedErrorCategory_ ()};
#endif
    };

    template <>
    wstring StringException::As () const;
    template <>
    Characters::String StringException::As () const;
    template <>
    error_code StringException::As () const;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "StringException.inl"

#endif /*_Stroia_Foundation_Execution_StringException_h_*/
