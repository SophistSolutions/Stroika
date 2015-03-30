/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_ToString_inl_
#define _Stroika_Foundation_Characters_ToString_inl_    1

#include    <wchar.h>

#include    "../Configuration/Concepts.h"
#include    "Float2String.h"

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {


            namespace Private_ {
                using   namespace   Configuration;
                template    <typename T>
                struct  ToString_result_impl {
                    template    <typename X>
                    static auto check(const X& x) -> decltype(x.ToString ());
                    static substitution_failure check(...);
                    using type = decltype(check(declval<T>()));
                };
                template    <typename T>
                using   ToString_result = typename Private_::ToString_result_impl<T>::type;
                template    <typename T>
                struct  has_ToString : integral_constant <bool, not is_same<ToString_result<T>, substitution_failure>::value> {};
            }



            namespace Private_ {
                using   namespace   Configuration;
                template    <typename T>
                struct  beginenditerable_result_impl {
                    template    <typename X>
                    static auto check(const X& x) -> decltype(x.begin () != x.end ());
                    static substitution_failure check(...);
                    using type = decltype(check(declval<T>()));
                };
                template    <typename T>
                using   beginenditerable_result = typename Private_::beginenditerable_result_impl<T>::type;
                template    <typename T>
                struct  has_beginenditerable : integral_constant <bool, not is_same<beginenditerable_result<T>, substitution_failure>::value> {};
            }



            /*
             ********************************************************************************
             ********************************* ToString *************************************
             ********************************************************************************
             */
            namespace Private_ {

                template    <typename T>
                inline  String  ToString_ (const T& t, typename enable_if<Private_::has_ToString<T>::value>::type* = 0)
                {
                    return t.ToString ();
                }

                template    <typename T>
                inline  String  ToString_ (const T& t, typename enable_if<Private_::has_beginenditerable<T>::value and not is_convertible<T, String>::value>::type* = 0)
                {
                    StringBuilder sb;
                    sb << L"{";
                    bool didFirst { false };
                    for (auto i : t) {
                        if (didFirst) {
                            sb << L", ";
                        }
                        else {
                            sb << L" ";
                        }
                        sb << ToString (i);
                        didFirst = true;
                    }
                    if (didFirst) {
                        sb << L" ";
                    }
                    sb << L"}";
                    return sb.str ();
                }

                template    <typename T>
                inline  String  ToString_ (const T& t, typename enable_if<is_convertible<T, String>::value>::type* = 0)
                {
                    return static_cast<String> (t);
                }

                /*
                 * From section from section 3.9.1 of http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3337.pdf
                 *      There are five standard signed integer types : signed char, short int, int,
                 *      long int, and long long int. In this list, each type provides at least as much
                 *      storage as those preceding it in the list.
                 *      For each of the standard signed integer types, there exists a corresponding (but different)
                 *      standard unsigned integer type: unsigned char, unsigned short int, unsigned int, unsigned long int,
                 *      and unsigned long long int, each of which occupies the same amount of storage and has the
                 *      same alignment requirements.
                 */
                template    <typename T>
                inline  String  ToString_ (const T& t, typename enable_if<is_same<T, signed char>::value>::type* = 0)
                {
                    wchar_t buf[1024];
                    swprintf (buf, NEltsOf (buf), L"%d", t);
                    return buf;
                }
                template    <typename T>
                inline  String  ToString_ (const T& t, typename enable_if<is_same<T, short int>::value>::type* = 0)
                {
                    wchar_t buf[1024];
                    swprintf (buf, NEltsOf (buf), L"%d", t);
                    return buf;
                }
                template    <typename T>
                inline  String  ToString_ (const T& t, typename enable_if<is_same<T, int>::value>::type* = 0)
                {
                    wchar_t buf[1024];
                    swprintf (buf, NEltsOf (buf), L"%d", t);
                    return buf;
                }
                template    <typename T>
                inline  String  ToString_ (const T& t, typename enable_if<is_same<T, long int>::value>::type* = 0)
                {
                    wchar_t buf[1024];
                    swprintf (buf, NEltsOf (buf), L"%ld", t);
                    return buf;
                }
                template    <typename T>
                inline  String  ToString_ (const T& t, typename enable_if<is_same<T, long long int>::value>::type* = 0)
                {
                    wchar_t buf[1024];
                    swprintf (buf, NEltsOf (buf), L"%lld", t);
                    return buf;
                }
                template    <typename T>
                inline  String  ToString_ (const T& t, typename enable_if<is_same<T, unsigned char>::value>::type* = 0)
                {
                    wchar_t buf[1024];
                    swprintf (buf, NEltsOf (buf), L"0x%ud", t);
                    return buf;
                }
                template    <typename T>
                inline  String  ToString_ (const T& t, typename enable_if<is_same<T, unsigned short>::value>::type* = 0)
                {
                    wchar_t buf[1024];
                    swprintf (buf, NEltsOf (buf), L"0x%ud", t);
                    return buf;
                }
                template    <typename T>
                inline  String  ToString_ (const T& t, typename enable_if<is_same<T, unsigned int>::value>::type* = 0)
                {
                    wchar_t buf[1024];
                    swprintf (buf, NEltsOf (buf), L"0x%ud", t);
                    return buf;
                }
                template    <typename T>
                inline  String  ToString_ (const T& t, typename enable_if<is_same<T, unsigned long>::value>::type* = 0)
                {
                    wchar_t buf[1024];
                    swprintf (buf, NEltsOf (buf), L"0x%uld", t);
                    return buf;
                }
                template    <typename T>
                inline  String  ToString_ (const T& t, typename enable_if<is_same<T, unsigned long long>::value>::type* = 0)
                {
                    wchar_t buf[1024];
                    swprintf (buf, NEltsOf (buf), L"0x%ulld", t);
                    return buf;
                }

                template    <typename T>
                inline  String  ToString_ (const T& t, typename enable_if < is_floating_point<T>::value>::type* = 0)
                {
                    return Float2String (t);
                }
            }


            template    <typename T>
            inline  String  ToString (const T& t)
            {
                return Private_::ToString_ (t);
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Characters_ToString_inl_*/
