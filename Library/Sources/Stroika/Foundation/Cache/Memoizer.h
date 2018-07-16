/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_Memoizer_h_
#define _Stroika_Foundation_Cache_Memoizer_h_ 1

#include "../StroikaPreComp.h"

#include <optional>
#include <vector>

#include "../Characters/String.h"
#include "../Configuration/Common.h"
#include "../Configuration/TypeHints.h"
#include "../Containers/Mapping.h"
#include "../Debug/AssertExternallySynchronizedLock.h"
#include "LRUCache.h"

/**
 *      \file
 *
 *  \version    <a href="Code-Status.md#Pre-Alpha-Maybe">Pre-Alpha-Maybe</a>
 *
 * TODO:
 *
 */

namespace Stroika::Foundation {
    namespace Cache {


        /**
         *** maybe update https://softwareengineering.stackexchange.com/questions/375257/how-can-i-aggregate-this-large-data-set-to-reduce-the-overhead-of-calculating-th/375303#375303 with this... if/when I get it working well...
         */
        template <template CACHE = LRUCache, typename RESULT, typename... ARGS>
        class Memoizer : private Debug::AssertExternallySynchronizedLock {

        public:
            Memoizer (function<RESULT (ARGS...)> f, function<Hash (ARGS...)> hash);
            Memoizer (function<RESULT (ARGS...)> f, function<Hash (ARGS...)> hash, size_t size);
            Memoizer (const Memoizer& from) = default;

        public:
            nonvirtual const Memoizer& operator= (const Memoizer& rhs) = default;

        public:
            nonvirtual RESULT Compute (ARGS... args);

        private:
            function<RESULT (ARGS...)>    fFunction_;
            CACHE<tuple<ARGS...>, RESULT> fCache_;
        };
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Memoizer.inl"

#endif /*_Stroika_Foundation_Cache_Memoizer_h_*/
