/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Math_LinearAlgebra_Matrix_h_
#define _Stroika_Foundation_Math_LinearAlgebra_Matrix_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Containers/Sequence.h"

#include "Vector.h"

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
                class Matrix {
                public:
                    struct DimensionType {
                        size_t fRows;
                        size_t fColumns;
                    };

                public:
                    Matrix (const DimensionType& dimensions);
                    Matrix (size_t rows, size_t columns);

                public:
                    nonvirtual DimensionType GetDimensions () const;

                public:
                    nonvirtual Containers::Sequence<Vector<T>> GetRows () const;

                public:
                    nonvirtual Containers::Sequence<Vector<T>> GetColumns () const;

                public:
                    nonvirtual T GetAt (size_t r, size_t c) const;
                    // nonvirtual T operator[] (size_t r, size_t c) const;

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
#include "Matrix.inl"

#endif /*_Stroika_Foundation_Math_LinearAlgebra_Matrix_h_*/
