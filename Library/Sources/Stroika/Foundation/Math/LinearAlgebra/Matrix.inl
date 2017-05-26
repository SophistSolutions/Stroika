/*
* Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
*/
#ifndef _Stroika_Foundation_Math_LinearAlgebra_Matrix_inl_
#define _Stroika_Foundation_Math_LinearAlgebra_Matrix_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <vector>

#include "../../Characters/StringBuilder.h"
#include "../../Characters/ToString.h"
#include "../../Debug/AssertExternallySynchronizedLock.h"

namespace Stroika {
    namespace Foundation {
        namespace Math {
            namespace LinearAlgebra {
                /*
                 ********************************************************************************
                 ******************************** Matrix<T>::Rep_ *******************************
                 ********************************************************************************
                 */
                template <typename T>
                class Matrix<T>::Rep_ : private Debug::AssertExternallySynchronizedLock {
                public:
                    Rep_ (const DimensionType& dimensions)
                        : fDimensions (dimensions)
                    {
                    }
                    DimensionType fDimensions;
                    // nb: use vector<> because for debug builds - big difference in speed  - and hidden anyhow
                    vector<T> fData; // row*nCols + col is addressing scheme

                    T GetAt (size_t row, size_t col) const
                    {
                        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                        Require (row < fDimensions.fRows);
                        Require (col < fDimensions.fColumns);
                        return fData[row * fDimensions.fColumns + col];
                    }
                    void SetAt (size_t row, size_t col, T value)
                    {
                        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                        Require (row < fDimensions.fRows);
                        Require (col < fDimensions.fColumns);
						//fData.SetAt (row * fDimensions.fColumns + col, value);
						fData[row * fDimensions.fColumns + col] = value;
					}
                };

                /*
                 ********************************************************************************
                 ********************************** Matrix<T> ***********************************
                 ********************************************************************************
                 */
                template <typename T>
                inline Matrix<T>::Matrix (const DimensionType& dimensions)
                    : Matrix (dimensions, T{0})
                {
                }
                template <typename T>
                inline Matrix<T>::Matrix (size_t rows, size_t columns)
                    : Matrix (DimensionType{rows, columns})
                {
                }
                template <typename T>
                Matrix<T>::Matrix (const DimensionType& dimensions, Configuration::ArgByValueType<T> fillValue)
                    : fRep_ (make_shared<Rep_> (dimensions))
                {
                    lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{*fRep_.get ()};
                    for (size_t r = 0; r < dimensions.fRows; ++r) {
                        for (size_t c = 0; c < dimensions.fColumns; ++c) {
                            fRep_.get ()->fData.push_back (fillValue);
                        }
                    }
                }
                template <typename T>
                inline Matrix<T>::Matrix (size_t rows, size_t columns, Configuration::ArgByValueType<T> fillValue)
                    : Matrix (DimensionType{rows, columns}, fillValue)
                {
                }
                template <typename T>
                Matrix<T>::Matrix (const DimensionType& dimensions, const function<T ()>& filler)
                    : fRep_ (make_shared<Rep_> (dimensions))
                {
                    lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{*fRep_.get ()};
                    for (size_t r = 0; r < dimensions.fRows; ++r) {
                        for (size_t c = 0; c < dimensions.fColumns; ++c) {
                            fRep_.get ()->fData.push_back (filler ());
                        }
                    }
                }
                template <typename T>
                Matrix<T> Matrix<T>::Identity (const DimensionType& dimensions)
                {
                    size_t row = 0;
                    size_t col = 0;
                    return Matrix{dimensions, [&]() { T result = (row == col) ? 1 : 0; ++col; if (col > dimensions.fColumns) { col = 0; row++; } }};
                }
                template <typename T>
                inline auto Matrix<T>::GetDimensions () const -> DimensionType
                {
                    return fRep_.cget ()->fDimensions;
                }
                template <typename T>
                Containers::Sequence<Vector<T>> Matrix<T>::GetRows () const
                {
                    Containers::Sequence<Vector<T>> result;
                    DimensionType                   dim = GetDimensions ();
                    for (size_t r = 0; r < dim.fRows; ++r) {
                        vector<T> row;
                        for (size_t c = 0; c < dim.fColumns; ++c) {
                            row.push_back (fRep_.cget ()->GetAt (r, c));
                        }
                        result += Vector<T>{row};
                    }
                    return result;
                }
                template <typename T>
                Containers::Sequence<Vector<T>> Matrix<T>::GetColumns () const
                {
                    Containers::Sequence<Vector<T>> result;
                    DimensionType                   dim = GetDimensions ();
                    for (size_t c = 0; c < dim.fColumns; ++c) {
                        vector<T> col;
                        for (size_t r = 0; r < dim.fRows; ++r) {
                            col.push_back (fRep_.cget ()->GetAt (r, c));
                        }
                        result += Vector<T>{col};
                    }
                    return result;
                }
                template <typename T>
                inline T Matrix<T>::GetAt (size_t r, size_t c) const
                {
                    return fRep_.cget ()->GetAt (r, c);
                }
                template <typename T>
                inline void Matrix<T>::SetAt (size_t r, size_t c, T v)
                {
                    fRep_.get ()->SetAt (r, c, v);
                }
                template <typename T>
                inline typename Matrix<T>::TemporaryRowReference_ Matrix<T>::operator[] (size_t row) const
                {
                    return TemporaryRowReference_{*this, row};
                }
                template <typename T>
                Characters::String Matrix<T>::ToString () const
                {
                    Characters::StringBuilder sb;
                    sb += L"[";
                    for (size_t row = 0; row < GetDimensions ().fRows; ++row) {
                        sb += L"[";
                        for (size_t col = 0; col < GetDimensions ().fColumns; ++col) {
                            sb += Characters::ToString (GetAt (row, col)) + L", ";
                        }
                        sb += L"],";
                    }
                    sb += L"]";
                    return sb.str ();
                }
            }
        }
    }
}
#endif /*_Stroika_Foundation_Math_LinearAlgebra_Matrix_inl_*/
