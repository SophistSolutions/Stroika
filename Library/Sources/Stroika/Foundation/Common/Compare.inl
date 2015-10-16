/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_Compare_inl_
#define _Stroika_Foundation_Common_Compare_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Common {


#if 0
            template <typename T>
            inline  int Comparer<T>::Compare (T v1, T v2)
            {
                if (v1 < v2) {
                    return -1;
                }
                else if (v2 < v1) {
                    return 1;
                }
                else {
                    Assert (v1 == v2);  // should only do if operator== defined
                    return 0;
                }
            }
            template <typename T>
            inline  bool    Comparer<T>::Equals (T v1, T v2)
            {
                return Compare (v1, v2) == 0;
            }
#endif



            template <typename T>
            inline  bool    ComparerWithEqualsOptionally<T>::Equals (Configuration::ArgByValueType<T> v1, Configuration::ArgByValueType<T> v2)
            {
                return v1 == v2;
            }



            namespace Private_ {
                template <typename T>
                inline  int Compare_Helper_ (Configuration::ArgByValueType<T> v1, Configuration::ArgByValueType<T> v2, typename enable_if<Has_Compare<T>::value>::type* = nullptr)
                {
                    return v1.Compare (v2);
                }
                template <typename T>
                inline  int Compare_Helper_ (Configuration::ArgByValueType<T> v1, Configuration::ArgByValueType<T> v2, typename enable_if<not Has_Compare<T>::value>::type* = nullptr)
                {
                    if (v1 < v2) {
                        return -1;
                    }
                    else if (v2 < v1) {
                        return 1;
                    }
                    else {
                        return 0;
                    }
                }
            }


            template <typename T>
            inline  int ComparerWithWellOrder<T>::Compare (Configuration::ArgByValueType<T> v1, Configuration::ArgByValueType<T> v2)
            {
                int result { Private_::Compare_Helper_<T> (v1, v2) };
                Assert ((result == 0) == Equals (v1, v2));
                return result;
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Common_Compare_inl_*/
