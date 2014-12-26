/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_DisjointRange_inl_
#define _Stroika_Foundation_Traversal_DisjointRange_inl_


namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            /*
            ********************************************************************************
            ************************* DisjointRange<RANGE_TYPE> ****************************
            ********************************************************************************
            */
            template    <typename RANGE_TYPE>
            DisjointRange<RANGE_TYPE>::DisjointRange (const RangeType& from)
                : fSubRanges_ ({ from })
            {
            }
            template    <typename RANGE_TYPE>
            DisjointRange<RANGE_TYPE>::DisjointRange (const initializer_list<RangeType>& from)
                : fSubRanges_ (from)
            {
            }
            template    <typename RANGE_TYPE>
            inline  auto    DisjointRange<RANGE_TYPE>::GetSubRanges () const -> Containers::Sequence<RangeType> {
                return fSubRanges_;
            }
            template    <typename RANGE_TYPE>
            DisjointRange<RANGE_TYPE> DisjointRange<RANGE_TYPE>::FullRange ()
            {
                return DisjointRange<RangeType> (RangeType::FullRange ());
            }
            template    <typename RANGE_TYPE>
            inline  bool    DisjointRange<RANGE_TYPE>::empty () const
            {
                return fSubRanges_.empty ();
            }
            template    <typename RANGE_TYPE>
            RANGE_TYPE   DisjointRange<RANGE_TYPE>::GetBounds () const
            {
                size_t  n   =   fSubRanges_.size ();
                switch (n) {
                    case 0:
                        return RangeType {};
                    case 1:
                        return fSubRanges_[0];
                    default:
                        return RangeType (fSubRanges_[0].GetLowerBound (), fSubRanges_.GetLast ().GetUpperBound ());
                }
            }


        }
    }
}
#endif /* _Stroika_Foundation_Traversal_DisjointRange_inl_ */
