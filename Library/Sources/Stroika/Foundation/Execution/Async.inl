/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

#include <future>
#include <vector>

namespace Stroika::Foundation::Execution {

    template <invocable<>... I>
    void RunAll (I... functions)
    {
        vector<future<void>> futures;

        // @todo is simple way to iterate without converting to function<>
        for (auto fi : initializer_list<function<void ()>>{forward<I> (functions)...}) {
            futures.push_back (async (launch::async, fi));
        }

        // Wait for all tasks to complete
        for (auto& future : futures) {
            future.wait ();
        }
    }

}
