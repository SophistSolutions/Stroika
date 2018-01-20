/*
* Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
*/
#ifndef _Stroika_Foundation_Math_LinearAlgebra_Vector_h_
#define _Stroika_Foundation_Math_LinearAlgebra_Vector_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Characters/String.h"
#include "../../Containers/Sequence.h"
#include "../../Memory/SharedByValue.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 *  TODO
 */

#define Stroika_Foundation_Math_LinearAlgebra_Vector_ALLOW_MUTATION 1
#ifndef Stroika_Foundation_Math_LinearAlgebra_Vector_ALLOW_MUTATION
#define Stroika_Foundation_Math_LinearAlgebra_Vector_ALLOW_MUTATION 0
#endif

namespace Stroika {
    namespace Foundation {
        namespace Math {
            namespace LinearAlgebra {

                /**
                 */
                template <typename T>
                class Vector {
                public:
                    /**
                     */
                    Vector (size_t dimension);
                    Vector (size_t dimension, Configuration::ArgByValueType<T> fillValue);
                    Vector (size_t dimension, const function<T ()>& filler);
                    template <typename CONTAINER_OF_T, typename ENABLE_IF = typename enable_if<Configuration::IsIterableOfT<CONTAINER_OF_T, T>::value>::type>
                    Vector (const CONTAINER_OF_T& c);

#if Stroika_Foundation_Math_LinearAlgebra_Vector_ALLOW_MUTATION
                public:
                    /**
                     *  \note - Armadillo calls the Fill overload with a function argument 'imbue'
                     */
                    nonvirtual void Fill (T value);
                    nonvirtual void Fill (function<T ()> filler);
#endif

                public:
                    /**
                     */
                    nonvirtual size_t GetDimension () const;

                public:
                    /**
                     * construct a new vector by applying the argument function to each element and collecting the results.
                     */
                    nonvirtual Vector<T> Transform (function<T (T)> f) const;

                public:
                    /**
                     *  Euclidian norm = sqrt (sum (xi^2))
                     */
                    nonvirtual T Norm () const;

                public:
                    /**
                     */
                    nonvirtual Containers::Sequence<T> GetItems () const;

#if Stroika_Foundation_Math_LinearAlgebra_Vector_ALLOW_MUTATION
                public:
                    /**
                     */
                    template <typename CONTAINER>
                    nonvirtual void SetItems (const CONTAINER& s);
#endif

                public:
                    /**
                     */
                    nonvirtual T GetAt (size_t i) const;

#if Stroika_Foundation_Math_LinearAlgebra_Vector_ALLOW_MUTATION
                public:
                    /**
                     */
                    nonvirtual void SetAt (size_t i, Configuration::ArgByValueType<T> v);
#endif

                public:
                    /**
                     */
                    nonvirtual T operator[] (size_t i) const;

#if Stroika_Foundation_Math_LinearAlgebra_Vector_ALLOW_MUTATION
                private:
                    struct TMP_ {
                        Vector<T>& fV;
                        size_t     fIndex;
                        T          fValue;
                        ~TMP_ ()
                        {
                            fV.SetAt (fIndex, fValue);
                        }
                        operator T& ()
                        {
                            return fValue;
                        }
                    };

                public:
                    /**
                     */
                    nonvirtual TMP_ operator[] (size_t i)
                    {
                        return TMP_{*this, i, GetAt (i)};
                    }
#endif

                public:
                    nonvirtual Characters::String ToString () const;

                private:
                    class IRep_;

                private:
                    Memory::SharedByValue<Memory::SharedByValue_Traits<IRep_>> fRep_;
                };

                template <typename T>
                Vector<T> operator* (T lhs, const Vector<T>& rhs)
                {
                    std::vector<T> tmp;
                    for (T i : rhs.GetItems ()) {
                        tmp.push_back (lhs * i);
                    }
                    return tmp;
                }
                template <typename T>
                Vector<T> operator* (const Vector<T>& lhs, T rhs)
                {
                    std::vector<T> tmp;
                    for (T i : lhs.GetItems ()) {
                        tmp.push_back (i * rhs);
                    }
                    return tmp;
                }

                template <typename T>
                Vector<T> operator+ (const Vector<T>& lhs, const Vector<T>& rhs)
                {
                    Require (lhs.GetDimension () == rhs.GetDimension ());
                    std::vector<T> tmp;
                    for (size_t i = 0; i < lhs.GetDimension (); ++i) {
                        tmp.push_back (lhs[i] + rhs[i]);
                    }
                    return tmp;
                }
                template <typename T>
                Vector<T> operator+ (T lhs, const Vector<T>& rhs)
                {
                    std::vector<T> tmp;
                    for (T i : rhs.GetItems ()) {
                        tmp.push_back (lhs + i);
                    }
                    return tmp;
                }
                template <typename T>
                Vector<T> operator+ (const Vector<T>& lhs, T rhs)
                {
                    std::vector<T> tmp;
                    for (T i : lhs.GetItems ()) {
                        tmp.push_back (i + rhs);
                    }
                    return tmp;
                }

                template <typename T>
                Vector<T> operator- (const Vector<T>& lhs, const Vector<T>& rhs)
                {
                    Require (lhs.GetDimension () == rhs.GetDimension ());
                    std::vector<T> tmp;
                    for (size_t i = 0; i < lhs.GetDimension (); ++i) {
                        tmp.push_back (lhs[i] - rhs[i]);
                    }
                    return tmp;
                }
                template <typename T>
                Vector<T> operator- (T lhs, const Vector<T>& rhs)
                {
                    std::vector<T> tmp;
                    for (T i : rhs.GetItems ()) {
                        tmp.push_back (lhs - i);
                    }
                    return tmp;
                }
                template <typename T>
                Vector<T> operator- (const Vector<T>& lhs, T rhs)
                {
                    std::vector<T> tmp;
                    for (T i : lhs.GetItems ()) {
                        tmp.push_back (i - rhs);
                    }
                    return tmp;
                }
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
