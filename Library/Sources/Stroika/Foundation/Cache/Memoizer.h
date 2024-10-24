/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_Memoizer_h_
#define _Stroika_Foundation_Cache_Memoizer_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <optional>
#include <vector>

#include "Stroika/Foundation/Cache/LRUCache.h"
#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Configuration/TypeHints.h"
#include "Stroika/Foundation/Containers/Mapping.h"

/**
 *  \note Code-Status:    <a href="Code-Status.md#Beta">Beta</a>
 *
 * TODO:
 *      @todo   maybe allow passing in Cache object as CTOR parameter as a way to specify the hash function etc (for LRUCache with hash)
 *
 *      @todo   Investigate if better arg order for template or instantiation guide might reduce number of explicit 
 *              args needed for template
 */

namespace Stroika::Foundation::Cache {

#if qCompilerAndStdLib_template_template_argument_as_different_template_paramters_Buggy
    namespace MemoizerSupport {
        template <typename T1, typename T2>
        using DEFAULT_CACHE_BWA_ = LRUCache<T1, T2>;
    }
#endif

    /**
     * \brief Cache the results of expensive computations transparently
     *
     *  @see https://en.wikipedia.org/wiki/Memoization
     *
     *  TODO:
     *      o   @todo   Asked https://softwareengineering.stackexchange.com/questions/377020/c-templates-combining-deduction-with-default-template-arguments 
     *                  to see how to improve
     *
     *      o   @todo   maybe update https://softwareengineering.stackexchange.com/questions/375257/how-can-i-aggregate-this-large-data-set-to-reduce-the-overhead-of-calculating-th/375303#375303 with this... if/when I get it working well...
     *
     *  \note   Memoizer works well wtih LRUCache, SynchronizedLRUCache, TimedCache, or SynchronizedTimeCache. But
     *          it does NOT work with CallerStalenessCache, because that cache requires the caller to specify an allowed staleness on each
     *          call.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md">Same as (worse case of) underlying CACHE template argument, and argument function. Since the function will typically be fully reentrant, this comes down to the re-entrancy of the argument Cache. Used with SynchronizedLRUCache and a typical function, for example, this is fully re-entrant</a>
     */
    template <typename RESULT, template <typename, typename> class CACHE = LRUCache, typename... ARGS>
    class Memoizer {
    public:
        /**
         *  \note see Tests use of qCompilerAndStdLib_template_template_argument_as_different_template_paramters_Buggy if you get the message
         *        "template template argument has different template parameters than its corresponding template template parameter"
         *
         *  \par Example Usage
         *      \code
         *          unsigned int                      totalCallsCount{};
         *          Memoizer<int, LRUCache, int, int> memoizer{[&totalCallsCount](int a, int b) { ++totalCallsCount;  return a + b; }};
         *          EXPECT_TRUE (memoizer (1, 1) == 2 and totalCallsCount == 1);
         *          EXPECT_TRUE (memoizer (1, 1) == 2 and totalCallsCount == 1);
         *      \endcode
         */
        Memoizer (const function<RESULT (ARGS...)>& f, CACHE<tuple<ARGS...>, RESULT>&& cache = CACHE<tuple<ARGS...>, RESULT>{});
        Memoizer (Memoizer&& from) noexcept = default;
        Memoizer (const Memoizer& from)     = default;

    public:
        nonvirtual Memoizer& operator= (Memoizer&& rhs) noexcept = default;
        nonvirtual Memoizer& operator= (const Memoizer& rhs)     = default;

    public:
        /**
         *  \note this function is not const, because it modifies the state of the object/cache.
         */
        nonvirtual RESULT operator() (ARGS... args);

    private:
        function<RESULT (ARGS...)>    fFunction_;
        CACHE<tuple<ARGS...>, RESULT> fCache_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Memoizer.inl"

#endif /*_Stroika_Foundation_Cache_Memoizer_h_*/
