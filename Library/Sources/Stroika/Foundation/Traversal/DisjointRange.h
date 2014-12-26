/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_DisjointRange_h_
#define _Stroika_Foundation_Traversal_DisjointRange_h_  1

#include    "../StroikaPreComp.h"

#include    "../Characters/String.h"
#include    "../Configuration/Common.h"
#include    "../Containers/Sequence.h"
#include    "../Memory/Optional.h"

#include    "Range.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 *  TODO:
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {

            template    <typename RANGE_TYPE>
            class   DisjointRange {
            public:
                /**
                 */
                using   RangeType     =   RANGE_TYPE;

            public:
                /**
                 */
                using   ElementType     =   typename RangeType::ElementType;

            public:
                DisjointRange () = default;
                DisjointRange (const DisjointRange&) = default;
                DisjointRange (const RangeType& from);
                DisjointRange (const initializer_list<RangeType>& from);

            public:
                DisjointRange& operator= (const DisjointRange& rhs) = default;

            public:
                /**
                 */
                static  DisjointRange<RangeType> FullRange ();

            public:
                /**
                ***NYI
                 */
                nonvirtual  Containers::Sequence<RangeType>    GetSubRanges () const;

            public:
                /**
                 */
                nonvirtual   bool    empty () const;

            public:
                /**
                 * if empty, returns empty range()
                 */
                nonvirtual  RangeType   GetBounds () const;

            public:
                /**
                ***NYI
                 */
                template    <typename RANGE_TYPE2>
                nonvirtual  bool    Equals (const DisjointRange<RANGE_TYPE2>& rhs) const;

            public:
                /**
                ***NYI
                 */
                template    <typename RANGE_TYPE2>
                nonvirtual  bool    Intersects (const DisjointRange<RANGE_TYPE2>& rhs) const;

            public:
                /**
                ***NYI
                 */
                nonvirtual  DisjointRange<RangeType>    Intersection (const DisjointRange<RangeType>& rhs) const;

            public:
                /**
                ***NYI
                 */
                nonvirtual  DisjointRange<RangeType>    Union (const DisjointRange<RangeType>& rhs) const;

            public:
                /**
                ***NYI
                 */
                nonvirtual  RangeType    UnionBounds (const DisjointRange<RangeType>& rhs) const;

            private:
                Containers::Sequence<RangeType> fSubRanges_;
            };


        }
    }
}



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include    "DisjointRange.inl"

#endif  /*_Stroika_Foundation_Traversal_DisjointRange_h_ */
