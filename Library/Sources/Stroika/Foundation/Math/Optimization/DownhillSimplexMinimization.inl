/*
* Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
*/
#ifndef _Stroika_Foundation_Math_Optimization_DownhillSimplexMinimization_inl_
#define _Stroika_Foundation_Math_Optimization_DownhillSimplexMinimization_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Characters/Format.h"
#include "../../Characters/ToString.h"
#include "../../Debug/Trace.h"

#include "../LinearAlgebra/Matrix.h"
#include "../LinearAlgebra/Vector.h"

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define Stroika_Foundation_Math_Optimization_DownhillSimplexMinimization_USE_NOISY_TRACE_IN_THIS_MODULE_ 1

namespace Stroika::Foundation::Math::Optimization::DownhillSimplexMinimization {

    /*
     ********************************************************************************
     ****************** DownhillSimplexMinimization::Options ************************
     ********************************************************************************
     */
    template <typename FLOAT_TYPE>
    Characters::String Options<FLOAT_TYPE>::ToString () const
    {
        Characters::StringBuilder sb;
        sb += L"{";
        if (fMaxIterations) {
            sb += L"Max-Iterations: " + Characters::ToString (fMaxIterations) + L",";
        }
        if (fNoImprovementThreshold) {
            sb += L"No-Improvement-Threshold: " + Characters::ToString (fNoImprovementThreshold) + L",";
        }
        sb += L"}";
        return sb.str ();
    }

    /*
     ********************************************************************************
     ****************** DownhillSimplexMinimization::Results ************************
     ********************************************************************************
     */
    template <typename FLOAT_TYPE>
    Characters::String Results<FLOAT_TYPE>::ToString () const
    {
        Characters::StringBuilder sb;
        sb += L"{";
        sb += L"Optimized-Parameters: " + Characters::ToString (fOptimizedParameters) + L",";
        sb += L"Score: " + Characters::ToString (fScore) + L",";
        sb += L"Iteration-Count: " + Characters::Format (L"%d", fIterationCount) + L",";
        sb += L"}";
        return sb.str ();
    }

    /*
     ********************************************************************************
     ******************** DownhillSimplexMinimization::Run **************************
     ********************************************************************************
     */
    namespace PRIVATE_ {
        using Containers::Sequence;
        using namespace LinearAlgebra;

