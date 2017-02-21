/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Math_Optimization_DownhillSimplexMinimization_h_
#define _Stroika_Foundation_Math_Optimization_DownhillSimplexMinimization_h_ 1

#include "../../StroikaPreComp.h"

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

                    /*
                     *  This is a list of parameters to be minimized for the given function
                     */
                    template <typename FLOAT_TYPE>
                    using MinimizationParametersType = Traversal::Iterable<FLOAT_TYPE>;

                    template <typename FLOAT_TYPE>
                    using TargetFunction = function<FLOAT_TYPE (const MinimizationParametersType<FLOAT_TYPE>&)>;

                    struct Options {
                        Memory::Optional<unsigned int> fMaxIterations;
                    };

                    template <typename FLOAT_TYPE>
                    struct Results {
                        MinimizationParametersType<FLOAT_TYPE> fMinimizedParameters;
                        FLOAT_TYPE                             fScore{};
                        unsigned int                           fIterationCount{};
                    };

                    /**
                     *      Reference: https://github.com/fchollet/nelder-mead/blob/master/nelder_mead.py
                     *      Reference: https://en.wikipedia.org/wiki/Nelder%E2%80%93Mead_method
                     *      Reference http://www.aip.de/groups/soe/local/numres/bookcpdf/c10-4.pdf
                     *      Reference: https://www.mathworks.com/matlabcentral/mlc-downloads/downloads/submissions/20398/versions/1/previews/fminsearch2.m/index.html
                     */
                    template <typename FLOAT_TYPE>
                    Results<FLOAT_TYPE> Run (const TargetFunction<FLOAT_TYPE>& function2Minimize, const MinimizationParametersType<FLOAT_TYPE>& initialValues, const Options& options = Options{});
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
