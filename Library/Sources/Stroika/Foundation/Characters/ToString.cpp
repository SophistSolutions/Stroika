/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Debug/Demangle.h"
#include    "../Execution/StringException.h"

#include    "String_Constant.h"

#include    "ToString.h"

using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;





/*
 ********************************************************************************
 ************************************* ToString *********************************
 ********************************************************************************
 */
namespace Stroika {
    namespace Foundation {
        namespace Characters {
            template    <>
            String  ToString (const exception_ptr& e)
            {
                static  const   String_Constant kExceptPefix_   {  L"Exception: " };
                static  const   String_Constant kUnknown_       {  L"Unknown Exception" };
                try {
                    std::rethrow_exception (e);
                }
                catch (const Execution::StringException& e) {
                    //saying Exception: first produces 'Exception: HTTP exception: status 404 (URL not found)}' - redundant. Not sure about all cases, but try this way.
                    //return kExceptPefix_ + e.As<String> ();
                    return e.As<String> ();
                }
                catch (const std::exception& e) {
                    return kExceptPefix_ + String::FromNarrowSDKString (e.what ());
                }
                catch (...) {
                    // fall through
                }
                return kUnknown_;
            }
            template    <>
            String  ToString (const type_info& t)
            {
                //nb: demangle needed for gcc, but not msvc (but harmless there)
                return Debug::Demangle (String::FromNarrowSDKString (t.name ()));
            }
            template    <>
            String  ToString (const type_index& t)
            {
                //nb: demangle needed for gcc, but not msvc (but harmless there)
                return Debug::Demangle (String::FromNarrowSDKString (t.name ()));
            }


            template    <>
            String  ToString (const bool& t)
            {
                static  const   String_Constant kTrue_ { L"true" };
                static  const   String_Constant kFalse { L"false" };
                return t ? kTrue_ : kFalse;
            }
            template    <>
            String  ToString (const signed char& t)
            {
                wchar_t buf[1024];
                (void)::swprintf (buf, NEltsOf (buf), L"%d", t);
                return buf;
            }
            template    <>
            String  ToString (const short int& t)
            {
                wchar_t buf[1024];
                (void)::swprintf (buf, NEltsOf (buf), L"%d", t);
                return buf;
            }
            template    <>
            String  ToString (const int& t)
            {
                wchar_t buf[1024];
                (void)::swprintf (buf, NEltsOf (buf), L"%d", t);
                return buf;
            }
            template    <>
            String  ToString (const long int& t)
            {
                wchar_t buf[1024];
                (void)::swprintf (buf, NEltsOf (buf), L"%ld", t);
                return buf;
            }
            template    <>
            String  ToString (const long long int& t)
            {
                wchar_t buf[1024];
                (void)::swprintf (buf, NEltsOf (buf), L"%lld", t);
                return buf;
            }
            template    <>
            String  ToString (const unsigned char& t)
            {
                wchar_t buf[1024];
                (void)::swprintf (buf, NEltsOf (buf), L"0x%x", t);
                return buf;
            }
            template    <>
            String  ToString (const unsigned short& t)
            {
                wchar_t buf[1024];
                (void)::swprintf (buf, NEltsOf (buf), L"0x%x", t);
                return buf;
            }
            template    <>
            String  ToString (const unsigned int& t)
            {
                wchar_t buf[1024];
                (void)::swprintf (buf, NEltsOf (buf), L"0x%x", t);
                return buf;
            }
            template    <>
            String  ToString (const unsigned long& t)
            {
                wchar_t buf[1024];
                (void)::swprintf (buf, NEltsOf (buf), L"0x%lx", t);
                return buf;
            }
            template    <>
            String  ToString (const unsigned long long& t)
            {
                wchar_t buf[1024];
                (void)::swprintf (buf, NEltsOf (buf), L"0x%llx", t);
                return buf;
            }
        }
    }
}

