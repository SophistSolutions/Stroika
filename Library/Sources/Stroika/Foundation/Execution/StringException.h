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

#include "Exception.h"
#include "Exceptions.h"

//_DeprecatedFile_ ("DEPRECATED in v2.1d18 - #include Exception.h instead of StringException.h");

namespace Stroika::Foundation::Execution {

#if 1
    using StringException[[deprecated ("DEPRECATED in v2.1d18 - use Exception<>")]] = Exception<exception>;
#else
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
    ....
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
        nonvirtual error_code
        code () const;

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

#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroia_Foundation_Execution_StringException_h_*/
