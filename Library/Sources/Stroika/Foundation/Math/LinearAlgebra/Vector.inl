/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Math_LinearAlgebra_Vector_inl_
#define _Stroika_Foundation_Math_LinearAlgebra_Vector_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include <vector>

namespace Stroika {
    namespace Foundation {
        namespace Math {
            namespace LinearAlgebra {

                /*
                 ********************************************************************************
                 ******************************** Vector<T>::IRep_ ******************************
                 ********************************************************************************
                 */
                template <typename T>
                class Vector<T>::IRep_ {
                public:
                    Containers::Sequence<T> fData;
                };

                /*
                 ********************************************************************************
                 ********************************** Vector<T> ***********************************
                 ********************************************************************************
                 */
                template <typename T>
                inline Vector<T>::Vector (size_t dimension)
                    : Vector (dimension, 0)
                {
                }
                template <typename T>
                Vector<T>::Vector (size_t dimension, Configuration::ArgByValueType<T> fillValue)
                    : fRep_ (make_shared<IRep_> ())
                {
                    for (size_t i = 0; i < dimension; ++i) {
                        fRep_->get ()->Append (fillValue);
                    }
                }
                template <typename T>
                Vector<T>::Vector (size_t dimension, const function<T ()>& filler)
                    : fRep_ (make_shared<IRep_> ())
                {
                    for (size_t i = 0; i < dimension; ++i) {
                        fRep_->get ()->Append (filler ());
                    }
                }
                template <typename T>
                template <typename CONTAINER_OF_T, typename ENABLE_IF>
                Vector<T>::Vector (const CONTAINER_OF_T& c)
                    : fRep_ (make_shared<IRep_> (c))
                {
                }
#if 0
                template <typename T>
                void    Vector<T>::Fill (T value)
                {
                    for (size_t i = 0; i < dimension; ++i) {
                        fRep_->get ()->SetAt (i, value);
                    }
                }
                template <typename T>
                void    Vector<T>::Fill (function<T ()> filler)
                {
                    for (size_t i = 0; i < dimension; ++i) {
                        fRep_->get ()->SetAt (i, filler ());
                    }
                }
#endif
                template <typename T>
                inline size_t Vector<T>::GetDimension () const
                {
                    return Rep_->cget ()->size ();
                }
                template <typename T>
                Vector<T> Vector<T>::Transform (function<T (T)> f) const
                {
                    vector<T> tmp;
                    size_t    dimension = GetDimension ();
                    tmp.reserve (dimension);
                    for (size_t i = 0; i < dimension; ++i) {
                        tmp.push_back (f (GetAt (i)));
                    }
                    return tmp;
                }
                template <typename T>
                T Vector<T>::Norm () const
                {
                    T result{};
                    fRep_->cget ()->Apply ([&](T v) { result += v * v; });
                    return sqrt (v);
                }
                template <typename T>
                inline Containers::Sequence<T> Vector<T>::GetItems () const
                {
                    return fRep_->cget ()->fData;
                }
                template <typename T>
                inline T Vector<T>::GetAt (size_t i) const
                {
                    return fRep_->cget ()->fData[i];
                }
                template <typename T>
                inline T Vector<T>::operator[] (size_t i) const
                {
                    return GetAt (i);
                }
            }
        }
    }
}
#endif /*_Stroika_Foundation_Math_LinearAlgebra_Vector_inl_*/
