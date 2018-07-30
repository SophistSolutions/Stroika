/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Math_LinearAlgebra_Vector_inl_
#define _Stroika_Foundation_Math_LinearAlgebra_Vector_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include <vector>

#include "../../Characters/StringBuilder.h"
#include "../../Characters/ToString.h"

namespace Stroika::Foundation::Math::LinearAlgebra {

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
            fRep_.get ()->fData.Append (fillValue);
        }
    }
    template <typename T>
    Vector<T>::Vector (size_t dimension, const function<T ()>& filler)
        : fRep_ (make_shared<IRep_> ())
    {
        for (size_t i = 0; i < dimension; ++i) {
            fRep_.get ()->fData.Append (filler ());
        }
    }
    template <typename T>
    template <typename CONTAINER_OF_T, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_T, T>>*>
    Vector<T>::Vector (const CONTAINER_OF_T& c)
        : fRep_ (make_shared<IRep_> ())
    {
        fRep_->fData = c;
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
        return fRep_.cget ()->fData.size ();
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
        fRep_.cget ()->Apply ([&](T v) { result += v * v; });
        return sqrt (result);
    }
    template <typename T>
    inline Containers::Sequence<T> Vector<T>::GetItems () const
    {
        return fRep_.cget ()->fData;
    }
    template <typename T>
    inline T Vector<T>::GetAt (size_t i) const
    {
        return fRep_.cget ()->fData[i];
    }
#if Stroika_Foundation_Math_LinearAlgebra_Vector_ALLOW_MUTATION
    template <typename T>
    void Vector<T>::SetAt (size_t i, Configuration::ArgByValueType<T> v)
    {
#if 1
        fRep_.get ()->fData.SetAt (i, v);
#else
        fRep_.get ()->fData[i] = v;
#endif
    }
#endif
    template <typename T>
    inline T Vector<T>::operator[] (size_t i) const
    {
        return GetAt (i);
    }
    template <typename T>
    Characters::String Vector<T>::ToString () const
    {
        Characters::StringBuilder sb;
        sb += L"[";
        for (T i : fRep_.cget ()->fData) {
            sb += Characters::ToString (i) + L", ";
        }
        sb += L"]";
        return sb.str ();
    }

}

#endif /*_Stroika_Foundation_Math_LinearAlgebra_Vector_inl_*/
