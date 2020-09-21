/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Debug/Demangle.h"
#include "../Execution/Exceptions.h"

#include "String_Constant.h"

#include "ToString.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

/*
 ********************************************************************************
 ************************************* ToString *********************************
 ********************************************************************************
 */
namespace Stroika::Foundation::Characters {
    template <>
    String ToString (const exception_ptr& e)
    {
        static const String kExceptPefix_{L"Exception: "sv};
        static const String kUnknown_{L"Unknown Exception"sv};
        try {
            rethrow_exception (e);
        }
        catch (const Execution::ExceptionStringHelper& e) {
            //saying Exception: first produces 'Exception: HTTP exception: status 404 (URL not found)}' - redundant. Not sure about all cases, but try this way.
            //return kExceptPefix_ + e.As<String> ();
            return e.As<String> ();
        }
        catch (const exception& e) {
            return kExceptPefix_ + String::FromNarrowSDKString (e.what ());
        }
        catch (...) {
            // fall through
        }
        return kUnknown_;
    }
    template <>
    String ToString (const type_info& t)
    {
        //nb: demangle needed for gcc, but not msvc (but harmless there)
        return Debug::Demangle (String::FromNarrowSDKString (t.name ()));
    }
    template <>
    String ToString (const type_index& t)
    {
        //nb: demangle needed for gcc, but not msvc (but harmless there)
        return Debug::Demangle (String::FromNarrowSDKString (t.name ()));
    }

    String ToString (const char* t)
    {
        // No way to know the 'right' characterset in this case, but as this is mostly used for debugging, no biggie. The caller
        // can be more careful if he cares about charset. This should be safe, and mostly helpful
        return String::FromISOLatin1 (t);
    }

    template <>
    String ToString (const bool& t)
    {
        static const String kTrue_{L"true"sv};
        static const String kFalse{L"false"sv};
        return t ? kTrue_ : kFalse;
    }

    namespace {
        template <typename T>
        inline String num2Str_ (T t, std::ios_base::fmtflags flags)
        {
            static_assert (sizeof (t) <= sizeof (int));
            wchar_t buf[1024];
            switch (flags) {
                case std::ios_base::dec:
                    (void)::swprintf (buf, NEltsOf (buf), L"%d", t);
                    break;
                case std::ios_base::hex:
                    (void)::swprintf (buf, NEltsOf (buf), L"0x%x", t);
                    break;
                default:
                    AssertNotReached ();    // @todo support octal
            }
            return buf;
        }
        template <typename T>
        inline String num2Strl_ (T t, std::ios_base::fmtflags flags)
        {
            wchar_t buf[1024];
            static_assert (sizeof (t) == sizeof (long int));
            switch (flags) {
                case std::ios_base::dec:
                    (void)::swprintf (buf, NEltsOf (buf), L"%ld", t);
                    break;
                case std::ios_base::hex:
                    (void)::swprintf (buf, NEltsOf (buf), L"0x%lx", t);
                    break;
                default:
                    AssertNotReached (); // @todo support octal
            }
            return buf;
        }
        template <typename T>
        inline String num2Strll_ (T t, std::ios_base::fmtflags flags)
        {
            wchar_t buf[1024];
            static_assert (sizeof (t) == sizeof (long long int));
            switch (flags) {
                case std::ios_base::dec:
                    (void)::swprintf (buf, NEltsOf (buf), L"%lld", t);
                    break;
                case std::ios_base::hex:
                    (void)::swprintf (buf, NEltsOf (buf), L"0x%llx", t);
                    break;
                default:
                    AssertNotReached (); // @todo support octal
            }
            return buf;
        }
    }

    template <>
    String ToString (const signed char t, std::ios_base::fmtflags flags)
    {
        return num2Str_ (t, flags);
    }
    template <>
    String ToString (const short int t, std::ios_base::fmtflags flags)
    {
        return num2Str_ (t, flags);
    }
    template <>
    String ToString (const int t, std::ios_base::fmtflags flags)
    {
        return num2Str_ (t, flags);
    }
    template <>
    String ToString (const long int t, std::ios_base::fmtflags flags)
    {
        return num2Strl_ (t, flags);
    }
    template <>
    String ToString (const long long int t, std::ios_base::fmtflags flags)
    {
        return num2Strll_ (t, flags);
    }
    template <>
    String ToString (const unsigned char t, std::ios_base::fmtflags flags)
    {
        return num2Str_ (t, flags);
    }
    template <>
    String ToString (const unsigned short t, std::ios_base::fmtflags flags)
    {
        return num2Str_ (t, flags);
    }
    template <>
    String ToString (const unsigned int t, std::ios_base::fmtflags flags)
    {
        return num2Str_ (t, flags);
    }
    template <>
    String ToString (const unsigned long t, std::ios_base::fmtflags flags)
    {
        return num2Strl_ (t, flags);
    }
    template <>
    String ToString (const unsigned long long t, std::ios_base::fmtflags flags)
    {
        return num2Strll_ (t, flags);
    }

    namespace Private_ {
        String ToString_ex_ (const std::exception& t)
        {
            if (auto p = dynamic_cast<const Execution::ExceptionStringHelper*> (&t)) {
                return p->As<String> ();
            }
            //saying Exception: first produces 'Exception: HTTP exception: status 404 (URL not found)}' - redundant. Not sure about all cases, but try this way.
            //return String_Constant {L"Exception: " } + String::FromNarrowSDKString (t.what ()) + String_Constant {L"}" };
            return String::FromNarrowSDKString (t.what ());
        }
    }
}
