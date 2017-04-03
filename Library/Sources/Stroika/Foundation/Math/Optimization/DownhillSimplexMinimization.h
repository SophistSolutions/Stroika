/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Math_Optimization_DownhillSimplexMinimization_h_
#define _Stroika_Foundation_Math_Optimization_DownhillSimplexMinimization_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Characters/String.h"
#include "../../Containers/Sequence.h"
#include "../../Memory/Optional.h"
#include "../../Traversal/Iterable.h"

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
            namespace Optimization {
                namespace DownhillSimplexMinimization {

                    using Characters::String;
                    using Containers::Sequence;
                    using Memory::Optional;

                    /*
                     *  This is a list of parameters to be minimized for the given function
                     */
                    template <typename FLOAT_TYPE>
                    using MinimizationParametersType = Sequence<FLOAT_TYPE>;

                    template <typename FLOAT_TYPE>
                    using TargetFunction = function<FLOAT_TYPE (const MinimizationParametersType<FLOAT_TYPE>&)>;

                    template <typename FLOAT_TYPE>
                    struct Options {
                        Optional<unsigned int> fMaxIterations;
                        Optional<FLOAT_TYPE>   fNoImprovementThreshold;

                        nonvirtual Characters::String ToString () const;
                    };

                    template <typename FLOAT_TYPE>
                    struct Results {
                        MinimizationParametersType<FLOAT_TYPE> fOptimizedParameters;
                        FLOAT_TYPE                             fScore{};
                        unsigned int                           fIterationCount{};

                        nonvirtual Characters::String ToString () const;
                    };

                    /**
                     *  Take an argument function with argument 'initialValues' and try many values to find where the targetFunction is least (ie minimize it).
                     *
                     *  If the function is naturally periodic - you can add checks in the minimization function for the desired target range and produce LARGE answers there
                     *  so they wont appear as minima of the 'targetFunction'.
                     *
                     *      Reference: https://github.com/fchollet/nelder-mead/blob/master/nelder_mead.py
                     *      Reference: https://en.wikipedia.org/wiki/Nelder%E2%80%93Mead_method
                     *      Reference: http://www.aip.de/groups/soe/local/numres/bookcpdf/c10-4.pdf
                     *      Reference: https://www.mathworks.com/matlabcentral/mlc-downloads/downloads/submissions/20398/versions/1/previews/fminsearch2.m/index.html
                     *
                     *  \par Example Usage
                     *      \code
                     *          TargetFunction<double> f = [](const Sequence<double>& x) {
                     *              double d = x[0];
                     *               if (d < 0 or d >= Math::kPi) {     // avoid falling off ends of ranges - periodic function
                     *                  return 100.0;
                     *               }
                     *               return -std::cos (d);
                     *           };
                     *           DownhillSimplexMinimization::Results<double> result = DownhillSimplexMinimization::Run (f, {.1});
                     *           VerifyTestResult (Math::NearlyEquals (result.fOptimizedParameters[0], 0.0, 1e-10));
                     *      \endcode
                     */
                    template <typename FLOAT_TYPE>
                    Results<FLOAT_TYPE> Run (const TargetFunction<FLOAT_TYPE>& function2Minimize, const MinimizationParametersType<FLOAT_TYPE>& initialValues, const Options<FLOAT_TYPE>& options = Options<FLOAT_TYPE>{});
                }
            }
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "DownhillSimplexMinimization.inl"

#endif /*_Stroika_Foundation_Math_Optimization_DownhillSimplexMinimization_h_*/
