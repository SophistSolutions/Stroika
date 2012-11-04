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


namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {

            //  class   BLOB
            inline  BLOB::BLOB ()
                : fRep_ (DEBUG_NEW _IRep ())
            {
            }
            inline  BLOB::BLOB (const vector<Byte>& data)
                : fRep_ (DEBUG_NEW _IRep ())
            {
                fRep_->fData = data;
            }
            inline  BLOB::BLOB (const Byte* start, const Byte* end)
                : fRep_ (DEBUG_NEW _IRep ())
            {
                fRep_->fData = vector<Byte> (start, end);
            }
            template    <>
            inline	void    BLOB::As (vector<Byte>* into) const
            {
                *into = fRep_->fData;
            }
            template    <>
            inline	vector<Byte> BLOB::As () const
            {
                return fRep_->fData;
            }
            inline  bool    BLOB::empty () const
            {
                return fRep_->fData.size () == 0;
            }
            inline  void    BLOB::clear ()
            {
                if (not empty ()) {
                    fRep_ = shared_ptr<_IRep> (DEBUG_NEW _IRep ());
                }
            }
            inline  const Byte* BLOB::begin () const
            {
                return Containers::Start (fRep_->fData);
            }
            inline  const Byte* BLOB::end () const
            {
                return Containers::End (fRep_->fData);
            }
            inline  size_t  BLOB::GetSize () const
            {
                return fRep_->fData.size ();
            }
            inline  size_t  BLOB::length () const
            {
                return fRep_->fData.size ();
            }
            inline  size_t  BLOB::size () const
            {
                return fRep_->fData.size ();
            }
            inline  bool    operator== (const BLOB& lhs, const BLOB& rhs)
            {
                vector<Byte> L   =   lhs.As<vector<Byte>> ();
                vector<Byte> R   =   rhs.As<vector<Byte>> ();
                return L == R;
            }
            inline  bool    operator> (const BLOB& lhs, const BLOB& rhs)
            {
                vector<Byte> L   =   lhs.As<vector<Byte>> ();
                vector<Byte> R   =   rhs.As<vector<Byte>> ();
                return L > R;
            }
            inline  bool    operator< (const BLOB& lhs, const BLOB& rhs)
            {
                vector<Byte> L   =   lhs.As<vector<Byte>> ();
                vector<Byte> R   =   rhs.As<vector<Byte>> ();
                return L < R;
            }


        }
    }
}


#endif  /*_Stroika_Foundation_Memory_BLOB_inl_*/
