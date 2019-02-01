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
 *
 *          @todo   Maybe add As<error_code> () as well.
 */

namespace Stroika::Foundation::Execution {

    /**
     *  This takes a 'String' argument, and maps it to the 'what()' in std::exception.
     *  This maps using the default native SDK characterset.
     */
    class StringException : public exception {
    private:
        using inherited = exception;

    public:
        StringException ()                       = delete;
        StringException (const StringException&) = default;
        StringException (const Characters::String& reasonForError, optional<error_code> oErrCode = {});

    public:
        /**
         * Only implemented for
         *      o   wstring
         *      o   String
         */
        template <typename T>
        nonvirtual T As () const;

    public:
        /**
        TODO - writeup and test theory

        from   https://stackoverflow.com/questions/32232295/use-cases-for-stderror-code

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
        nonvirtual error_code GetErrorCode () const;

    public:
        /**
         *  Provide a 'c string' variant of the exception message. Convert the UNICODE
         *  string argument to a narrow-string (multibyte) in the SDK code page.
         *  @see GetDefaultSDKCodePage()
         */
        virtual const char* what () const noexcept override;

    private:
        Characters::String      fErrorMessage_;
        error_code              fErrorCode_{};
        string                  fSDKCharString_;
        static const error_code kDefaultErrorCode_; // for now - later improve to capture string somehow
    };

    template <>
    wstring StringException::As () const;
    template <>
    Characters::String StringException::As () const;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "StringException.inl"

#endif /*_Stroia_Foundation_Execution_StringException_h_*/
