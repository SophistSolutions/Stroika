/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_BLOB_inl_
#define _Stroika_Foundation_Memory_BLOB_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Containers/Common.h"
#include "../Debug/Assertions.h"
#include "../Memory/BlockAllocated.h"
#include "../Memory/Common.h"
#include "../Memory/InlineBuffer.h"
#include "../Traversal/Iterator.h"

namespace Stroika::Foundation::Memory {

    struct BLOB::BasicRep_ : public _IRep, public Memory::UseBlockAllocationIfAppropriate<BasicRep_> {
        //  really not sure what size to use???
        //  May not be any universal, good answer...
        //  Remember - users can subclass BLOB, and provider their own
        //  'rep' type tuned to their application.
        InlineBuffer<byte, 64> fData;

        BasicRep_ (span<const byte> s)
            : fData{Memory::eUninitialized, s.size ()}
        {
            copy (s.begin (), s.end (), fData.begin ());
        }

        BasicRep_ (const initializer_list<pair<const byte*, const byte*>>& startEndPairs);
        BasicRep_ (const initializer_list<BLOB>& list2Concatenate);
        BasicRep_ (const BasicRep_&)            = delete;
        BasicRep_& operator= (const BasicRep_&) = delete;

        virtual span<const byte> GetBounds () const override;
    };

    struct BLOB::ZeroRep_ : public _IRep, public Memory::UseBlockAllocationIfAppropriate<ZeroRep_> {
        virtual span<const byte> GetBounds () const override;
        ZeroRep_ ()                           = default;
        ZeroRep_ (const ZeroRep_&)            = delete;
        ZeroRep_& operator= (const ZeroRep_&) = delete;
    };

    struct BLOB::AdoptRep_ : public _IRep, public Memory::UseBlockAllocationIfAppropriate<AdoptRep_> {
        const byte* fStart;
        const byte* fEnd;

        AdoptRep_ (const AdoptRep_&) = delete;
        AdoptRep_ (const byte* start, const byte* end);
        ~AdoptRep_ ();
        AdoptRep_&               operator= (const AdoptRep_&) = delete;
        virtual span<const byte> GetBounds () const override;
    };

    struct BLOB::AdoptAppLifetimeRep_ : public _IRep, public Memory::UseBlockAllocationIfAppropriate<AdoptAppLifetimeRep_> {
        const byte* fStart;
        const byte* fEnd;

        AdoptAppLifetimeRep_ ()                            = delete;
        AdoptAppLifetimeRep_ (const AdoptAppLifetimeRep_&) = delete;
        AdoptAppLifetimeRep_ (const byte* start, const byte* end);
        AdoptAppLifetimeRep_&    operator= (const AdoptAppLifetimeRep_&) = delete;
        virtual span<const byte> GetBounds () const override;
    };

