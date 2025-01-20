/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */

#include <future>
#include <vector>

namespace Stroika::Foundation::Execution {

    template <invocable<>... I>
    void InvokeAsync (I... f)
    {
        std::vector<std::future<void>> futures;

        for (auto fi : f) {
            futures.push_back (std::async (std::launch::async, fi));
        }

        // Wait for all tasks to complete
        for (auto& future : futures) {
            future.wait ();
        }
    }

}