        // Translated by hand from https://github.com/fchollet/nelder-mead/blob/master/nelder_mead.py - LGP 2017-02-20
        template <typename FLOAT_TYPE>
        Results<FLOAT_TYPE> nelder_mead_by_fchollet (
            const TargetFunction<FLOAT_TYPE>&        f,
            const LinearAlgebra::Vector<FLOAT_TYPE>& x_start,
            FLOAT_TYPE                               step            = 0.1,
            FLOAT_TYPE                               no_improve_thr  = 10e-6,
            unsigned int                             no_improv_break = 10,
            unsigned int                             max_iter        = 0,
            FLOAT_TYPE                               alpha           = 1,
            FLOAT_TYPE                               gamma           = 2,
            FLOAT_TYPE                               rho             = -0.5,
            FLOAT_TYPE                               sigma           = 0.5)
        {
            // init
            size_t       dim       = x_start.GetDimension ();
            FLOAT_TYPE   prev_best = f (x_start.GetItems ());
            unsigned int no_improv = 0;
            struct PartialResultType_ {
                Sequence<FLOAT_TYPE> fResults;
                FLOAT_TYPE           fScore;
            };

            // @todo - fix - temporarily must use vector due to need to call sort which requires random access iterators
            vector<PartialResultType_> res{PartialResultType_{x_start.GetItems (), prev_best}};
            for (size_t i = 0; i != dim; ++i) {
                Vector<FLOAT_TYPE> x = x_start;
                x[i] += step;
                FLOAT_TYPE score{f (x.GetItems ())};
                res.push_back (PartialResultType_{x.GetItems (), score});
            }

            // Simplex iteration
            unsigned int iters = 0;
            while (true) {
                sort (res.begin (), res.end (), [] (auto l, auto r) { return l.fScore < r.fScore; });
                FLOAT_TYPE best = res[0].fScore;

                // break after max_iter
                if (max_iter and iters >= max_iter) {
                    return Results<FLOAT_TYPE>{res[0].fResults, res[0].fScore, iters};
                }
                iters += 1;

#if Stroika_Foundation_Math_Optimization_DownhillSimplexMinimization_USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace (L"...best so far (iteration %d): %s", iters, Characters::ToString (best).c_str ());
#endif
                // break after no_improv_break iterations with no improvement
                if (best < prev_best - no_improve_thr) {
                    no_improv = 0;
                    prev_best = best;
                }
                else {
                    no_improv += 1;
                }
                if (no_improv >= no_improv_break) {
                    return Results<FLOAT_TYPE>{res[0].fResults, res[0].fScore, iters};
                }

                // Centroid
                Vector<FLOAT_TYPE> x0{dim, 0.0};
                {
                    auto removeLast = [] (decltype (res) v) {
                        decltype (v) tmp;
                        for (auto i = v.begin (); i != v.end () and i + 1 != v.end (); ++i) {
                            tmp.push_back (*i);
                        }
                        return tmp;
                    };
                    for (PartialResultType_ tup : removeLast (res)) {
                        for (size_t i = 0; i < x0.GetDimension (); ++i) {
                            FLOAT_TYPE c = Sequence<FLOAT_TYPE>{tup.fResults}[i];
                            x0[i] += c / (res.size () - 1);
                        }
                    }
                }

                // Reflection
                Vector<FLOAT_TYPE> xr     = x0 + alpha * (x0 - Vector<FLOAT_TYPE>{res[res.size () - 1].fResults});
                FLOAT_TYPE         rscore = f (xr.GetItems ());
                {
                    if (res[0].fScore <= rscore and rscore < res[res.size () - 2].fScore) {
                        res[res.size () - 1] = PartialResultType_{xr.GetItems (), rscore};
                        continue;
                    }
                }

                // Expansion
                {
                    if (rscore < res[0].fScore) {
                        Vector<FLOAT_TYPE> xe     = x0 + gamma * (x0 - Vector<FLOAT_TYPE>{res[res.size () - 1].fResults});
                        FLOAT_TYPE         escore = f (xe.GetItems ());
                        if (escore < rscore) {
                            res[res.size () - 1] = PartialResultType_{xe.GetItems (), escore};
                        }
                        else {
                            res[res.size () - 1] = PartialResultType_{xr.GetItems (), rscore};
                        }
                        continue;
                    }
                }

                // Contraction
                {
                    Vector<FLOAT_TYPE> xc     = x0 + rho * (x0 - Vector<FLOAT_TYPE>{res[res.size () - 1].fResults});
                    FLOAT_TYPE         cscore = f (xc.GetItems ());
                    if (cscore < res[res.size () - 1].fScore) {
                        res[res.size () - 1] = PartialResultType_{xc.GetItems (), cscore};
                    }
                    continue;
                }

                // Reduction
                {
                    Vector<FLOAT_TYPE>         x1 = res[0].fResults;
                    vector<PartialResultType_> nres;
                    for (PartialResultType_ tup : res) {
                        Vector<FLOAT_TYPE> redx  = x1 + sigma * (Vector<FLOAT_TYPE>{tup.fResults} - x1);
                        FLOAT_TYPE         score = f (redx.GetItems ());
                        res.push_back (PartialResultType_{redx.GetItems (), score});
                    }
                    res = nres;
                }
            }
            AssertNotReached ();
            return Results<FLOAT_TYPE>{};
        };
    }
    template <typename FLOAT_TYPE>
    Results<FLOAT_TYPE> Run (const TargetFunction<FLOAT_TYPE>& function2Minimize, const Sequence<FLOAT_TYPE>& initialValues, const Options<FLOAT_TYPE>& options)
    {
#if Stroika_Foundation_Math_Optimization_DownhillSimplexMinimization_USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{L"Optimization::DownhillSimplexMinimization::Run", L"initialValues=%s, options=%s", Characters::ToString (initialValues).c_str (), Characters::ToString (options).c_str ()};
#endif
        Results<FLOAT_TYPE> results{};
        FLOAT_TYPE          step            = 0.1;
        FLOAT_TYPE          no_improve_thr  = options.fNoImprovementThreshold.value_or (10e-6);
        unsigned int        no_improv_break = 10;
        unsigned int        max_iter        = options.fMaxIterations.value_or (0);
        results                             = PRIVATE_::nelder_mead_by_fchollet<FLOAT_TYPE> (function2Minimize, initialValues, step, no_improve_thr, no_improv_break, max_iter);
#if Stroika_Foundation_Math_Optimization_DownhillSimplexMinimization_USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (L"returns: %s", Characters::ToString (results).c_str ());
#endif
        return results;
    }

}

#endif /*_Stroika_Foundation_Math_Optimization_DownhillSimplexMinimization_inl_*/
