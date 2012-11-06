/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
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


            struct  BLOB::_IRep {
                _IRep ();
                virtual ~_IRep ();
                virtual pair<const Byte*, const Byte*>   GetBounds () const =    0;

                NO_COPY_CONSTRUCTOR(_IRep);
                NO_ASSIGNMENT_OPERATOR(_IRep);
            };



            //  class BLOB::_IRep
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
                virtual pair<const Byte*, const Byte*>   GetBounds () const override;

                DECLARE_USE_BLOCK_ALLOCATION (BasicRep_);
            };

            struct  BLOB::ZeroRep_ : public _IRep {
                virtual pair<const Byte*, const Byte*>   GetBounds () const override;

                inline ZeroRep_ () {
                }
                DECLARE_USE_BLOCK_ALLOCATION (ZeroRep_);
            };



            //  class   BLOB
            inline  BLOB::BLOB ()
                : fRep_ (DEBUG_NEW ZeroRep_ ())
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
            inline  size_t  BLOB::length () const
            {
                pair<const Byte*, const Byte*>   tmp =   fRep_->GetBounds ();
                return tmp.second - tmp.first;
            }
            inline  size_t  BLOB::size () const
            {
                pair<const Byte*, const Byte*>   tmp =   fRep_->GetBounds ();
                return tmp.second - tmp.first;
            }
            inline  bool    operator== (const BLOB& lhs, const BLOB& rhs)
            {
                return lhs.compare (rhs) == 0;
            }
            inline  bool    operator> (const BLOB& lhs, const BLOB& rhs)
            {
                return lhs.compare (rhs) > 0;
            }
            inline  bool    operator< (const BLOB& lhs, const BLOB& rhs)
            {
                return lhs.compare (rhs) < 0;
            }


        }
    }
}


#endif  /*_Stroika_Foundation_Memory_BLOB_inl_*/
