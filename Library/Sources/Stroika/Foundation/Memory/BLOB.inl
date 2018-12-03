/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
#include "../Memory/SmallStackBuffer.h"
#include "../Traversal/Iterator.h"

namespace Stroika::Foundation::Memory {

    struct BLOB::BasicRep_ : public _IRep, public Memory::UseBlockAllocationIfAppropriate<BasicRep_> {
        //  really not sure what size to use???
        //  May not be any universal, good answer...
        //  Remember - users can subclass BLOB, and provider their own
        //  'rep' type tuned to their application.
        SmallStackBuffer<byte, 64> fData;

        template <typename BYTE_ITERATOR>
        BasicRep_ (BYTE_ITERATOR start, BYTE_ITERATOR end)
            : fData (SmallStackBufferCommon::eUninitialized, end - start)
        {
            // use memcpy instead of std::copy because std::copy doesn't work between uint8_t, and byte arrays.
            static_assert (sizeof (*start) == 1);
            static_assert (is_trivially_copyable_v<typename iterator_traits<BYTE_ITERATOR>::value_type>);
            (void)::memcpy (fData.begin (), Traversal::Iterator2Pointer (start), end - start);
        }

        BasicRep_ (const initializer_list<pair<const byte*, const byte*>>& startEndPairs);
        BasicRep_ (const initializer_list<BLOB>& list2Concatenate);
        BasicRep_ (const BasicRep_&) = delete;
        BasicRep_& operator= (const BasicRep_&) = delete;

        virtual pair<const byte*, const byte*> GetBounds () const override;
    };

    struct BLOB::ZeroRep_ : public _IRep, public Memory::UseBlockAllocationIfAppropriate<ZeroRep_> {
        virtual pair<const byte*, const byte*> GetBounds () const override;
        ZeroRep_ ()                = default;
        ZeroRep_ (const ZeroRep_&) = delete;
        ZeroRep_& operator= (const ZeroRep_&) = delete;
    };

    struct BLOB::AdoptRep_ : public _IRep, public Memory::UseBlockAllocationIfAppropriate<AdoptRep_> {
        const byte* fStart;
        const byte* fEnd;

        AdoptRep_ (const AdoptRep_&) = delete;
        AdoptRep_ (const byte* start, const byte* end);
        ~AdoptRep_ ();
        AdoptRep_&                             operator= (const AdoptRep_&) = delete;
        virtual pair<const byte*, const byte*> GetBounds () const override;
    };

    struct BLOB::AdoptAppLifetimeRep_ : public _IRep, public Memory::UseBlockAllocationIfAppropriate<AdoptAppLifetimeRep_> {
        const byte* fStart;
        const byte* fEnd;

