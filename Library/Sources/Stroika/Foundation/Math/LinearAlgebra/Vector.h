/*
* Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
*/
#ifndef _Stroika_Foundation_Math_LinearAlgebra_Vector_h_
#define _Stroika_Foundation_Math_LinearAlgebra_Vector_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Containers/Sequence.h"

/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 *  TODO
 */

namespace Stroika {
    namespace Foundation {
        namespace Math {
            namespace LinearAlgebra {

                /**
                 */
                template <typename T>
                class Vector {
                public:
                    Vector (size_t dimension);

                public:
                    static Vector<T> Ones (size_t dimension);

                public:
                    static Vector<T> Zeros (size_t dimension);

                public:
                    /**
                     *  \note - armadilla calls the Fill overload with a function argument 'imbue'
                     */
                    static Vector<T> Fill (size_t dimension, T value);
                    static Vector<T> Fill (size_t dimension, function<T ()> filler);

                public:
                    /**
                     * construct a new vector by applying the argument function to each element and collecting the results.
                     */
                    nonvirtual Vector<T> Transform (function<T (T)> f) const;

                public:
                    nonvirtual T Norm () const;

                public:
                    nonvirtual Containers::Sequence<T> GetItems () const;

                public:
                    template <typename CONTAINER>
                    nonvirtual void SetItems (const CONTAINER& s);

                public:
                    nonvirtual T operator[] (size_t i) const;

                    // @todo need to return temporary object which can do assignment

                private:
                    class IRep_;

                private:
                    //@todo COPYOnWrite!!!!
                };
            }
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Vector.inl"

#endif /*_Stroika_Foundation_Math_LinearAlgebra_Vector_h_*/
