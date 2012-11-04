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
#include    "../Debug/Assertions.h"
#include    "../Execution/ModuleInit.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {

            //  class   BLOB
            inline  BLOB::BLOB ():
                fRep (DEBUG_NEW Rep ())
            {
            }
            inline  BLOB::BLOB (const vector<Byte>& data):
                fRep (DEBUG_NEW Rep ())
            {
                fRep->fData = data;
            }
            inline  BLOB::BLOB (const Byte* start, const Byte* end):
                fRep (DEBUG_NEW Rep ())
            {
                fRep->fData = vector<Byte> (start, end);
            }
#if 0
            inline  BLOB::operator const vector<Byte>& () const
            {
                return fRep->fData;
            }
#endif
            template    <>
            void    BLOB::As (vector<Byte>* into) const
            {
                *into = fRep->fData;
            }
            template    <>
            vector<Byte> BLOB::As () const
            {
                return fRep->fData;
            }
            inline  bool    BLOB::empty () const
            {
                return fRep->fData.size () == 0;
            }
            inline  void    BLOB::clear ()
            {
                if (not empty ()) {
                    fRep = shared_ptr<Rep> (DEBUG_NEW Rep ());
                }
            }
            inline  const Byte* BLOB::begin () const
            {
                return Containers::Start (fRep->fData);
            }
            inline  const Byte* BLOB::end () const
            {
                return Containers::End (fRep->fData);
            }
            inline  size_t  BLOB::GetSize () const
            {
                return fRep->fData.size ();
            }
            inline  size_t  BLOB::length () const
            {
                return fRep->fData.size ();
            }
            inline  size_t  BLOB::size () const
            {
                return fRep->fData.size ();
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
