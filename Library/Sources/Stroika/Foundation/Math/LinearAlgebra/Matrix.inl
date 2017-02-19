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
            }
        }
    }
}
#endif /*_Stroika_Foundation_Math_LinearAlgebra_Matrix_inl_*/
