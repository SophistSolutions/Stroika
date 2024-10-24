/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include <vector>

#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"

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
        : Vector{dimension, 0}
    {
    }
    template <typename T>
    Vector<T>::Vector (size_t dimension, Configuration::ArgByValueType<T> fillValue)
        : fRep_{make_shared<IRep_> ()}
    {
        for (size_t i = 0; i < dimension; ++i) {
            fRep_.rwget ()->fData.Append (fillValue);
        }
    }
    template <typename T>
    Vector<T>::Vector (size_t dimension, const function<T ()>& filler)
        : fRep_{make_shared<IRep_> ()}
    {
        for (size_t i = 0; i < dimension; ++i) {
            fRep_.rwget ()->fData.Append (filler ());
        }
    }
    template <typename T>
    template <Traversal::IIterableOf<T> CONTAINER_OF_T>
    Vector<T>::Vector (const CONTAINER_OF_T& c)
        : fRep_{make_shared<IRep_> ()}
    {
        fRep_->fData = Containers::Sequence<T>{c};
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
    Vector<T> Vector<T>::Transform (const function<T (T)>& f) const
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
        fRep_.cget ()->Apply ([&] (T v) { result += v * v; });
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
        fRep_.rwget ()->fData.SetAt (i, v);
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
        sb << "["sv;
        for (const T& i : fRep_.cget ()->fData) {
            sb << i << ", "sv;
        }
        sb << "]"sv;
        return sb;
    }

}
