/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
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

    struct BLOB::BasicRep_ final : public _IRep, public Memory::UseBlockAllocationIfAppropriate<BasicRep_> {
        //  really not sure what size to use???
        //  May not be any universal, good answer...
        //  Remember - users can subclass BLOB, and provider their own
        //  'rep' type tuned to their application.
        InlineBuffer<byte, 64> fData;

        BasicRep_ (span<const byte> s)
            : fData{Memory::eUninitialized, s.size ()}
        {
            Memory::CopySpanData (s, span{fData});
        }

        BasicRep_ (const initializer_list<pair<const byte*, const byte*>>& startEndPairs);
        BasicRep_ (const initializer_list<BLOB>& list2Concatenate);
        BasicRep_ (const BasicRep_&)            = delete;
        BasicRep_& operator= (const BasicRep_&) = delete;

        virtual span<const byte> GetBounds () const override;
    };

    struct BLOB::ZeroRep_ final : public _IRep, public Memory::UseBlockAllocationIfAppropriate<ZeroRep_> {
        virtual span<const byte> GetBounds () const override;
        ZeroRep_ ()                           = default;
        ZeroRep_ (const ZeroRep_&)            = delete;
        ZeroRep_& operator= (const ZeroRep_&) = delete;
    };

    struct BLOB::AdoptRep_ final : public _IRep, public Memory::UseBlockAllocationIfAppropriate<AdoptRep_> {
        const byte* fStart;
        const byte* fEnd;

        AdoptRep_ (const AdoptRep_&) = delete;
        AdoptRep_ (const byte* start, const byte* end);
        ~AdoptRep_ ()                                         = default;
        AdoptRep_&               operator= (const AdoptRep_&) = delete;
        virtual span<const byte> GetBounds () const override;
    };

    struct BLOB::AdoptAndDeleteRep_ final : public _IRep, public Memory::UseBlockAllocationIfAppropriate<AdoptAndDeleteRep_> {
        const byte* fStart;
        const byte* fEnd;

        AdoptAndDeleteRep_ (const AdoptAndDeleteRep_&) = delete;
        AdoptAndDeleteRep_ (const byte* start, const byte* end);
        ~AdoptAndDeleteRep_ ();
        AdoptAndDeleteRep_&      operator= (const AdoptAndDeleteRep_&) = delete;
        virtual span<const byte> GetBounds () const override;
    };

    /*
     ********************************************************************************
     ************************************** BLOB ************************************
     ********************************************************************************
     */
    inline BLOB::BLOB ()
        : fRep_{MakeSharedPtr<ZeroRep_> ()}
    {
    }
    template <ranges::range CONTAINER_OF_BYTE>
    inline BLOB::BLOB (const CONTAINER_OF_BYTE& data)
        requires (is_convertible_v<typename CONTAINER_OF_BYTE::value_type, byte> or is_convertible_v<typename CONTAINER_OF_BYTE::value_type, uint8_t>)
        : BLOB{as_bytes (span{data.data (), data.size ()})}
    {
    }
    inline BLOB::BLOB (const initializer_list<byte>& bytes)
        : BLOB{span{bytes}}
    {
    }
    inline BLOB::BLOB (const initializer_list<uint8_t>& bytes)
        : BLOB{as_bytes (span{bytes})}
    {
    }
    inline BLOB::BLOB (span<const byte> s)
        : fRep_{s.empty () ? shared_ptr<_IRep>{MakeSharedPtr<ZeroRep_> ()} : shared_ptr<_IRep>{MakeSharedPtr<BasicRep_> (s)}}
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
        : fRep_{MakeSharedPtr<BasicRep_> (startEndPairs)}
    {
    }
    inline BLOB::BLOB (const initializer_list<BLOB>& list2Concatenate)
        : fRep_{MakeSharedPtr<BasicRep_> (list2Concatenate)}
    {
    }
    inline BLOB::BLOB (const shared_ptr<_IRep>& rep)
        : fRep_{rep}
    {
    }
    inline BLOB::BLOB (shared_ptr<_IRep>&& rep)
        : fRep_{move (rep)}
    {
    }
    inline BLOB BLOB::FromHex (const char* b)
    {
        RequireNotNull (b);
        return FromHex (b, b + ::strlen (b));
    }
    inline BLOB BLOB::FromHex (const char* s, const char* e)
    {
        return FromHex (span{s, e});
    }
    inline BLOB BLOB::FromHex (string_view s)
    {
        return FromHex (span{s});
    }
    template <typename T>
    inline BLOB BLOB::FromRaw (const T* s, const T* e)
        requires (is_trivially_copyable_v<T>)
    {
        return BLOB{span{reinterpret_cast<const byte*> (s), reinterpret_cast<const byte*> (e)}};
    }
    template <typename T>
    inline BLOB BLOB::FromRaw (const T* s, size_t sz)
        requires (is_trivially_copyable_v<T>)
    {
        return BLOB{span{reinterpret_cast<const byte*> (s), sz}};
    }
    template <typename T>
    inline BLOB BLOB::FromRaw (const T* s)
        requires (is_same_v<typename char_traits<T>::char_type, T>)
    {
        RequireNotNull (s);
        return FromRaw (s, s + char_traits<T>::length (s));
    }
    template <typename T>
    inline BLOB BLOB::FromRaw (const basic_string<T>& s)
        requires (is_same_v<typename char_traits<T>::char_type, T>)
    {
        return FromRaw (s.c_str (), s.c_str () + s.length ());
    }
    template <typename T>
    inline BLOB BLOB::FromRaw (const T& s)
        requires (is_trivially_copyable_v<T>)
    {
        return FromRaw (&s, &s + 1);
    }
    template <typename BYTEISH, size_t EXTENT>
    inline BLOB BLOB::Attach (span<BYTEISH, EXTENT> s)
        requires (convertible_to<BYTEISH, const byte> or convertible_to<BYTEISH, const uint8_t>)
    {
        const byte* b = reinterpret_cast<const byte*> (s.data ());
        return BLOB{MakeSharedPtr<AdoptRep_> (b, b + s.size ())};
    }
    template <typename BYTEISH, size_t EXTENT>
    inline BLOB BLOB::Attach (BYTEISH (&data)[EXTENT])
        requires (convertible_to<BYTEISH, const byte> or convertible_to<BYTEISH, const uint8_t>)
    {
        return Attach (span<const BYTEISH>{data, EXTENT});
    }
    inline BLOB BLOB::AttachAndDelete (const byte* b, size_t arrayLen)
    {
        return BLOB{MakeSharedPtr<AdoptAndDeleteRep_> (b, b + arrayLen)};
    }
    template <>
    inline void BLOB::As (span<const byte>* into) const
    {
        RequireNotNull (into);
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
        *into = fRep_->GetBounds ();
    }
    template <>
    inline void BLOB::As (span<const uint8_t>* into) const
    {
        RequireNotNull (into);
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
        auto                                                  s = fRep_->GetBounds ();
        *into = span<const uint8_t>{reinterpret_cast<const uint8_t*> (s.data ()), s.size ()};
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
        span<const byte>                                      result;
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
        As (&result);
        return result;
    }
    template <>
    inline span<const uint8_t> BLOB::As () const
    {
        span<const uint8_t>                                   result;
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
        As (&result);
        return result;
    }
    template <>
    inline pair<const byte*, const byte*> BLOB::As () const
    {
        pair<const byte*, const byte*>                        result;
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
        As (&result);
        return result;
    }
    template <>
    inline pair<const uint8_t*, const uint8_t*> BLOB::As () const
    {
        pair<const uint8_t*, const uint8_t*>                  result;
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
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
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
        span<const byte>                                      tmp = fRep_->GetBounds ();
        Assert (tmp.begin () <= tmp.end ());
        into->assign (tmp.begin (), tmp.end ());
    }
    template <>
    inline void BLOB::As (vector<uint8_t>* into) const
    {
        RequireNotNull (into);
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
        span<const byte>                                      tmp = fRep_->GetBounds ();
        Assert (tmp.begin () <= tmp.end ());
        into->assign (reinterpret_cast<const uint8_t*> (tmp.data ()), reinterpret_cast<const uint8_t*> (tmp.data () + tmp.size ()));
    }
    template <>
    inline void BLOB::As (string* into) const
    {
        RequireNotNull (into);
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
        span<const byte>                                      tmp = fRep_->GetBounds ();
        Assert (tmp.begin () <= tmp.end ());
        into->clear ();
        into->assign (reinterpret_cast<const char*> (tmp.data ()), reinterpret_cast<const char*> (tmp.data () + tmp.size ()));
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
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
        auto                                                  s = fRep_->GetBounds ();
        *into                                                   = make_pair (s.data (), s.data () + s.size ());
    }
    template <>
    inline void BLOB::As (pair<const uint8_t*, const uint8_t*>* into) const
    {
        RequireNotNull (into);
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
        span<const byte>                                      s = fRep_->GetBounds ();
        *into = make_pair (reinterpret_cast<const uint8_t*> (s.data ()), reinterpret_cast<const uint8_t*> (s.data () + s.size ()));
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
        pair<const byte*, const byte*>                        result;
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
        As<pair<const byte*, const byte*>> (&result);
        Assert (i < static_cast<size_t> (result.second - result.first));
        return result.first[i];
    }
    inline bool BLOB::empty () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
        span<const byte>                                      tmp = fRep_->GetBounds ();
        Assert (tmp.begin () <= tmp.end ());
        return tmp.empty ();
    }
    inline const byte* BLOB::begin () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
        return fRep_->GetBounds ().data ();
    }
    inline const byte* BLOB::end () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
        auto                                                  b = fRep_->GetBounds ();
        return b.data () + b.size ();
    }
    inline size_t BLOB::GetSize () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
        span<const byte>                                      tmp = fRep_->GetBounds ();
        Assert (tmp.begin () <= tmp.end ());
        return tmp.size ();
    }
    inline size_t BLOB::length () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
        return GetSize ();
    }
    inline const byte* BLOB::data () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
        return begin ();
    }
    inline size_t BLOB::size () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
        return GetSize ();
    }
    inline strong_ordering BLOB::operator<=> (const BLOB& rhs) const
    {
        return TWC_ (*this, rhs);
    }
    inline bool BLOB::operator== (const BLOB& rhs) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext readLockL{fThisAssertExternallySynchronized_}; // this pattern of double locking might risk a deadlock for real locks, but these locks are fake to assure externally locked
        Debug::AssertExternallySynchronizedMutex::ReadContext readLockR{rhs.fThisAssertExternallySynchronized_};
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
        return MemCmp (l, r) == 0;
    }
    inline BLOB BLOB::operator+ (const BLOB& rhs) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fThisAssertExternallySynchronized_};
        return BLOB ({*this, rhs});
    }
    inline strong_ordering BLOB::TWC_ (const BLOB& lhs, const BLOB& rhs)
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext readLockL{lhs.fThisAssertExternallySynchronized_}; // this pattern of double locking might risk a deadlock for real locks, but these locks are fake to assure externally locked
        Debug::AssertExternallySynchronizedMutex::ReadContext readLockR{rhs.fThisAssertExternallySynchronized_};
        span<const byte>                                      l            = lhs.fRep_->GetBounds ();
        span<const byte>                                      r            = rhs.fRep_->GetBounds ();
        size_t                                                lSize        = l.size ();
        size_t                                                rSize        = r.size ();
        size_t                                                nCommonBytes = min (lSize, rSize);
        if (nCommonBytes != 0) {
            // see http://stackoverflow.com/questions/16362925/can-i-pass-a-null-pointer-to-memcmp -- illegal to pass nullptr to memcmp() even if size 0
            if (strong_ordering tmp = MemCmp (l.subspan (0, nCommonBytes), r.subspan (0, nCommonBytes)); tmp != strong_ordering::equal) {
                return tmp;
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
