/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_Memoizer_h_
#define _Stroika_Foundation_Cache_Memoizer_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <optional>
#include <vector>

#include "Stroika/Foundation/Cache/LRUCache.h"
#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/Concepts.h"
#include "Stroika/Foundation/Common/TypeHints.h"
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

#if qCompilerAndStdLib_template_template_argument_as_different_template_paramters_Buggy || qCompilerAndStdLib_template_template_auto_deduced_Buggy
#define qStroika_template_template_BWA(...) __VA_ARGS__, typename...
#else
#define qStroika_template_template_BWA(...) __VA_ARGS__
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
     *  \par Example Usage
     *      \code
     *          unsigned int                      totalCallsCount{};
     *          Memoizer<int, LRUCache, int, int> memoizer{[&totalCallsCount](int a, int b) { ++totalCallsCount;  return a + b; }};
     *          EXPECT_TRUE (memoizer (1, 1) == 2 and totalCallsCount == 1);
     *          EXPECT_TRUE (memoizer (1, 1) == 2 and totalCallsCount == 1);
     *      \endcode
     * 
     *  \see    Factory::Memoizer::Make () for more simple to use examples.
     *
     *  \note   Memoizer works well with LRUCache, or TimedCache.
     * 
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md">Same as (worse case of) underlying CACHE template argument, and argument function. Since the function will typically be fully reentrant, this comes down to the re-entrancy of the argument Cache.</a>
     */
    template <typename RESULT, template <qStroika_template_template_BWA (typename, typename)> class CACHE = LRUCache, typename... ARGS>
    class Memoizer {
    public:
        /**
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

    namespace Factory::Memoizer {

        /**
         *  @brief Factory function to make a memoizer out of any argument function.
         * 
         *  \par Example Usage (simple):
         * 
         *  \code
         *      unsigned int totalCallsCount{};
         *      Memoizer         memoizer = Cache::Factory::Memoizer::Make ([&totalCallsCount] (int a, int b) {
         *          totalCallsCount++;
         *          return a + b;
         *      });
         *      EXPECT_TRUE (memoizer (1, 1) == 2 and totalCallsCount == 1);
         *      EXPECT_TRUE (memoizer (1, 1) == 2 and totalCallsCount == 1);
         *  \endcode
         *
         *  \par Example Usage (force using LRUCache):
         * 
         *  \code
         *      unsigned int totalCallsCount{};
         *      Memoizer         memoizer = Cache::Factory::Memoizer::Make<Cache::LRUCache> ([&totalCallsCount] (int a, int b) {
         *          totalCallsCount++;
         *          return a + b;
         *      });
         *      EXPECT_TRUE (memoizer (1, 1) == 2 and totalCallsCount == 1);
         *      EXPECT_TRUE (memoizer (1, 1) == 2 and totalCallsCount == 1);
         *  \endcode
         *
         *  \par Example Usage (force using TimedCache):
         * 
         *  \code
         *      unsigned int totalCallsCount{};
         *      Memoizer         memoizer = Cache::Factory::Memoizer::Make<Cache::TimedCache> ([&totalCallsCount] (int a, int b) {
         *          totalCallsCount++;
         *          return a + b;
         *      });
         *      EXPECT_TRUE (memoizer (1, 1) == 2 and totalCallsCount == 1);
         *      EXPECT_TRUE (memoizer (1, 1) == 2 and totalCallsCount == 1);
         *  \endcode
         * 
         *
         *  \par Example Usage (use InternallySyncrhonized - or other special/custom cache):
         * 
         *  \code
         *      using namespace Cache::LRUCacheSupport;
         *      unsigned int totalCallsCount{};
         *      // use internally synchronized cache for memoizer
         *      template <typename K, typename V>
         *      using MyCache_ = Cache::LRUCache<K,V,InternallySynchronizedTraits<DefaultTraits<K,V>>>;
         *      Memoizer     memoizer = Cache::Factory::Memoizer::Make<MyCache_> ([&totalCallsCount] (int a, int b) {
         *          totalCallsCount++;
         *          return a + b;
         *      });
         *      EXPECT_TRUE (memoizer (1, 1) == 2 and totalCallsCount == 1);
         *      EXPECT_TRUE (memoizer (1, 1) == 2 and totalCallsCount == 1);
         *  \endcode
         * 
         */
        template <template <qStroika_template_template_BWA (typename, typename)> typename CACHE = LRUCache, typename FUNCTION>
        auto Make (FUNCTION&& f);

    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Memoizer.inl"

#endif /*_Stroika_Foundation_Cache_Memoizer_h_*/
