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

namespace Stroika {
    namespace Foundation {
        namespace Math {
            namespace LinearAlgebra {
                /*
                 ********************************************************************************
                 ******************************** Matrix<T>::IRep_ ******************************
                 ********************************************************************************
                 */
                template <typename T>
                class Matrix<T>::IRep_ {
                public:
                    DimensionType           fDimensions;
                    Containers::Sequence<T> fData; // row*nCols + col is addressing scheme

                    T GetAt (size_t row, size_t col)
                    {
                        Require (row < fDimensions.fRows);
                        Require (col < fDimensions.fColumns);
                        return fData[row * fDimensions.fColumns + col];
                    }
                };

                /*
                 ********************************************************************************
                 ********************************** Matrix<T> ***********************************
                 ********************************************************************************
                 */
                template <typename T>
                inline Matrix<T>::Matrix (const DimensionType& dimensions)
                    : Matrix (dimension, 0)
                {
                }
                template <typename T>
                Matrix<T>::Matrix (const DimensionType& dimensions, Configuration::ArgByValueType<T> fillValue)
                    : fRep_ (make_shared<IRep_> (dimensions))
                {
                    for (size_t r = 0; r < dimension.fRows; ++r) {
                        for (size_t c = 0; c < dimension.fColumns; ++c) {
                            fRep_->get ()->Append (fillValue);
                        }
                    }
                }
                template <typename T>
                Matrix<T>::Matrix (const DimensionType& dimensions, const function<T ()>& filler)
                    : fRep_ (make_shared<IRep_> (dimensions))
                {
                    for (size_t r = 0; r < dimension.fRows; ++r) {
                        for (size_t c = 0; c < dimension.fColumns; ++c) {
                            fRep_->get ()->Append (filler ());
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
                    return fRep_->cget ()->fDimensions;
                }
                template <typename T>
                Containers::Sequence<Vector<T>> Matrix<T>::GetRows () const
                {
                    Containers::Sequence<Vector<T>> result;
                    DimensionType                   dim = GetDimensions ();
                    for (size_t r = 0; r < dim.fRows; ++r) {
                        vector<T> row;
                        for (size_t c = 0; c < dim.fColumns; ++c) {
                            row.push_back (fRep_->cget ()->GetAt (r, c));
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
                            col.push_back (fRep_->cget ()->GetAt (r, c));
                        }
                        result += Vector<T>{col};
                    }
                    return result;
                }
                template <typename T>
                inline T Matrix<T>::GetAt (size_t r, size_t c) const
                {
                    return fRep_->cget ()->GetAt (r, c);
                }
                template <typename T>
                inline typename Matrix<T>::TemporaryRowReference_ Matrix<T>::operator[] (size_t row) const
                {
                    return TMP_{*this, row};
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
