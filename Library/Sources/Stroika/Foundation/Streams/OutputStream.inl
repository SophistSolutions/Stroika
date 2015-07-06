/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_OutputStream_inl_
#define _Stroika_Foundation_Streams_OutputStream_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"
#include    "../Memory/BLOB.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {



            /*
             ********************************************************************************
             **************************** OutputStream<ELEMENT_TYPE> *************************
             ********************************************************************************
             */
            template    <typename ELEMENT_TYPE>
            inline  OutputStream<ELEMENT_TYPE>::OutputStream (const _SharedIRep& rep)
                : inherited (rep)
            {
            }
            template    <typename ELEMENT_TYPE>
            inline  OutputStream<ELEMENT_TYPE>::OutputStream (nullptr_t)
                : inherited (nullptr)
            {
            }
            template    <typename ELEMENT_TYPE>
            inline  auto    OutputStream<ELEMENT_TYPE>::_GetRep () const -> _SharedIRep
            {
                return dynamic_pointer_cast<_IRep> (inherited::_GetRep ());
            }
            template    <typename ELEMENT_TYPE>
            inline  SeekOffsetType  OutputStream<ELEMENT_TYPE>::GetOffset () const
            {
                return _GetRep ()->GetWriteOffset ();
            }
            template    <typename ELEMENT_TYPE>
            SeekOffsetType  OutputStream<ELEMENT_TYPE>::GetOffsetToEndOfStream () const
            {
                SeekOffsetType  savedReadFrom = GetOffset ();
                SeekOffsetType  size =  Seek (Whence::eFromEnd, 0);
                Seek (Whence::eFromStart, savedReadFrom);
                Assert (size >= savedReadFrom);
                size -= savedReadFrom;
                return size;
            }
            template    <typename ELEMENT_TYPE>
            inline  SeekOffsetType    OutputStream<ELEMENT_TYPE>::Seek (SignedSeekOffsetType offset) const
            {
                return _GetRep ()->SeekWrite (Whence::eFromStart, offset);
            }
            template    <typename ELEMENT_TYPE>
            inline  SeekOffsetType    OutputStream<ELEMENT_TYPE>::Seek (Whence whence, SignedSeekOffsetType offset) const
            {
                return _GetRep ()->SeekWrite (whence, offset);
            }
            template    <typename ELEMENT_TYPE>
            inline  void    OutputStream<ELEMENT_TYPE>::Write (const ElementType* start, const ElementType* end) const
            {
                Require (start <= end);
                Require (start != nullptr or start == end);
                Require (end != nullptr or start == end);
                if (start != end) {
                    _GetRep ()->Write (start, end);
                }
            }
            template    <typename ELEMENT_TYPE>
            template    <typename TEST_TYPE, typename ENABLE_IF_TEST>
            inline  void    OutputStream<ELEMENT_TYPE>::Write (const Memory::BLOB& blob) const
            {
                Write (blob.begin (), blob.end ());
            }
            template    <typename ELEMENT_TYPE>
            inline  void    OutputStream<ELEMENT_TYPE>::Flush () const
            {
                _GetRep ()->Flush ();
            }



        }
    }
}
#endif  /*_Stroika_Foundation_Streams_OutputStream_inl_*/
