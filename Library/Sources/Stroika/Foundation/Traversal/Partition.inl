/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Partition_inl_
#define _Stroika_Foundation_Traversal_Partition_inl_

#include "../Containers/SortedMapping.h"
#include "../Debug/Assertions.h"
#include "../Debug/Trace.h"

namespace Stroika::Foundation::Traversal {

    /**
     */
    template <Traversal::IRange RANGETYPE, Common::IPotentiallyComparer<typename RANGETYPE::value_type> RANGE_ELT_COMPARER>
    inline bool IsPartition (const Iterable<RANGETYPE>& iterable, RANGE_ELT_COMPARER comparer)
    {
        using Common::KeyValuePair;
        using Containers::SortedMapping;
        using Debug::TraceContextBumper;
        TraceContextBumper ctx{"IsPartition_Helper_"};
        using namespace Traversal;
        using value_type = typename RANGETYPE::value_type;
        SortedMapping<value_type, RANGETYPE> tmp;
        for (const RANGETYPE& r : iterable) {
            tmp.Add (r.GetLowerBound (), r);
        }
        optional<value_type> upperBoundSeenSoFar;
        Openness             upperBoundSeenSoFarOpenness{};
        for (const KeyValuePair<value_type, RANGETYPE>& i : tmp) {
            //DbgTrace ("i.fKey = %f, i.fValue = (%f,%f, ol=%d, or=%d)", i.fKey, i.fValue.GetLowerBound (), i.fValue.GetUpperBound (), i.fValue.GetLowerBoundOpenness (), i.fValue.GetUpperBoundOpenness ());
            if (upperBoundSeenSoFar) {
                if (not comparer (*upperBoundSeenSoFar, i.fValue.GetLowerBound ())) {
                    //DbgTrace ("i.fKey = %f, i.fValue = (%f,%f, ol=%d, or=%d)", i.fKey, i.fValue.GetLowerBound (), i.fValue.GetUpperBound (), i.fValue.GetLowerBoundOpenness (), i.fValue.GetUpperBoundOpenness ());
                    //DbgTrace ("return false cuz boudns no match");
                    return false;
                }
                if (upperBoundSeenSoFarOpenness == i.fValue.GetLowerBoundOpenness ()) {
                    //DbgTrace ("i.fKey = %f, i.fValue = (%f,%f, ol=%d, or=%d)", i.fKey, i.fValue.GetLowerBound (), i.fValue.GetUpperBound (), i.fValue.GetLowerBoundOpenness (), i.fValue.GetUpperBoundOpenness ());
                    //DbgTrace ("return false cuz boudns openness no match: upperBoundSeenSoFarOpenness =%d, and i.fValue.GetLowerBoundOpenness ()=%d)", upperBoundSeenSoFarOpenness, i.fValue.GetLowerBoundOpenness ());
                    return false;
                }
            }
            upperBoundSeenSoFar         = i.fValue.GetUpperBound ();
            upperBoundSeenSoFarOpenness = i.fValue.GetUpperBoundOpenness ();
        }
        return true;
    }
    template <Traversal::IRange RANGETYPE>
    inline bool IsPartition (const Iterable<RANGETYPE>& iterable)
    {
        return IsPartition (
            iterable, [] (typename RANGETYPE::value_type lhs, typename RANGETYPE::value_type rhs) { return Math::NearlyEquals (lhs, rhs); });
    }

}

#endif /* _Stroika_Foundation_Traversal_Partition_inl_ */
