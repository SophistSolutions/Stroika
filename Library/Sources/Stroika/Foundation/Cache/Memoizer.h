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

#if qCompilerAndStdLib_template_template_argument_as_different_template_paramters_Buggy
    namespace MemoizerSupport {
        template <typename T1, typename T2>
        using DEFAULT_CACHE_BWA_ = LRUCache<T1, T2>;
    }
#endif
#if qCompilerAndStdLib_template_template_argument_as_different_template_paramters_Buggy || 1
#define qStroika_template_template_BWA(...) typename, typename
#else
#define qStroika_template_template_BWA(...) typename...
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
     *  \note   Memoizer works well wtih LRUCache, or TimedCache.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md">Same as (worse case of) underlying CACHE template argument, and argument function. Since the function will typically be fully reentrant, this comes down to the re-entrancy of the argument Cache.</a>
     */
    template <typename RESULT, template <qStroika_template_template_BWA (typename, typename)> class CACHE = LRUCache, typename... ARGS>
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

    namespace Private_ {
        template <typename F, typename Tuple>
        struct memoizer_builder;

        template <typename F, typename... Args>
        struct memoizer_builder<F, std::tuple<Args...>> {
            using type = Memoizer<typename Common::FunctionTraits<F>::result_type, LRUCache, Args...>;
        };
    }
    template <typename F, template <qStroika_template_template_BWA (typename, typename)> typename CACHE = LRUCache>
    auto MakeMemoizer (F&& f)
    {
        using ArgsTuple = typename Common::FunctionTraits<F>::args_tuple;
        typename Private_::memoizer_builder<F, ArgsTuple>::type m (f);
        return m;
    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Memoizer.inl"

#endif /*_Stroika_Foundation_Cache_Memoizer_h_*/
