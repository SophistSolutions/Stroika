/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InputStream_inl_
#define _Stroika_Foundation_Streams_InputStream_inl_  1


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
             **************************** InputStream<ELEMENT_TYPE> *************************
             ********************************************************************************
             */
            template    <typename ELEMENT_TYPE>
            inline  InputStream<ELEMENT_TYPE>::InputStream (const _SharedIRep& rep)
                : inherited (rep)
            {
            }
            template    <typename ELEMENT_TYPE>
            inline  InputStream<ELEMENT_TYPE>::InputStream (nullptr_t)
                : inherited (nullptr)
            {
            }
            template    <typename ELEMENT_TYPE>
            inline  auto    InputStream<ELEMENT_TYPE>::_GetRep () const -> _SharedIRep
            {
                return dynamic_pointer_cast<_IRep> (inherited::_GetRep ());
            }
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
            template    <typename   BASESTREAM>
			template    <typename TEST_TYPE, typename ENABLE_IF_TEST>
            Memory::BLOB InputStream<BASESTREAM>::ReadAll () const
            {
                if (this->IsSeekable ()) {
                    SeekOffsetType  size = this->GetOffsetToEndOfStream ();
                    if (size >= numeric_limits<size_t>::max ()) {
                        Execution::DoThrow<bad_alloc> (bad_alloc ());
                    }
                    size_t sb = static_cast<size_t> (size);
                    if (sb == 0) {
                        return Memory::BLOB ();
                    }
                    Byte* b = new Byte[sb];   // if this fails, we had no way to create the BLOB
                    size_t n = this->Read (b, b + sb);
                    Assert (n <= sb);
                    return Memory::BLOB::Attach (b, b + n);
                }
                else {
                    // Less efficient implementation
                    vector<Byte>    r;
                    size_t          n;
                    Byte            buf[32 * 1024];
                    while ( (n = this->Read (std::begin (buf), std::end (buf))) != 0) {
                        r.insert (r.end (), std::begin (buf), std::begin (buf) + n);
                    }
                    return Memory::BLOB (r);
                }
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_InputStream_inl_*/
