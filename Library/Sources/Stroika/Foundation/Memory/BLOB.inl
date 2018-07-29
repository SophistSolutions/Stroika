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
        SmallStackBuffer<Byte, 64> fData;

        template <typename BYTE_ITERATOR>
        BasicRep_ (BYTE_ITERATOR start, BYTE_ITERATOR end)
            : fData (end - start)
        {
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
            Memory::Private::VC_BWA_std_copy (start, end, fData.begin ());
#else
            copy (start, end, fData.begin ());
#endif
        }
        BasicRep_ (const Byte* start, const Byte* end)
            : fData (end - start)
        {
            (void)::memcpy (fData.begin (), start, end - start);
        }

        BasicRep_ (const initializer_list<pair<const Byte*, const Byte*>>& startEndPairs);
        BasicRep_ (const initializer_list<BLOB>& list2Concatenate);
        BasicRep_ (const BasicRep_&) = delete;
        BasicRep_& operator= (const BasicRep_&) = delete;

        virtual pair<const Byte*, const Byte*> GetBounds () const override;
    };

    struct BLOB::ZeroRep_ : public _IRep, public Memory::UseBlockAllocationIfAppropriate<ZeroRep_> {
        virtual pair<const Byte*, const Byte*> GetBounds () const override;
        ZeroRep_ ()                = default;
        ZeroRep_ (const ZeroRep_&) = delete;
        ZeroRep_& operator= (const ZeroRep_&) = delete;
    };

    struct BLOB::AdoptRep_ : public _IRep, public Memory::UseBlockAllocationIfAppropriate<AdoptRep_> {
        const Byte* fStart;
        const Byte* fEnd;

        AdoptRep_ (const AdoptRep_&) = delete;
        AdoptRep_ (const Byte* start, const Byte* end);
        ~AdoptRep_ ();
        AdoptRep_&                             operator= (const AdoptRep_&) = delete;
        virtual pair<const Byte*, const Byte*> GetBounds () const override;
    };

    struct BLOB::AdoptAppLifetimeRep_ : public _IRep, public Memory::UseBlockAllocationIfAppropriate<AdoptAppLifetimeRep_> {
        const Byte* fStart;
        const Byte* fEnd;

        AdoptAppLifetimeRep_ ()                            = delete;
        AdoptAppLifetimeRep_ (const AdoptAppLifetimeRep_&) = delete;
        AdoptAppLifetimeRep_ (const Byte* start, const Byte* end);
        AdoptAppLifetimeRep_&                  operator= (const AdoptAppLifetimeRep_&) = delete;
        virtual pair<const Byte*, const Byte*> GetBounds () const override;
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
    inline BLOB::BLOB (BLOB&& src)
        : fRep_{move (src.fRep_)}
    {
    }
    template <typename CONTAINER_OF_BYTE, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_BYTE> and is_convertible_v<typename CONTAINER_OF_BYTE::value_type, Byte>>*>
    inline BLOB::BLOB (const CONTAINER_OF_BYTE& data)
        : fRep_{move ((std::begin (data) == std::end (data)) ? move<_SharedIRep> (_MakeSharedPtr<ZeroRep_> ()) : move<_SharedIRep> (_MakeSharedPtr<BasicRep_> (data.begin (), data.end ())))}
    {
    }
    inline BLOB::BLOB (const initializer_list<Byte>& bytes)
        : fRep_ (move (bytes.size () == 0 ? move<_SharedIRep> (_MakeSharedPtr<ZeroRep_> ()) : move<_SharedIRep> (_MakeSharedPtr<BasicRep_> (bytes.begin (), bytes.end ()))))
    {
    }
    inline BLOB::BLOB (const Byte* start, const Byte* end)
        : fRep_ (move (start == end ? move<_SharedIRep> (_MakeSharedPtr<ZeroRep_> ()) : move<_SharedIRep> (_MakeSharedPtr<BasicRep_> (start, end))))
    {
    }
    inline BLOB::BLOB (const initializer_list<pair<const Byte*, const Byte*>>& startEndPairs)
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
        return BLOB (reinterpret_cast<const Byte*> (s), reinterpret_cast<const Byte*> (e));
    }
    template <typename T>
    inline BLOB BLOB::Raw (const T* s, size_t sz)
    {
        return BLOB (reinterpret_cast<const Byte*> (s), reinterpret_cast<const Byte*> (s + sz));
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
    inline BLOB BLOB::Attach (const Byte* start, const Byte* end)
    {
        Require ((start == nullptr and end == nullptr) or (start != nullptr and end != nullptr));
        Require (start <= end);
        return BLOB{_MakeSharedPtr<AdoptRep_> (start, end)};
    }
    inline BLOB BLOB::AttachApplicationLifetime (const Byte* start, const Byte* end)
    {
        Require ((start == nullptr and end == nullptr) or (start != nullptr and end != nullptr));
        Require (start <= end);
        return BLOB{_MakeSharedPtr<AdoptAppLifetimeRep_> (start, end)};
    }
    template <size_t SIZE>
    inline BLOB BLOB::AttachApplicationLifetime (const Byte (&data)[SIZE])
    {
        return AttachApplicationLifetime (Containers::Start (data), Containers::Start (data) + SIZE);
    }
    template <>
    inline void BLOB::As (vector<Byte>* into) const
    {
        RequireNotNull (into);
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        pair<const Byte*, const Byte*>                      tmp = fRep_->GetBounds ();
        Assert (tmp.first <= tmp.second);
        into->clear ();
        into->insert (into->begin (), tmp.first, tmp.second);
    }
    template <>
    inline vector<Byte> BLOB::As () const
    {
        vector<Byte>                                        result;
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        As<vector<Byte>> (&result);
        return result;
    }
    template <>
    inline void BLOB::As (pair<const Byte*, const Byte*>* into) const
    {
        RequireNotNull (into);
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        *into = fRep_->GetBounds ();
    }
    template <>
    inline pair<const Byte*, const Byte*> BLOB::As () const
    {
        pair<const Byte*, const Byte*>                      result;
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        As<pair<const Byte*, const Byte*>> (&result);
        return result;
    }
    inline bool BLOB::empty () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        pair<const Byte*, const Byte*>                      tmp = fRep_->GetBounds ();
        Assert (tmp.first <= tmp.second);
        return tmp.first == tmp.second;
    }
    inline const Byte* BLOB::begin () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return fRep_->GetBounds ().first;
    }
    inline const Byte* BLOB::end () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return fRep_->GetBounds ().second;
    }
    inline size_t BLOB::GetSize () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        pair<const Byte*, const Byte*>                      tmp = fRep_->GetBounds ();
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
        pair<const Byte*, const Byte*> l     = fRep_->GetBounds ();
        pair<const Byte*, const Byte*> r     = rhs.fRep_->GetBounds ();
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
