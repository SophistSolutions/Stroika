/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_DisjointDiscreteRange_inl_
#define _Stroika_Foundation_Traversal_DisjointDiscreteRange_inl_


namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            /*
            ********************************************************************************
            ************************ DisjointDiscreteRange<RANGE_TYPE> *********************
            ********************************************************************************
            */
            template    <typename RANGE_TYPE>
            void    DisjointDiscreteRange<RANGE_TYPE>::Add (ElementType elt)
            {
                auto srs { this->GetSubRanges () };
                // Walk list, and if new item < than a given, either extend or insert. If contained, we have nothing todo
                for (Iterator<RangeType> i = srs.begin (); i != srs.end (); ++i) {
                    if (elt < i->GetLowerBound ()) {
                        // then either extend by one or insert new item
                        if (elt == i->GetLowerBound () - 1) {
                            srs.Update (i, DiscreteRange<ElementType> (elt, i->GetUpperBound ()));
                            //@todo in this case we should check for merge adjacent, but not critical
                        }
                        else {
                            srs.insert (i, DiscreteRange<ElementType> (elt, elt));
                        }
                        *this = srs;
                        return;
                    }
                    if (i->Contains (elt)) {
                        return;
                    }
                }
                // if not less than any there, we must append new item
                srs.push_back (DiscreteRange<ElementType> (elt, elt));
                *this = srs;
            }
            template    <typename RANGE_TYPE>
            auto   DisjointDiscreteRange<RANGE_TYPE>::GetNext (ElementType elt) const -> Memory::Optional<ElementType> {
                for (DiscreteRange<ElementType> i : this->GetSubRanges ())
                {
                    if (i.Contains (elt + 1)) {
                        return elt + 1;
                    }
                    //                    if (i.GetUpperBound () < s) {
                    //                        break;
                    //                    }
                }
                return Memory::Optional<ElementType> ();
            }
            template    <typename RANGE_TYPE>
            auto DisjointDiscreteRange<RANGE_TYPE>::GetPrevious (ElementType elt) const -> Memory::Optional<ElementType> {
                for (DiscreteRange<ElementType> i : this->GetSubRanges ())
                {
                    if (i.Contains (elt - 1)) {
                        return elt - 1;
                    }
                    if (i.GetUpperBound () <= elt) {
                        break;
                    }
                }
                return Memory::Optional<ElementType> ();
            }


        }
    }
}
#endif /* _Stroika_Foundation_Traversal_DisjointDiscreteRange_inl_ */
