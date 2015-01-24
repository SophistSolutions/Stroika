/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Partition_inl_
#define _Stroika_Foundation_Traversal_Partition_inl_

#include    "../Containers/SortedMapping.h"
#include    "../Debug/Assertions.h"
#include    "../Debug/Trace.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            namespace Private_ {
                template    <typename RANGETYPE>
                bool    IsPartition_Helper_ (const Iterable<RANGETYPE>& iterable/*, typename enable_if <isomethingto check for operator <>::type usesInsertPair = 0*/)
                {
                    using   Common::KeyValuePair;
                    using   Containers::SortedMapping;
                    using   Debug::TraceContextBumper;
                    using   Memory::Optional;
                    TraceContextBumper ctx (SDKSTR ("IsPartition_Helper_"));
                    using   namespace   Traversal;
                    using ElementType = typename RANGETYPE::ElementType;
                    SortedMapping<ElementType, RANGETYPE>   tmp;
                    for (RANGETYPE r : iterable) {
                        tmp.Add (r.GetLowerBound (), r);
                    }
                    Optional<ElementType>   upperBoundSeenSoFar;
                    Openness                upperBoundSeenSoFarOpenness {};
                    for (KeyValuePair<ElementType, RANGETYPE> i : tmp) {
                        DbgTrace ("i.fKey = %f, i.fValue = (%f,%f, ol=%d, or=%d)", i.fKey, i.fValue.GetLowerBound (), i.fValue.GetUpperBound (), i.fValue.GetLowerBoundOpenness (), i.fValue.GetUpperBoundOpenness ());
                        if (upperBoundSeenSoFar) {
                            if (*upperBoundSeenSoFar != i.fValue.GetLowerBound ()) {
                                DbgTrace ("i.fKey = %f, i.fValue = (%f,%f, ol=%d, or=%d)", i.fKey, i.fValue.GetLowerBound (), i.fValue.GetUpperBound (), i.fValue.GetLowerBoundOpenness (), i.fValue.GetUpperBoundOpenness ());
                                DbgTrace ("return false cuz boudns no match)");
                                return false;
                            }
                            if (upperBoundSeenSoFarOpenness == i.fValue.GetLowerBoundOpenness ()) {
                                DbgTrace ("i.fKey = %f, i.fValue = (%f,%f, ol=%d, or=%d)", i.fKey, i.fValue.GetLowerBound (), i.fValue.GetUpperBound (), i.fValue.GetLowerBoundOpenness (), i.fValue.GetUpperBoundOpenness ());
                                DbgTrace ("return false cuz boudns openness no match: upperBoundSeenSoFarOpenness =%d, and i.fValue.GetLowerBoundOpenness ()=%d)", upperBoundSeenSoFarOpenness, i.fValue.GetLowerBoundOpenness ());
                                return false;
                            }
                        }
                        upperBoundSeenSoFar = i.fValue.GetUpperBound ();
                        upperBoundSeenSoFarOpenness = i.fValue.GetUpperBoundOpenness ();
                    }
                    return true;
                }
            }


            /**
             */
            template    <typename RANGETYPE>
            bool    IsPartition (const Iterable<RANGETYPE>& iterable)
            {
                return Private_::IsPartition_Helper_ (iterable);
            }


        }
    }
}
#endif /* _Stroika_Foundation_Traversal_Partition_inl_ */
