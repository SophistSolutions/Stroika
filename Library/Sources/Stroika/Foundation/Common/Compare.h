/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_Compare_h_
#define _Stroika_Foundation_Common_Compare_h_ 1

#include    "../StroikaPreComp.h"

#include    <functional>
#include    <memory>

#include    "../Configuration/Common.h"
#include    "../Configuration/Concepts.h"



/**
 * TODO:
 *      @todo   use http://en.cppreference.com/w/cpp/types/enable_if to make this work better
 *              So for example - we want something like:
 *                  enable_if<is_integral<T>> int compare (T, T) { return v1 - v2; }
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Common {


            /**
             *  Utility you can specialize to define how two types are to be compared equality using the defined operator==(T,T).
             */
            template <typename T>
            struct  ComparerWithEquals {
                typedef T   ElementType;

                RequireConceptAppliesToTypeMemberOfClass(RequireOperatorEquals, T);

                /**
                 *  Returns true if "v1 == v2" - or more precisely - if Compare (v1, v2) == 0. Users can template specialize to
                 *  replace these, but they must remain consistent.
                 */
                static  bool    Equals (T v1, T v2);
            };


            /**
             *  Utility you can specialize to define how two types are to be compared for ordering (and how that fits with equality)
             *  The default implementation only requires you define operator< (T,T) - and it fiugres out other cases from just that.
             *
             *  @todo - writeup !!! NOTE - ASSERTS ComparerWithWellOrder and ComparerWithEquals compatible - namely a < b and b > a iff .... writeup!!!
             */
            template <typename T>
            struct  ComparerWithWellOrder { /*: ComparerWithEquals<T>*/
                typedef T   ElementType;

                RequireConceptAppliesToTypeMemberOfClass(RequireOperatorLess, T);

                /**
                 *  Return < 0 if *this < rhs, return 0 if equal, and return > 0 if *this > rhs.
                 */
                static  int Compare (T v1, T v2);
                static  bool    Equals (T v1, T v2) {
                    return not (v1 < v2 or v2 < v1);
                }
            };


            // @todo - TRY CHANGING USE OF ComparerWithEquals so instance copied around. Verify no perofrmance (size/time)
            //          costs!!!!
            //
            ///EXPERIMENTAL - MIMICS API/CONCEPT of ComparerWithEquals<T> - but without requirement on T
            /// BUT - to make it it work - we need all places that use
            // ComparerWithEquals to keep an instance - and we have t to test/verify that has no codesize/object
            // size implications when used with the above default case (ok that this costs)
            template <typename T>
            struct  ComparerWithEquals_XXX  {
                std::function<bool(T, T)> fEq;

                ComparerWithEquals_XXX (std::function<bool(T, T)> e)
                    : fEq (e) {
                }
                bool    Equals (T v1, T v2) {
                    return fEq (v1, v2);
                }
            };


        }
    }
}


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Compare.inl"

#endif  /*_Stroika_Foundation_Common_Compare_h_*/