    /*
     ********************************************************************************
     ************************************** BLOB ************************************
     ********************************************************************************
     */
    template <typename T, typename... ARGS_TYPE>
    inline BLOB::_SharedRepImpl<T> BLOB::_MakeSharedPtr (ARGS_TYPE&&... args)
    {
        if constexpr (kBLOBUsesStroikaSharedPtr) {
            return Memory::MakeSharedPtr<T> (forward<ARGS_TYPE> (args)...);
        }
        else if constexpr (Memory::UsesBlockAllocation<T> ()) {
            // almost as good, but still does two allocs, above does one shared alloc of the block allocated controlblock+T
            //return shared_ptr<T> (new T {forward<ARGS_TYPE> (args)...});
            return allocate_shared<T> (Memory::BlockAllocator<T>{}, forward<ARGS_TYPE> (args)...);
        }
        else {
            return make_shared<T> (forward<ARGS_TYPE> (args)...);
        }
    }
    inline BLOB::BLOB ()
        : fRep_{_MakeSharedPtr<ZeroRep_> ()}
    {
    }
    template <typename CONTAINER_OF_BYTE>
    inline BLOB::BLOB (const CONTAINER_OF_BYTE& data)
        requires Configuration::IsIterable_v<CONTAINER_OF_BYTE> and (is_convertible_v<typename CONTAINER_OF_BYTE::value_type, byte> or is_convertible_v<typename CONTAINER_OF_BYTE::value_type, uint8_t>)
#if qCompilerAndStdLib_spanOfContainer_Buggy
    : BLOB{as_bytes (mkSpan_BWA_ (data))}
#else
    : BLOB{as_bytes (span{data})}
#endif
    {
    }
    inline BLOB::BLOB (const initializer_list<byte>& bytes)
#if qCompilerAndStdLib_spanOfContainer_Buggy
        : BLOB{mkSpan_BWA_ (bytes)}
#else
        : BLOB{span{bytes}}
#endif
    {
    }
    inline BLOB::BLOB (const initializer_list<uint8_t>& bytes)
#if qCompilerAndStdLib_spanOfContainer_Buggy
        : BLOB{as_bytes (mkSpan_BWA_ (bytes))}
#else
        : BLOB{as_bytes (span{bytes})}
#endif
    {
    }
    inline BLOB::BLOB (span<const byte> s)
        : fRep_{s.empty () ? _SharedIRep{_MakeSharedPtr<ZeroRep_> ()} : _SharedIRep{_MakeSharedPtr<BasicRep_> (s)}}
    {
        Ensure (s.size () == size ());
    }
    inline BLOB::BLOB (const byte* start, const byte* end)
        : BLOB{span{start, end}}
    {
        Ensure (static_cast<size_t> (end - start) == size ());
    }
    inline BLOB::BLOB (const uint8_t* start, const uint8_t* end)
        : BLOB{as_bytes (span{start, end})}
    {
        Ensure (static_cast<size_t> (end - start) == size ());
    }
    inline BLOB::BLOB (const initializer_list<pair<const byte*, const byte*>>& startEndPairs)
        : fRep_{_MakeSharedPtr<BasicRep_> (startEndPairs)}
    {
    }
    inline BLOB::BLOB (const initializer_list<BLOB>& list2Concatenate)
        : fRep_{_MakeSharedPtr<BasicRep_> (list2Concatenate)}
    {
    }
    inline BLOB::BLOB (const _SharedIRep& rep)
        : fRep_{rep}
    {
    }
    inline BLOB::BLOB (_SharedIRep&& rep)
        : fRep_{move (rep)}
    {
    }
    inline BLOB BLOB::Hex (const char* b)
    {
        RequireNotNull (b);
        return Hex (b, b + ::strlen (b));
    }
    inline BLOB BLOB::Hex (const string& s)
    {
        return Hex (s.c_str ());
    }
    inline BLOB BLOB::Hex (const string_view& s)
    {
        return Hex (s.data (), s.data () + s.length ());
    }
    template <typename T, enable_if_t<is_trivially_copyable_v<T>>*>
    inline BLOB BLOB::Raw (const T* s, const T* e)
    {
        return BLOB (reinterpret_cast<const byte*> (s), reinterpret_cast<const byte*> (e));
    }
    template <typename T, enable_if_t<is_trivially_copyable_v<T>>*>
    inline BLOB BLOB::Raw (const T* s, size_t sz)
    {
        return BLOB (reinterpret_cast<const byte*> (s), reinterpret_cast<const byte*> (s + sz));
    }
    template <typename T, enable_if_t<is_same_v<typename char_traits<T>::char_type, T>>*>
    inline BLOB BLOB::Raw (const T* s)
    {
        RequireNotNull (s);
        return Raw (s, s + char_traits<T>::length (s));
    }
    template <typename T, enable_if_t<is_same_v<typename char_traits<T>::char_type, T>>*>
    inline BLOB BLOB::Raw (const basic_string<T>& s)
    {
        return Raw (s.c_str (), s.c_str () + s.length ());
    }
    template <typename T, enable_if_t<is_trivially_copyable_v<T>>*>
    inline BLOB BLOB::Raw (const T& s)
    {
        return Raw (&s, &s + 1);
    }
    inline BLOB BLOB::Attach (const byte* start, const byte* end)
    {
        Require ((start == nullptr and end == nullptr) or (start != nullptr and end != nullptr));
        Require (start <= end);
        return BLOB{_MakeSharedPtr<AdoptRep_> (start, end)};
    }
    inline BLOB BLOB::Attach (span<const byte> s)
    {
        return Attach (SafeBegin (s), SafeEnd (s));
    }
    inline BLOB BLOB::AttachApplicationLifetime (const byte* start, const byte* end)
    {
        Require ((start == nullptr and end == nullptr) or (start != nullptr and end != nullptr));
        Require (start <= end);
        return BLOB{_MakeSharedPtr<AdoptAppLifetimeRep_> (start, end)};
    }
    template <size_t SIZE>
    inline BLOB BLOB::AttachApplicationLifetime (const byte (&data)[SIZE])
    {
        return AttachApplicationLifetime (Containers::Start (data), Containers::Start (data) + SIZE);
    }
    template <>
    inline void BLOB::As (span<const byte>* into) const
    {
        RequireNotNull (into);
        Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fThisAssertExternallySynchronized_};
        *into = fRep_->GetBounds ();
    }
    template <>
    inline void BLOB::As (span<const uint8_t>* into) const
    {
        RequireNotNull (into);
        Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fThisAssertExternallySynchronized_};
        auto                                               s = fRep_->GetBounds ();
        *into                                                = span<const uint8_t>{reinterpret_cast<const uint8_t*> (SafeBegin (s)), s.size ()};
    }
    template <>
    inline vector<byte> BLOB::As () const
    {
        vector<byte> result;
        As (&result);
        return result;
    }
    template <>
    inline vector<uint8_t> BLOB::As () const
    {
        vector<uint8_t> result;
        As (&result);
        return result;
    }
    template <>
    inline span<const byte> BLOB::As () const
    {
        span<const byte>                                   result;
        Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fThisAssertExternallySynchronized_};
        As (&result);
        return result;
    }
    template <>
    inline span<const uint8_t> BLOB::As () const
    {
        span<const uint8_t>                                result;
        Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fThisAssertExternallySynchronized_};
        As (&result);
        return result;
    }
    template <>
    inline pair<const byte*, const byte*> BLOB::As () const
    {
        pair<const byte*, const byte*>                     result;
        Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fThisAssertExternallySynchronized_};
        As (&result);
        return result;
    }
    template <>
    inline pair<const uint8_t*, const uint8_t*> BLOB::As () const
    {
        pair<const uint8_t*, const uint8_t*>               result;
        Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fThisAssertExternallySynchronized_};
        As (&result);
        return result;
    }
    template <typename T>
    inline T BLOB::As () const
    {
        static_assert (is_trivially_copyable_v<T>);
        Require (size () >= sizeof (T)); // allow object slicing, but not reading garbage data
        return *(reinterpret_cast<const T*> (begin ()));
    }
    template <>
    inline void BLOB::As (vector<byte>* into) const
    {
        RequireNotNull (into);
        Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fThisAssertExternallySynchronized_};
        span<const byte>                                   tmp = fRep_->GetBounds ();
        Assert (tmp.begin () <= tmp.end ());
        into->assign (tmp.begin (), tmp.end ());
    }
    template <>
    inline void BLOB::As (vector<uint8_t>* into) const
    {
        RequireNotNull (into);
        Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fThisAssertExternallySynchronized_};
        span<const byte>                                   tmp = fRep_->GetBounds ();
        Assert (tmp.begin () <= tmp.end ());
        into->assign (reinterpret_cast<const uint8_t*> (SafeBegin (tmp)), reinterpret_cast<const uint8_t*> (SafeEnd (tmp)));
    }
    template <>
    inline void BLOB::As (string* into) const
    {
        RequireNotNull (into);
        Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fThisAssertExternallySynchronized_};
        span<const byte>                                   tmp = fRep_->GetBounds ();
        Assert (tmp.begin () <= tmp.end ());
        into->clear ();
        into->assign (reinterpret_cast<const char*> (SafeBegin (tmp)), reinterpret_cast<const char*> (SafeEnd (tmp)));
        Ensure (into->size () == tmp.size ());
    }
    template <>
    inline string BLOB::As () const
    {
        string r;
        As (&r);
        return r;
    }
    template <>
    inline void BLOB::As (pair<const byte*, const byte*>* into) const
    {
        RequireNotNull (into);
        Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fThisAssertExternallySynchronized_};
        auto                                               s = fRep_->GetBounds ();
        *into                                                = make_pair (SafeBegin (s), SafeEnd (s));
    }
    template <>
    inline void BLOB::As (pair<const uint8_t*, const uint8_t*>* into) const
    {
        RequireNotNull (into);
        Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fThisAssertExternallySynchronized_};
        auto                                               s = fRep_->GetBounds ();
        *into                                                = make_pair (reinterpret_cast<const uint8_t*> (SafeBegin (s)), reinterpret_cast<const uint8_t*> (SafeEnd (s)));
    }
    template <typename T>
    inline void BLOB::As (T* into) const
    {
        static_assert (is_trivially_copyable_v<T>);
        Require (size () >= sizeof (T)); // allow object slicing, but not reading garbage data
        (void)::memccpy (into, begin (), sizeof (T));
    }
    inline byte BLOB::operator[] (const size_t i) const
    {
        pair<const byte*, const byte*>                     result;
        Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fThisAssertExternallySynchronized_};
        As<pair<const byte*, const byte*>> (&result);
        Assert (i < static_cast<size_t> (result.second - result.first));
        return result.first[i];
    }
    inline bool BLOB::empty () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fThisAssertExternallySynchronized_};
        span<const byte>                                   tmp = fRep_->GetBounds ();
        Assert (tmp.begin () <= tmp.end ());
        return tmp.empty ();
    }
    inline const byte* BLOB::begin () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fThisAssertExternallySynchronized_};
        return SafeBegin (fRep_->GetBounds ());
    }
    inline const byte* BLOB::end () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fThisAssertExternallySynchronized_};
        return SafeEnd (fRep_->GetBounds ());
    }
    inline size_t BLOB::GetSize () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fThisAssertExternallySynchronized_};
        span<const byte>                                   tmp = fRep_->GetBounds ();
        Assert (tmp.begin () <= tmp.end ());
        return tmp.size ();
    }
    inline size_t BLOB::length () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fThisAssertExternallySynchronized_};
        return GetSize ();
    }
    inline size_t BLOB::size () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fThisAssertExternallySynchronized_};
        return GetSize ();
    }
    inline strong_ordering BLOB::operator<=> (const BLOB& rhs) const
    {
        return TWC_ (*this, rhs);
    }
    inline bool BLOB::operator== (const BLOB& rhs) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadLock readLockL{fThisAssertExternallySynchronized_}; // this pattern of double locking might risk a deadlock for real locks, but these locks are fake to assure externally locked
        Debug::AssertExternallySynchronizedMutex::ReadLock readLockR{rhs.fThisAssertExternallySynchronized_};
        if (fRep_ == rhs.fRep_) {
            return true; // cheap optimization for not super uncommon case
        }
        span<const byte> l     = fRep_->GetBounds ();
        span<const byte> r     = rhs.fRep_->GetBounds ();
        size_t           lSize = l.size ();
        size_t           rSize = r.size ();
        if (lSize != rSize) {
            return false;
        }
        if (lSize == 0) {
            return true; // see http://stackoverflow.com/questions/16362925/can-i-pass-a-null-pointer-to-memcmp -- illegal to pass nullptr to memcmp() even if size 0
        }
        return ::memcmp (SafeBegin (l), SafeBegin (r), lSize) == 0;
    }
    inline BLOB BLOB::operator+ (const BLOB& rhs) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadLock readLock{fThisAssertExternallySynchronized_};
        return BLOB ({*this, rhs});
    }
    inline strong_ordering BLOB::TWC_ (const BLOB& lhs, const BLOB& rhs)
    {
        Debug::AssertExternallySynchronizedMutex::ReadLock readLockL{lhs.fThisAssertExternallySynchronized_}; // this pattern of double locking might risk a deadlock for real locks, but these locks are fake to assure externally locked
        Debug::AssertExternallySynchronizedMutex::ReadLock readLockR{rhs.fThisAssertExternallySynchronized_};
        span<const byte>                                   l            = lhs.fRep_->GetBounds ();
        span<const byte>                                   r            = rhs.fRep_->GetBounds ();
        size_t                                             lSize        = l.size ();
        size_t                                             rSize        = r.size ();
        size_t                                             nCommonBytes = min (lSize, rSize);
        if (nCommonBytes != 0) {
            // see http://stackoverflow.com/questions/16362925/can-i-pass-a-null-pointer-to-memcmp -- illegal to pass nullptr to memcmp() even if size 0
            if (int tmp = ::memcmp (SafeBegin (l), SafeBegin (r), nCommonBytes)) {
                return tmp <=> 0;
            }
        }
        // if tmp is zero, and same size - its really zero. But if lhs shorter than right, say lhs < right
        if (lSize == rSize) {
            return strong_ordering::equal;
        }
        return (lSize < rSize) ? strong_ordering::less : strong_ordering::greater;
    }

}

#endif /*_Stroika_Foundation_Memory_BLOB_inl_*/
