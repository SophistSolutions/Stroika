/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
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


            /*
             ********************************************************************************
             *********************************** BLOB::_IRep ********************************
             ********************************************************************************
             */
            inline  BLOB::_IRep::_IRep ()
            {
            }
            inline  BLOB::_IRep::~_IRep ()
            {
            }


            struct  BLOB::BasicRep_ : public _IRep {
                //  really not sure what size to use???
                //  May not be any universal, good answer...
                //  Remember - users can subclass BLOB, and provider their own
                //  'rep' type tuned to their application.
                SmallStackBuffer<Byte, 64>    fData;

                BasicRep_ (const Byte* start, const Byte* end);
                BasicRep_ (const initializer_list<pair<const Byte*, const Byte*>>& startEndPairs);
                BasicRep_ (const initializer_list<BLOB>& list2Concatenate);

                virtual pair<const Byte*, const Byte*>   GetBounds () const override;

                DECLARE_USE_BLOCK_ALLOCATION (BasicRep_);
            };


            struct  BLOB::ZeroRep_ : public _IRep {
                virtual pair<const Byte*, const Byte*>   GetBounds () const override;
                inline ZeroRep_ () = default;
                DECLARE_USE_BLOCK_ALLOCATION (ZeroRep_);
            };


            struct  BLOB::AdoptRep_ : public _IRep {
                const Byte* fStart;
                const Byte* fEnd;

                AdoptRep_ (const Byte* start, const Byte* end);
                ~AdoptRep_ ();
                virtual pair<const Byte*, const Byte*>   GetBounds () const override;

                DECLARE_USE_BLOCK_ALLOCATION (AdoptRep_);
            };


            struct  BLOB::AdoptAppLifetimeRep_ : public _IRep {
                const Byte* fStart;
                const Byte* fEnd;

                AdoptAppLifetimeRep_ (const Byte* start, const Byte* end);
                virtual pair<const Byte*, const Byte*>   GetBounds () const override;

                DECLARE_USE_BLOCK_ALLOCATION (AdoptAppLifetimeRep_);
            };



            /*
             ********************************************************************************
             ************************************** BLOB ************************************
             ********************************************************************************
             */
            inline  BLOB::BLOB ()
                : fRep_ (DEBUG_NEW ZeroRep_ ())
            {
            }
            inline  BLOB::BLOB (const initializer_list<Byte>& data)
                : fRep_ (DEBUG_NEW BasicRep_ (data.begin (), data.end ()))
            {
            }
            template    <typename CONTAINER_OF_BYTE>
            inline  BLOB::BLOB (const CONTAINER_OF_BYTE& data)
                : fRep_ (DEBUG_NEW BasicRep_ (data.begin (), data.end ()))
            {
            }
            template    <size_t SIZE>
            inline  BLOB::BLOB (const Byte (&data)[SIZE])
                : fRep_ (DEBUG_NEW BasicRep_ (Containers::Start (data), Containers::Start (data) + SIZE))
            {
            }
            template    <size_t SIZE>
            inline  BLOB::BLOB (const array<Byte, SIZE>& data)
                : fRep_ (DEBUG_NEW BasicRep_ (Containers::Start (data), Containers::Start (data) + SIZE))
            {
            }
            inline  BLOB::BLOB (const vector<Byte>& data)
                : fRep_ (DEBUG_NEW BasicRep_ (Containers::Start (data), Containers::End (data)))
            {
            }
            inline  BLOB::BLOB (const Byte* start, const Byte* end)
                : fRep_ (DEBUG_NEW BasicRep_ (start, end))
            {
            }
            inline  BLOB::BLOB (const initializer_list<pair<const Byte*, const Byte*>>& startEndPairs)
                : fRep_ (DEBUG_NEW BasicRep_ (startEndPairs))
            {
            }
            inline  BLOB::BLOB (const initializer_list<BLOB>& list2Concatenate)
                : fRep_ (DEBUG_NEW BasicRep_ (list2Concatenate))
            {
            }
            inline  BLOB::BLOB (const SharedIRep& rep)
                : fRep_ (rep)
            {
            }
            inline  BLOB::BLOB (SharedIRep&& rep)
                : fRep_ (std::move (rep))
            {
            }
            inline  BLOB    BLOB::Attach (const Byte* start, const Byte* end)
            {
                return BLOB (SharedIRep (DEBUG_NEW AdoptRep_ (start, end)));
            }
            inline  BLOB    BLOB::AttachApplicationLifetime (const Byte* start, const Byte* end)
            {
                return BLOB (SharedIRep (DEBUG_NEW AdoptAppLifetimeRep_ (start, end)));
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
            inline  bool    BLOB::empty () const
            {
                pair<const Byte*, const Byte*>   tmp =   fRep_->GetBounds ();
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
                return tmp.second - tmp.first;
            }
            inline  bool    BLOB::operator< (const BLOB& rhs) const
            {
                return compare (rhs) < 0;
            }
            inline  bool    BLOB::operator<= (const BLOB& rhs) const
            {
                return compare (rhs) <= 0;
            }
            inline  bool    BLOB::operator> (const BLOB& rhs) const
            {
                return compare (rhs) > 0;
            }
            inline  bool    BLOB::operator>= (const BLOB& rhs) const
            {
                return compare (rhs) >= 0;
            }
            inline  bool    BLOB::operator== (const BLOB& rhs) const
            {
                return compare (rhs) == 0;
            }
            inline  bool    BLOB::operator!= (const BLOB& rhs) const
            {
                return compare (rhs) != 0;
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


        }
    }
}


#endif  /*_Stroika_Foundation_Memory_BLOB_inl_*/