        AdoptAppLifetimeRep_ ()                            = delete;
        AdoptAppLifetimeRep_ (const AdoptAppLifetimeRep_&) = delete;
        AdoptAppLifetimeRep_ (const byte* start, const byte* end);
        AdoptAppLifetimeRep_&                  operator= (const AdoptAppLifetimeRep_&) = delete;
        virtual pair<const byte*, const byte*> GetBounds () const override;
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
        else {
            return make_shared<T> (forward<ARGS_TYPE> (args)...);
        }
    }
    inline BLOB::BLOB ()
        : fRep_{_MakeSharedPtr<ZeroRep_> ()}
    {
    }
    inline BLOB::BLOB (BLOB&& src) noexcept
        : fRep_{move (src.fRep_)}
    {
    }
    template <typename CONTAINER_OF_BYTE, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_BYTE> and (is_convertible_v<typename CONTAINER_OF_BYTE::value_type, byte> or is_convertible_v<typename CONTAINER_OF_BYTE::value_type, uint8_t>)>*>
    inline BLOB::BLOB (const CONTAINER_OF_BYTE& data)
        : fRep_{(std::begin (data) == std::end (data)) ? _SharedIRep (_MakeSharedPtr<ZeroRep_> ()) : _SharedIRep (_MakeSharedPtr<BasicRep_> (data.begin (), data.end ()))}
    {
    }
    inline BLOB::BLOB (const initializer_list<byte>& bytes)
        : fRep_ (bytes.size () == 0 ? _SharedIRep (_MakeSharedPtr<ZeroRep_> ()) : _SharedIRep (_MakeSharedPtr<BasicRep_> (bytes.begin (), bytes.end ())))
    {
    }
    inline BLOB::BLOB (const initializer_list<uint8_t>& bytes)
        : fRep_ (bytes.size () == 0 ? _SharedIRep (_MakeSharedPtr<ZeroRep_> ()) : _SharedIRep (_MakeSharedPtr<BasicRep_> (bytes.begin (), bytes.end ())))
    {
    }
    inline BLOB::BLOB (const byte* start, const byte* end)
        : fRep_ (start == end ? _SharedIRep (_MakeSharedPtr<ZeroRep_> ()) : _SharedIRep (_MakeSharedPtr<BasicRep_> (start, end)))
    {
    }
    inline BLOB::BLOB (const uint8_t* start, const uint8_t* end)
        : fRep_ (start == end ? _SharedIRep (_MakeSharedPtr<ZeroRep_> ()) : _SharedIRep (_MakeSharedPtr<BasicRep_> (start, end)))
    {
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
        : fRep_ (move (rep))
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
    template <typename T>
    inline BLOB BLOB::Raw (const T* s, const T* e)
    {
        return BLOB (reinterpret_cast<const byte*> (s), reinterpret_cast<const byte*> (e));
    }
    template <typename T>
    inline BLOB BLOB::Raw (const T* s, size_t sz)
    {
        return BLOB (reinterpret_cast<const byte*> (s), reinterpret_cast<const byte*> (s + sz));
    }
    inline BLOB BLOB::Raw (const char* s)
    {
        RequireNotNull (s);
        return Raw (s, s + strlen (s));
    }
    inline BLOB BLOB::Raw (const wchar_t* s)
    {
        RequireNotNull (s);
        return Raw (s, s + ::wcslen (s));
    }
    template <typename CONTAINER_OF_POD_T, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_POD_T> and is_pod_v<typename CONTAINER_OF_POD_T::value_type>>*>
    inline BLOB BLOB::Raw (const CONTAINER_OF_POD_T& s)
    {
        // note we use .size () instead of s.end () because this funtion requires argument CONTAINER to be contiguous, and thats more likely checked by this (really need some concept check)
        // also - Traversal::Iterator2Pointer (s.end ()) generally crashes in debug mode - windows - _ITERATOR_DEBUG_LEVEL >= 1
        return s.empty () ? BLOB{} : Raw (Traversal::Iterator2Pointer (s.begin ()), Traversal::Iterator2Pointer (s.begin ()) + s.size ());
    }
    inline BLOB BLOB::Attach (const byte* start, const byte* end)
    {
        Require ((start == nullptr and end == nullptr) or (start != nullptr and end != nullptr));
        Require (start <= end);
        return BLOB{_MakeSharedPtr<AdoptRep_> (start, end)};
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
    inline void BLOB::As (vector<byte>* into) const
    {
        RequireNotNull (into);
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        pair<const byte*, const byte*>                      tmp = fRep_->GetBounds ();
        Assert (tmp.first <= tmp.second);
        into->assign (tmp.first, tmp.second);
    }
    template <>
    inline void BLOB::As (vector<uint8_t>* into) const
    {
        RequireNotNull (into);
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        pair<const byte*, const byte*>                      tmp = fRep_->GetBounds ();
        Assert (tmp.first <= tmp.second);
        into->assign (reinterpret_cast<const uint8_t*> (tmp.first), reinterpret_cast<const uint8_t*> (tmp.second));
    }
    template <>
    inline vector<byte> BLOB::As () const
    {
        vector<byte> result;
        As<vector<byte>> (&result);
        return result;
    }
    template <>
    inline vector<uint8_t> BLOB::As () const
    {
        vector<uint8_t> result;
        As<vector<uint8_t>> (&result);
        return result;
    }
    template <>
    inline void BLOB::As (pair<const byte*, const byte*>* into) const
    {
        RequireNotNull (into);
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        *into = fRep_->GetBounds ();
    }
    template <>
    inline pair<const byte*, const byte*> BLOB::As () const
    {
        pair<const byte*, const byte*>                      result;
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        As<pair<const byte*, const byte*>> (&result);
        return result;
    }
    inline byte BLOB::operator[] (const size_t i) const
    {
        pair<const byte*, const byte*>                      result;
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        As<pair<const byte*, const byte*>> (&result);
        Assert (i < result.second - result.first);
        return result.first[i];
    }
    inline bool BLOB::empty () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        pair<const byte*, const byte*>                      tmp = fRep_->GetBounds ();
        Assert (tmp.first <= tmp.second);
        return tmp.first == tmp.second;
    }
    inline const byte* BLOB::begin () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return fRep_->GetBounds ().first;
    }
    inline const byte* BLOB::end () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return fRep_->GetBounds ().second;
    }
    inline size_t BLOB::GetSize () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        pair<const byte*, const byte*>                      tmp = fRep_->GetBounds ();
        Assert (tmp.first <= tmp.second);
        return tmp.second - tmp.first;
    }
    inline int BLOB::compare (const BLOB& rhs) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return Compare (rhs);
    }
    inline size_t BLOB::length () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return GetSize ();
    }
    inline size_t BLOB::size () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return GetSize ();
    }
    inline BLOB BLOB::operator+ (const BLOB& rhs) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return BLOB ({*this, rhs});
    }
    inline bool BLOB::Equals (const BLOB& rhs) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        shared_lock<const AssertExternallySynchronizedLock> critSec2{rhs};
        if (fRep_ == rhs.fRep_) {
            return true; // cheap optimization for not super uncommon case
        }
        pair<const byte*, const byte*> l     = fRep_->GetBounds ();
        pair<const byte*, const byte*> r     = rhs.fRep_->GetBounds ();
        size_t                         lSize = l.second - l.first;
        size_t                         rSize = r.second - r.first;
        if (lSize != rSize) {
            return false;
        }
        if (lSize == 0) {
            return true; // see http://stackoverflow.com/questions/16362925/can-i-pass-a-null-pointer-to-memcmp -- illegal to pass nullptr to memcmp() even if size 0
        }
        return ::memcmp (l.first, r.first, lSize) == 0;
    }

    /*
     ********************************************************************************
     ****************************** BLOB operators **********************************
     ********************************************************************************
     */
    inline bool operator< (const BLOB& lhs, const BLOB& rhs)
    {
        return lhs.Compare (rhs) < 0;
    }
    inline bool operator<= (const BLOB& lhs, const BLOB& rhs)
    {
        return lhs.Compare (rhs) <= 0;
    }
    inline bool operator== (const BLOB& lhs, const BLOB& rhs)
    {
        return lhs.Equals (rhs);
    }
    inline bool operator!= (const BLOB& lhs, const BLOB& rhs)
    {
        return not lhs.Equals (rhs);
    }
    inline bool operator>= (const BLOB& lhs, const BLOB& rhs)
    {
        return lhs.Compare (rhs) >= 0;
    }
    inline bool operator> (const BLOB& lhs, const BLOB& rhs)
    {
        return lhs.Compare (rhs) > 0;
    }

}

#endif /*_Stroika_Foundation_Memory_BLOB_inl_*/
