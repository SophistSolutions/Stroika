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

                        // Translated by hand from https://github.com/fchollet/nelder-mead/blob/master/nelder_mead.py - LGP 2017-02-20
                        auto nelder_mead = [](
                            const TargetFunction<FLOAT_TYPE>&             f,
                            const MinimizationParametersType<FLOAT_TYPE>& initialValues,
                            FLOAT_TYPE                                    step            = 0.1,
                            FLOAT_TYPE                                    no_improve_thr  = 10e-6,
                            unsigned int                                  no_improv_break = 10,
                            unsigned int                                  max_iter        = 0,
                            FLOAT_TYPE                                    alpha           = 1,
                            FLOAT_TYPE                                    gamma           = 2,
                            FLOAT_TYPE                                    rho             = -0.5,
                            FLOAT_TYPE                                    sigma           = 0.5) -> pair<MinimizationParametersType<FLOAT_TYPE>, FLOAT_TYPE> {

                            using namespace LinearAlgebra;

                            // init
                            size_t       dim       = x_start.size ();
                            FLOAT_TYPE   prev_best = f (x_start);
                            unsigned int no_improv = 0;
                            //Matrix res ()

                            //tmphack
                            return pair<MinimizationParametersType<FLOAT_TYPE>, FLOAT_TYPE>{};
                        };

                        return results;
                    }
                }
            }
        }
    }
}
#endif /*_Stroika_Foundation_Math_Optimization_DownhillSimplexMinimization_inl_*/
