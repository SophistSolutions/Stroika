/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

#include <future>
#include <vector>

namespace Stroika::Foundation::Execution {

    template <invocable<>... I>
    auto RunAll (I... functions)
    {
        constexpr bool kAllVoid_ = (is_void_v<invoke_result_t<I>> && ...);

        tuple<future<invoke_result_t<I>>...> futures;

        // Launch all async tasks
        [&]<size_t... Idx> (index_sequence<Idx...>) { ((get<Idx> (futures) = async (launch::async, functions)), ...); }(index_sequence_for<I...>{});

        // Collect and return results (get does wait and then throws if function threw)
        if constexpr (kAllVoid_) {
            // All functions return void - just wait for all to complete
            [&]<size_t... Idx> (index_sequence<Idx...>) { (get<Idx> (futures).get (), ...); }(index_sequence_for<I...>{});
        }
        else {
            // Mix of void and non-void, or all non-void - filter out void results
            return [&]<size_t... Idx> (index_sequence<Idx...>) {
                return tuple_cat ([&] () {
                    if constexpr (not is_void_v<invoke_result_t<tuple_element_t<Idx, tuple<I...>>>>) {
                        return make_tuple (get<Idx> (futures).get ());
                    }
                    else {
                        get<Idx> (futures).get (); // wait for completion, but skip result
                        return tuple<> ();
                    }
                }()...);
            }(index_sequence_for<I...>{});
        }
    }

}
