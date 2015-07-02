/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InputOutputStream_inl_
#define _Stroika_Foundation_Streams_InputOutputStream_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {



            /*
             ********************************************************************************
             ********************** InputOutputStream<ELEMENT_TYPE> *************************
             ********************************************************************************
             */
            template    <typename ELEMENT_TYPE>
            inline  InputOutputStream<ELEMENT_TYPE>::InputOutputStream (const _SharedIRep& rep)
                : inherited (rep)
            {
            }
            template    <typename ELEMENT_TYPE>
            inline  InputOutputStream<ELEMENT_TYPE>::InputOutputStream (nullptr_t)
                : inherited (nullptr)
            {
            }
            template    <typename ELEMENT_TYPE>
            inline  auto    InputOutputStream<ELEMENT_TYPE>::_GetRep () const -> _SharedIRep
            {
                return dynamic_pointer_cast<_IRep> (InputOutputStream<ELEMENT_TYPE>::_GetRep ());
            }
#if 0
            template    <typename ELEMENT_TYPE>
            inline  SeekOffsetType  InputStream<ELEMENT_TYPE>::GetOffset () const
            {
                return _GetRep ()->GetReadOffset ();
            }
            template    <typename ELEMENT_TYPE>
            SeekOffsetType  InputStream<ELEMENT_TYPE>::GetOffsetToEndOfStream () const
            {
                SeekOffsetType  savedReadFrom = GetOffset ();
                SeekOffsetType  size =  Seek (Whence::eFromEnd, 0);
                Seek (Whence::eFromStart, savedReadFrom);
                Assert (size >= savedReadFrom);
                size -= savedReadFrom;
                return size;
            }
            template    <typename ELEMENT_TYPE>
            inline  SeekOffsetType    InputStream<ELEMENT_TYPE>::Seek (SignedSeekOffsetType offset) const
            {
                return _GetRep ()->SeekRead (Whence::eFromStart, offset);
            }
            template    <typename ELEMENT_TYPE>
            inline  SeekOffsetType    InputStream<ELEMENT_TYPE>::Seek (Whence whence, SignedSeekOffsetType offset) const
            {
                return _GetRep ()->SeekRead (whence, offset);
            }
            template    <typename ELEMENT_TYPE>
            inline  auto  InputStream<ELEMENT_TYPE>::Read () const -> Memory::Optional<ElementType> {
                ElementType b {};
                RequireNotNull (_GetRep ());
                return (_GetRep ()->Read (nullptr, &b, &b + 1) == 0) ? Memory::Optional<ElementType> () : b;
            }
            template    <typename ELEMENT_TYPE>
            inline  size_t  InputStream<ELEMENT_TYPE>::Read (ElementType* intoStart, ElementType* intoEnd) const
            {
                RequireNotNull (intoStart);
                Require ((intoEnd - intoStart) >= 1);
                RequireNotNull (_GetRep ().get ());
                return _GetRep ()->Read (nullptr, intoStart, intoEnd);
            }
            template    <typename ELEMENT_TYPE>
            size_t  InputStream<ELEMENT_TYPE>::Read (SeekOffsetType* offset, ElementType* intoStart, ElementType* intoEnd) const
            {
                RequireNotNull (offset);
                return _GetRep ()->Read (offset, intoStart, intoEnd);
            }
#endif


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_InputOutputStream_inl_*/
