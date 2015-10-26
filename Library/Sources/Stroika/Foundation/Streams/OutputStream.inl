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
#include    <mutex>

#include    "../Characters/String.h"
#include    "../Debug/Assertions.h"
#include    "../Memory/BLOB.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {



            /*
             ********************************************************************************
             **************************** OutputStream<ELEMENT_TYPE> ************************
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
            inline  OutputStream<ELEMENT_TYPE>  OutputStream<ELEMENT_TYPE>::Synchronized () const
            {
                struct   SyncRep_ : public OutputStream<ELEMENT_TYPE>::_IRep {
                public:
                    SyncRep_ (const OutputStream<ELEMENT_TYPE>& realIn)
                        : OutputStream<ELEMENT_TYPE>::_IRep ()
                        , fCriticalSection_ ()
                        , fRealIn_ (realIn)
                    {
                    }
                    virtual bool    IsSeekable () const override
                    {
                        using   Execution::make_unique_lock;
                        // @todo implement 'offset' support
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
                        MACRO_LOCK_GUARD_CONTEXT (fCriticalSection_);
#else
                        auto    critSec { make_unique_lock (fCriticalSection_) };
#endif
                        return fRealIn_.IsSeekable ();
                    }
                    virtual SeekOffsetType  GetWriteOffset () const override
                    {
                        using   Execution::make_unique_lock;
                        // @todo implement 'offset' support
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
                        MACRO_LOCK_GUARD_CONTEXT (fCriticalSection_);
#else
                        auto    critSec { make_unique_lock (fCriticalSection_) };
#endif
                        return fRealIn_.GetWriteOffset ();
                    }
                    virtual SeekOffsetType  SeekWrite (Whence whence, SignedSeekOffsetType offset) override
                    {
                        using   Execution::make_unique_lock;
                        // @todo implement 'offset' support
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
                        MACRO_LOCK_GUARD_CONTEXT (fCriticalSection_);
#else
                        auto    critSec { make_unique_lock (fCriticalSection_) };
#endif
                        return fRealIn_.SeekWrite (whence, offset);
                    }
                    virtual size_t  Write (ELEMENT_TYPE* intoStart, ELEMENT_TYPE* intoEnd) override
                    {
                        using   Execution::make_unique_lock;
                        // @todo implement 'offset' support
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
                        MACRO_LOCK_GUARD_CONTEXT (fCriticalSection_);
#else
                        auto    critSec { make_unique_lock (fCriticalSection_) };
#endif
                        return fRealIn_.Write (intoStart, intoEnd);
                    }

                private:
                    mutable mutex               fCriticalSection_;
                    OutputStream<ELEMENT_TYPE>  fRealIn_;
                };
                return OutputStream<ELEMENT_TYPE> (make_shared<SyncRep_> (*this));
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
            template    <typename TEST_TYPE, typename ENABLE_IF_TEST>
            inline  void    OutputStream<ELEMENT_TYPE>::Write (const wchar_t* cStr) const
            {
                Write (cStr, cStr + ::wcslen (cStr));
            }
            template    <typename ELEMENT_TYPE>
            inline  void    OutputStream<ELEMENT_TYPE>::Flush () const
            {
                _GetRep ()->Flush ();
            }
            template    <>
            template    <>
            inline  const OutputStream<Characters::Character>& OutputStream<Characters::Character>::operator<< (const Characters::String& write2TextStream) const
            {
                Write (write2TextStream);
                return *this;
            }
            template    <>
            template    <>
            inline  const OutputStream<Characters::Character>& OutputStream<Characters::Character>::operator<< (const wchar_t* write2TextStream) const
            {
                Write (write2TextStream);
                return *this;
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_OutputStream_inl_*/
