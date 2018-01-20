/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Math_LinearAlgebra_Matrix_h_
#define _Stroika_Foundation_Math_LinearAlgebra_Matrix_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Characters/String.h"
#include "../../Containers/Sequence.h"

#include "Vector.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
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
                class Matrix {
                public:
                    struct DimensionType {
                        size_t fRows;
                        size_t fColumns;
                    };

                public:
                    /**
                     *  All constructors require dimensions of the matrix (rows/columns).
                     *
                     *  The one with no other arguments, initializes to zeros.
                     *
                     *  The last 2 fill, but either with a hardwired value, or with a computed value (function called foreach row and within a row each column)
                     */
                    Matrix (const DimensionType& dimensions);
                    Matrix (size_t rows, size_t columns);
                    Matrix (const DimensionType& dimensions, Configuration::ArgByValueType<T> fillValue);
                    Matrix (size_t rows, size_t columns, Configuration::ArgByValueType<T> fillValue);
                    Matrix (const DimensionType& dimensions, const function<T ()>& filler);

                public:
                    static Matrix<T> Identity (const DimensionType& dimensions);

                public:
                    nonvirtual DimensionType GetDimensions () const;

                public:
                    nonvirtual Containers::Sequence<Vector<T>> GetRows () const;

                public:
                    nonvirtual Containers::Sequence<Vector<T>> GetColumns () const;

                public:
                    nonvirtual T GetAt (size_t r, size_t c) const;

                public:
                    nonvirtual void SetAt (size_t r, size_t c, T v);

                private:
                    struct ReadOnlyTemporaryRowReference_ {
                        const Matrix<T>& fMatrix;
                        size_t           fRow;
                        T                operator[] (size_t column) const
                        {
                            return fMatrix.GetAt (fRow, column);
                        }
                    };

                public:
                    /**
                     *  EXAMPLE USAGE:
                     *      Matrix<double> m (2,2);
                     *      Assert (m[1][1] == 0);
                     */
                    nonvirtual const ReadOnlyTemporaryRowReference_ operator[] (size_t row) const;

                public:
                    nonvirtual Characters::String ToString () const;

                private:
                    class Rep_;

                private:
                    Memory::SharedByValue<Memory::SharedByValue_Traits<Rep_>> fRep_;
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
#include "Matrix.inl"

#endif /*_Stroika_Foundation_Math_LinearAlgebra_Matrix_h_*/
