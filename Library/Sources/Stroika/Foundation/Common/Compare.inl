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
            inline  bool    ComparerWithEqualsOptionally<T>::Equals (T v1, T v2)
            {
                return v1 == v2;
            }


            template <typename T>
            inline  int ComparerWithWellOrder<T>::Compare (T v1, T v2)
            {
                if (v1 < v2) {
                    Assert (not Equals (v1, v2));
                    return -1;
                }
                else if (v2 < v1) {
                    Assert (not Equals (v1, v2));
                    return 1;
                }
                else {
                    Assert (Equals (v1, v2));
                    return 0;
                }
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Common_Compare_inl_*/
