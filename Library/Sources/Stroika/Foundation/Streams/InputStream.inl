/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InputStream_inl_
#define _Stroika_Foundation_Streams_InputStream_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <mutex>

#include "../Debug/Assertions.h"
#include "../Memory/BLOB.h"
#include "EOFException.h"

namespace Stroika {
    namespace Foundation {
        namespace Streams {

            /*
             ********************************************************************************
             **************************** InputStream<ELEMENT_TYPE> *************************
             ********************************************************************************
             */
            template <typename ELEMENT_TYPE>
            inline InputStream<ELEMENT_TYPE>::InputStream (const _SharedIRep& rep)
                : inherited (rep)
            {
            }
            template <typename ELEMENT_TYPE>
            inline InputStream<ELEMENT_TYPE>::InputStream (nullptr_t)
                : inherited (nullptr)
            {
            }
            template <typename ELEMENT_TYPE>
            inline auto InputStream<ELEMENT_TYPE>::_GetRep () const -> _SharedIRep
            {
                return dynamic_pointer_cast<_IRep> (inherited::_GetRep ());
            }
            template <typename ELEMENT_TYPE>
            inline InputStream<ELEMENT_TYPE> InputStream<ELEMENT_TYPE>::Synchronized () const
            {
                struct SyncRep_ : public InputStream<ELEMENT_TYPE>::_IRep {
                public:
                    SyncRep_ (const InputStream<ELEMENT_TYPE>& realIn)
                        : InputStream<ELEMENT_TYPE>::_IRep ()
                        , fCriticalSection_ ()
                        , fRealIn_ (realIn)
                    {
                    }
                    virtual bool IsSeekable () const override
                    {
                        using Execution::make_unique_lock;
// @todo implement 'offset' support
#if qCompilerAndStdLib_make_unique_lock_IsSlow
                        MACRO_LOCK_GUARD_CONTEXT (fCriticalSection_);
#else
                        auto critSec{make_unique_lock (fCriticalSection_)};
#endif
                        return fRealIn_.IsSeekable ();
                    }
                    virtual SeekOffsetType GetReadOffset () const override
                    {
                        using Execution::make_unique_lock;
// @todo implement 'offset' support
#if qCompilerAndStdLib_make_unique_lock_IsSlow
                        MACRO_LOCK_GUARD_CONTEXT (fCriticalSection_);
#else
                        auto critSec{make_unique_lock (fCriticalSection_)};
#endif
                        return fRealIn_.GetReadOffset ();
                    }
                    virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
                    {
                        using Execution::make_unique_lock;
// @todo implement 'offset' support
#if qCompilerAndStdLib_make_unique_lock_IsSlow
                        MACRO_LOCK_GUARD_CONTEXT (fCriticalSection_);
#else
                        auto critSec{make_unique_lock (fCriticalSection_)};
#endif
                        return fRealIn_.SeekRead (whence, offset);
                    }
                    virtual size_t Read (ELEMENT_TYPE* intoStart, ELEMENT_TYPE* intoEnd) override
                    {
                        using Execution::make_unique_lock;
#if qCompilerAndStdLib_make_unique_lock_IsSlow
                        MACRO_LOCK_GUARD_CONTEXT (fCriticalSection_);
#else
                        auto critSec{make_unique_lock (fCriticalSection_)};
#endif
                        return fRealIn_.Read (intoStart, intoEnd);
                    }
                    virtual Memory::Optional<size_t> ReadSome (ElementType* intoStart, ElementType* intoEnd) override
                    {
                        Require ((intoStart == nullptr and intoEnd == nullptr) or (intoEnd - intoStart) >= 1);
                        using Execution::make_unique_lock;
#if qCompilerAndStdLib_make_unique_lock_IsSlow
                        MACRO_LOCK_GUARD_CONTEXT (fCriticalSection_);
#else
                        auto critSec{make_unique_lock (fCriticalSection_)};
#endif
                        return fRealIn_.ReadSome (intoStart, intoEnd);
                    }

                private:
                    mutable mutex             fCriticalSection_;
                    InputStream<ELEMENT_TYPE> fRealIn_;
                };
                return InputStream<ELEMENT_TYPE> (make_shared<SyncRep_> (*this));
            }
            template <typename ELEMENT_TYPE>
            inline SeekOffsetType InputStream<ELEMENT_TYPE>::GetOffset () const
            {
                return _GetRep ()->GetReadOffset ();
            }
            template <typename ELEMENT_TYPE>
            SeekOffsetType InputStream<ELEMENT_TYPE>::GetOffsetToEndOfStream () const
            {
                SeekOffsetType savedReadFrom = GetOffset ();
                SeekOffsetType size          = Seek (Whence::eFromEnd, 0);
                Seek (Whence::eFromStart, savedReadFrom);
                Assert (size >= savedReadFrom);
                size -= savedReadFrom;
                return size;
            }
            template <typename ELEMENT_TYPE>
            inline SeekOffsetType InputStream<ELEMENT_TYPE>::Seek (SignedSeekOffsetType offset) const
            {
                return _GetRep ()->SeekRead (Whence::eFromStart, offset);
            }
            template <typename ELEMENT_TYPE>
            inline SeekOffsetType InputStream<ELEMENT_TYPE>::Seek (Whence whence, SignedSeekOffsetType offset) const
            {
                return _GetRep ()->SeekRead (whence, offset);
            }
            template <typename ELEMENT_TYPE>
            inline auto InputStream<ELEMENT_TYPE>::Read () const -> Memory::Optional<ElementType>
            {
                ElementType b{};
                RequireNotNull (_GetRep ());
                return (_GetRep ()->Read (&b, &b + 1) == 0) ? Memory::Optional<ElementType> () : b;
            }
            template <typename ELEMENT_TYPE>
            inline size_t InputStream<ELEMENT_TYPE>::Read (ElementType* intoStart, ElementType* intoEnd) const
            {
                RequireNotNull (intoStart);
                Require ((intoEnd - intoStart) >= 1);
                RequireNotNull (_GetRep ().get ());
                return _GetRep ()->Read (intoStart, intoEnd);
            }
            template <typename ELEMENT_TYPE>
            inline Memory::Optional<size_t> InputStream<ELEMENT_TYPE>::ReadSome () const
            {
                RequireNotNull (_GetRep ().get ());
                return _GetRep ()->ReadSome (nullptr, nullptr);
            }
            template <typename ELEMENT_TYPE>
            inline Memory::Optional<size_t> InputStream<ELEMENT_TYPE>::ReadSome (ElementType* intoStart, ElementType* intoEnd) const
            {
                RequireNotNull (intoStart);
                Require ((intoEnd - intoStart) >= 1);
                RequireNotNull (_GetRep ().get ());
                return _GetRep ()->ReadSome (intoStart, intoEnd);
            }
            template <typename ELEMENT_TYPE>
            template <typename POD_TYPE, typename TEST_TYPE, typename ENABLE_IF_TEST>
            POD_TYPE InputStream<ELEMENT_TYPE>::ReadRaw () const
            {
                static_assert (std::is_pod<POD_TYPE>::value, "");
                POD_TYPE tmp; // intentionally don't zero-initialize
                size_t   n{ReadAll (reinterpret_cast<Memory::Byte*> (&tmp), reinterpret_cast<Memory::Byte*> (&tmp + 1))};
                if (n == sizeof (tmp)) {
                    return tmp;
                }
                else {
                    Execution::Throw ((n == 0) ? EOFException::kThe : EOFException (true));
                }
            }
            template <typename ELEMENT_TYPE>
            template <typename POD_TYPE, typename TEST_TYPE, typename ENABLE_IF_TEST>
            inline void InputStream<ELEMENT_TYPE>::ReadRaw (POD_TYPE* start, POD_TYPE* end) const
            {
                static_assert (std::is_pod<POD_TYPE>::value, "");
                size_t n{ReadAll (reinterpret_cast<Memory::Byte*> (start), reinterpret_cast<Memory::Byte*> (end))};
                if (n != sizeof (POD_TYPE) * (end - start)) {
                    Execution::Throw ((n == 0) ? EOFException::kThe : EOFException (true));
                }
            }
            template <typename ELEMENT_TYPE>
            size_t InputStream<ELEMENT_TYPE>::ReadAll (ElementType* intoStart, ElementType* intoEnd) const
            {
                RequireNotNull (intoStart);
                Require ((intoEnd - intoStart) >= 1);
                RequireNotNull (_GetRep ().get ());
                size_t elementsRead{};
                for (ElementType* readCursor = intoStart; readCursor < intoEnd;) {
                    size_t eltsReadThisTime = Read (readCursor, intoEnd);
                    Assert (eltsReadThisTime <= static_cast<size_t> (intoEnd - readCursor));
                    if (eltsReadThisTime == 0) {
                        // irrevocable EOF
                        break;
                    }
                    elementsRead += eltsReadThisTime;
                    readCursor += eltsReadThisTime;
                }
                return elementsRead;
            }
        }
    }
}
#endif /*_Stroika_Foundation_Streams_InputStream_inl_*/
