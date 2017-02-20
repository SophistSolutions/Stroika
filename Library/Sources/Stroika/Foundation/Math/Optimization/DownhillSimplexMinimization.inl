/*
* Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
*/
#ifndef _Stroika_Foundation_Math_Optimization_DownhillSimplexMinimization_inl_
#define _Stroika_Foundation_Math_Optimization_DownhillSimplexMinimization_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../LinearAlgebra/Matrix.h"
#include "../LinearAlgebra/Vector.h"

namespace Stroika {
    namespace Foundation {
        namespace Math {
            namespace Optimization {
                namespace DownhillSimplexMinimization {

                    /*
                     ********************************************************************************
                     ******************** DownhillSimplexMinimization::Run **************************
                     ********************************************************************************
                     */
                    template <typename FLOAT_TYPE>
                    Results<FLOAT_TYPE> Run (const MinimizationParametersType<FLOAT_TYPE>& initialValues, const TargetFunction<FLOAT_TYPE>& function2Minimize, const Options& options)
                    {
                        Results<FLOAT_TYPE> results{};

                        return results;
                    }
                }
            }
        }
    }
}
#endif /*_Stroika_Foundation_Math_Optimization_DownhillSimplexMinimization_inl_*/
