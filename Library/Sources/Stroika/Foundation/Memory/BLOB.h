/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_BLOB_h_
#define _Stroika_Foundation_Memory_BLOB_h_ 1

#include    "../StroikaPreComp.h"

#include    <memory>
#include    <vector>

#include    "../Configuration/Common.h"



/**
 * TODO:
 *
 *      @todo   Add MemoryMappedFileBLOB to Foundation/File section – and have it subclass
 *              BLOB (object slicing) – and have different CTOR, and different virtual Rep
 *              (decide semantics – not clear – readonly)
 *
 *      @todo   Think since all blobs readonly, must deifne this to only allow being applied to readonly files.
 *              Not sure always possible? Cuz of file corruption (but how is that difernt htna memory corruption?).
 *              Not sure how to doucmetn/deal with this.
 *
 *      @todo   Document that BLOBs are ALWAYS readonly. They can NEVER be changed (but of course you can create new ones).
 *              This is crucial for caching reasons. Copy by value semantics, but often copy by reference perofmrance (refcounted).
 *
 *      @todo   Create overloads in the MD5/SHA1 hash code taking BLOBs as arguments.
 *
 *      @todo   Do CTOR that uses iterator start/end not just const Byte* start, const Byte* end.
 *
 */



//TODO - fix this - this is a temporary hack to get stuff compiling
#ifndef qWierdPrivateBLOBBug
#define qWierdPrivateBLOBBug 1
#endif



namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {

            using   namespace   std;

            using   namespace   Stroika::Foundation;
            using   namespace   Stroika::Foundation::Configuration;
            using   namespace   Stroika::Foundation::Memory;


            /**
			 *	A BLOB is a read-only binary region of memory. Once a BLOB is constructed, the data inside cannot
			 *	change.
             */
            class   BLOB {
            public:
                /**
                 */
                BLOB ();
                BLOB (const vector<Byte>& data);
                BLOB (const Byte* start, const Byte* end);

            protected:
                struct  _IRep;

                /**
                 * Subclass BLOB, and provider your own 'rep' type, to create more efficient storage.
                 */
                BLOB (const shared_ptr<_IRep>& rep);

            public:
                /**
				 *	Returns true iff the size of the BLOB is zero.
                 */
                nonvirtual  bool    empty () const;

            public:
                /**
                 *  Convert BLOB losslessly into a standard C++ type (right now just <vector<Byte>>
                 *  supported)
                 */
                template    <typename   T>
                nonvirtual  T   As () const;
                template    <typename   T>
                nonvirtual  void   As (T* into) const;

            public:
                /**
				 *
				 *	Pointers returned by begin(), remain valid for the lifetime of the containing BLOB.
                 */
                nonvirtual  const Byte* begin () const;
                /**
				 *	Pointers returned by end(), remain valid for the lifetime of the containing BLOB.
                 */
                nonvirtual  const Byte* end () const;

            public:
                /**
                 */
                nonvirtual  size_t      GetSize () const;
                /**
                 */
                nonvirtual  size_t      size () const;
                /**
                 */
                nonvirtual  size_t      length () const;

            public:
                /**
				 *	This is like memcmp() - it returns == 0, if the RHS and LHS are the same, and it
				 *	returns < 0 if the first byte where the two regions differ is less than the first byte
				 *	of the RHS (where they differ).
                 */
                nonvirtual  int      compare (const BLOB& rhs) const;

            protected:
                /**
                 */
                struct  _IRep;

            private:
#if qWierdPrivateBLOBBug
            public:
#endif
                struct  BasicRep_;
                struct  ZeroRep_ ;

            private:
                shared_ptr<_IRep>   fRep_;
            };
            /**
             */
            bool    operator< (const BLOB& lhs, const BLOB& rhs);
            /**
             */
            bool    operator> (const BLOB& lhs, const BLOB& rhs);
            /**
             */
            bool    operator== (const BLOB& lhs, const BLOB& rhs);

            template    <>
            void    BLOB::As (vector<Byte>* into) const;
            template    <>
            vector<Byte> BLOB::As () const;

        }
    }
}
#endif  /*_Stroika_Foundation_Memory_BLOB_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "BLOB.inl"
