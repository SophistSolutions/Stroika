/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_BLOB_inl_
#define _Stroika_Foundation_Memory_BLOB_inl_   1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Containers/Common.h"
#include    "../Debug/Assertions.h"
#include    "../Memory/Common.h"
#include    "../Memory/BlockAllocated.h"
#include    "../Memory/SmallStackBuffer.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {


            struct  BLOB::BasicRep_ : public _IRep {
                //  really not sure what size to use???
                //  May not be any universal, good answer...
                //  Remember - users can subclass BLOB, and provider their own
                //  'rep' type tuned to their application.
                SmallStackBuffer<Byte, 64>    fData;

                template <typename BYTE_ITERATOR>
                BasicRep_ (BYTE_ITERATOR start, BYTE_ITERATOR end)
                    : fData (end - start)
                {
#if     qSilenceAnnoyingCompilerWarnings && _MSC_VER
                    Memory::Private::VC_BWA_std_copy (start, end, fData.begin ());
#else
                    std::copy (start, end, fData.begin ());
#endif
                }
                BasicRep_ (const Byte* start, const Byte* end)
                    : fData (end - start)
                {
                    (void)::memcpy (fData.begin (), start, end - start);
                }

                BasicRep_ (const initializer_list<pair<const Byte*, const Byte*>>& startEndPairs);
                BasicRep_ (const initializer_list<BLOB>& list2Concatenate);
                BasicRep_(const BasicRep_&) = delete;
                BasicRep_& operator= (const BasicRep_&) = delete;

                virtual pair<const Byte*, const Byte*>   GetBounds () const override;

                DECLARE_USE_BLOCK_ALLOCATION (BasicRep_);
            };


            struct  BLOB::ZeroRep_ : public _IRep {
                virtual pair<const Byte*, const Byte*>   GetBounds () const override;
                ZeroRep_ () = default;
                ZeroRep_(const ZeroRep_&) = delete;
                ZeroRep_& operator= (const ZeroRep_&) = delete;
                DECLARE_USE_BLOCK_ALLOCATION (ZeroRep_);
            };


            struct  BLOB::AdoptRep_ : public _IRep {
                const Byte* fStart;
                const Byte* fEnd;

                AdoptRep_(const AdoptRep_&) = delete;
                AdoptRep_ (const Byte* start, const Byte* end);
                ~AdoptRep_ ();
                AdoptRep_& operator= (const AdoptRep_&) = delete;
                virtual pair<const Byte*, const Byte*>   GetBounds () const override;

                DECLARE_USE_BLOCK_ALLOCATION (AdoptRep_);
            };


            struct  BLOB::AdoptAppLifetimeRep_ : public _IRep {
                const Byte* fStart;
                const Byte* fEnd;

                AdoptAppLifetimeRep_() = delete;
                AdoptAppLifetimeRep_(const AdoptAppLifetimeRep_&) = delete;
                AdoptAppLifetimeRep_ (const Byte* start, const Byte* end);
                AdoptAppLifetimeRep_& operator= (const AdoptAppLifetimeRep_&) = delete;
                virtual pair<const Byte*, const Byte*>   GetBounds () const override;

                DECLARE_USE_BLOCK_ALLOCATION (AdoptAppLifetimeRep_);
            };



            /*
             ********************************************************************************
             ************************************** BLOB ************************************
             ********************************************************************************
             */
            template    <typename T, typename... ARGS_TYPE>
            inline  BLOB::_SharedRepImpl<T>     BLOB::_MakeSharedPtr (ARGS_TYPE&& ... args)
            {
#if     qStroika_Foundation_Memory_BLOBUsesStroikaSharedPtr_
                return Memory::MakeSharedPtr<T> (forward<ARGS_TYPE> (args)...);
#else
                return make_shared<T> (forward<ARGS_TYPE> (args)...);
#endif
            }
            inline  BLOB::BLOB ()
                : fRep_ { _MakeSharedPtr<ZeroRep_> () } {
            }
            inline  BLOB::BLOB (BLOB&& src)
                : fRep_ { move (src.fRep_) } {
            }
            template    <typename CONTAINER_OF_BYTE, typename ENABLE_IF>
            inline  BLOB::BLOB (const CONTAINER_OF_BYTE& data)
                : fRep_ { move ((std::begin (data) == std::end (data)) ? move <_SharedIRep> (_MakeSharedPtr<ZeroRep_> ()) : move<_SharedIRep> (_MakeSharedPtr<BasicRep_> (data.begin (), data.end ()))) } {
            }
            inline  BLOB::BLOB (const Byte* start, const Byte* end)
                : fRep_ (move (start == end ? move<_SharedIRep> (_MakeSharedPtr<ZeroRep_> ()) : move<_SharedIRep> (_MakeSharedPtr<BasicRep_> (start, end))))
            {
            }
            inline  BLOB::BLOB (const initializer_list<pair<const Byte*, const Byte*>>& startEndPairs)
                : fRep_ { _MakeSharedPtr<BasicRep_> (startEndPairs) } {
            }
            inline  BLOB::BLOB (const initializer_list<BLOB>& list2Concatenate)
                : fRep_ { _MakeSharedPtr<BasicRep_> (list2Concatenate) } {
            }
            inline  BLOB::BLOB (const _SharedIRep& rep)
                : fRep_ { rep } {
            }
            inline  BLOB::BLOB (_SharedIRep&& rep)
                : fRep_ (std::move (rep))
            {
            }
            inline  BLOB    BLOB::Attach (const Byte* start, const Byte* end)
            {
                Require ((start == nullptr and end == nullptr) or (start != nullptr and end != nullptr));
                Require (start <= end);
                return BLOB { _MakeSharedPtr<AdoptRep_> (start, end) };
            }
            inline  BLOB    BLOB::AttachApplicationLifetime (const Byte* start, const Byte* end)
            {
                Require ((start == nullptr and end == nullptr) or (start != nullptr and end != nullptr));
                Require (start <= end);
                return BLOB { _MakeSharedPtr<AdoptAppLifetimeRep_> (start, end) };
            }
            template    <size_t SIZE>
            inline  BLOB    AttachApplicationLifetime (const Byte (&data)[SIZE])
            {
                return AttachApplicationLifetime (Containers::Start (data), Containers::Start (data) + SIZE);
            }
            template    <>
            inline  void    BLOB::As (vector<Byte>* into) const
            {
                RequireNotNull (into);
                pair<const Byte*, const Byte*>   tmp =   fRep_->GetBounds ();
                Assert (tmp.first <= tmp.second);
                into->clear ();
                into->insert (into->begin (), tmp.first, tmp.second);
            }
            template    <>
            inline  vector<Byte> BLOB::As () const
            {
                vector<Byte>    result;
                As<vector<Byte>> (&result);
                return result;
            }
            template    <>
            inline  void    BLOB::As (pair<const Byte*, const Byte*>* into) const
            {
                RequireNotNull (into);
                *into = fRep_->GetBounds ();
            }
            template    <>
            inline  pair<const Byte*, const Byte*> BLOB::As () const
            {
                pair<const Byte*, const Byte*>    result;
                As<pair<const Byte*, const Byte*>> (&result);
                return result;
            }
            inline  bool    BLOB::empty () const
            {
                pair<const Byte*, const Byte*>   tmp =   fRep_->GetBounds ();
                Assert (tmp.first <= tmp.second);
                return tmp.first == tmp.second;
            }
            inline  const Byte* BLOB::begin () const
            {
                return fRep_->GetBounds ().first;
            }
            inline  const Byte* BLOB::end () const
            {
                return fRep_->GetBounds ().second;
            }
            inline  size_t  BLOB::GetSize () const
            {
                pair<const Byte*, const Byte*>   tmp =   fRep_->GetBounds ();
                Assert (tmp.first <= tmp.second);
                return tmp.second - tmp.first;
            }
            inline  int  BLOB::compare (const BLOB& rhs) const
            {
                return Compare (rhs);
            }
            inline  size_t  BLOB::length () const
            {
                return GetSize ();
            }
            inline  size_t  BLOB::size () const
            {
                return GetSize ();
            }
            inline  BLOB    BLOB::operator+ (const BLOB& rhs) const
            {
                return BLOB ({ * this, rhs });
            }
            inline  bool    BLOB::Equals (const BLOB& rhs) const
            {
                if (fRep_ == rhs.fRep_) {
                    return true;    // cheap optimization for not super uncommon case
                }
                pair<const Byte*, const Byte*>   l =   fRep_->GetBounds ();
                pair<const Byte*, const Byte*>   r =   rhs.fRep_->GetBounds ();
                size_t  lSize = l.second - l.first;
                size_t  rSize = r.second - r.first;
                if (lSize != rSize) {
                    return false;
                }
                return ::memcmp (l.first, r.first, lSize) == 0;
            }


            /*
             ********************************************************************************
             ****************************** BLOB operators **********************************
             ********************************************************************************
             */
            inline  bool    operator< (const BLOB& lhs, const BLOB& rhs)
            {
                return lhs.Compare (rhs) < 0;
            }
            inline  bool    operator<= (const BLOB& lhs, const BLOB& rhs)
            {
                return lhs.Compare (rhs) <= 0;
            }
            inline  bool    operator== (const BLOB& lhs, const BLOB& rhs)
            {
                return lhs.Equals (rhs);
            }
            inline  bool    operator!= (const BLOB& lhs, const BLOB& rhs)
            {
                return not lhs.Equals (rhs);
            }
            inline  bool    operator>= (const BLOB& lhs, const BLOB& rhs)
            {
                return lhs.Compare (rhs) >= 0;
            }
            inline  bool    operator> (const BLOB& lhs, const BLOB& rhs)
            {
                return lhs.Compare (rhs) > 0;
            }


        }
    }
}


#endif  /*_Stroika_Foundation_Memory_BLOB_inl_*/
