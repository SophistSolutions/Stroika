/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_BLOB_h_
#define _Stroika_Foundation_Memory_BLOB_h_ 1

#include    "../StroikaPreComp.h"

#include    <memory>
#include    <vector>

#include    "../Configuration/Common.h"
#include    "../Memory/Common.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Beta">Beta</a>
 *
 * TODO:
 *      @todo   Closely consider Streams::TODO.md item about a new Streams::BLOB class. This may replace
 *              several of the BELOW TODO items more elegantly (wthout th eSeekOffsetType change would
 *              might cause some difficultties. So you have Memory::BLOB when you Know i tmust be in ram
 *              nad oyu have  ptr api. And you have Streams::BLOB when it may not fit in RAM!
 *
 *      @todo   Redo API - so its all based on SeekOffsetType - not size_t. Document that if you exceed
 *              available in-RAM storage, no biggie - you just throw bad_alloc. But at least you can
 *              construct and operate on large BLOBs (as streams).
 *              [[NB SEE Streams::BLOB todo/project]]
 *
 *      @todo   Use (or document why not) ICompare API for our compare stuff.
 *
 *      @todo   Add MemoryMappedFileBLOB to Foundation/File section – and have it subclass
 *              BLOB (object slicing) – and have different CTOR, and different virtual Rep
 *              (decide semantics – not clear – readonly)
 *
 *      @todo   Create overloads in the MD5/SHA1 hash code taking BLOBs as arguments.
 *
 *      @todo   Do CTOR that uses iterator start/end not just const Byte* start, const Byte* end.
 *
 *      @todo   BLOB::As<BinaryInputStream>() should be seekable
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            class   BinaryInputStream;


        }
    }
}


namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {


            using   namespace   std;


            using   namespace   Configuration;
            using   namespace   Memory;


            /**
             *  A BLOB is a read-only binary region of memory. Once a BLOB is constructed, the data inside cannot
             *  change.
             */
            class   BLOB {
            public:
                /**
                 *  BLOB() overload with AdoptFlag.eAdopt causes 'move semantics' on the pointer - where
                 *  the BLOB takes over ownership of the pointer, and will call delete[] (start)
                 *  on the 'start' pointer. Note - DANGEROUS IF MISUSED.
                 */
                BLOB ();
                BLOB (const vector<Byte>& data);
                BLOB (const Byte* start, const Byte* end);
                enum class AdoptFlag { eAdopt };
                BLOB (const Byte* start, const Byte* end, AdoptFlag adopt);

            protected:
                struct  _IRep;

                /**
                 * Subclass BLOB, and provider your own 'rep' type, to create more efficient storage.
                 */
                explicit BLOB (const shared_ptr<_IRep>& rep);
                explicit BLOB (shared_ptr<_IRep>&&  rep);

            public:
                /**
                 *  Returns true iff the size of the BLOB is zero.
                 */
                nonvirtual  bool    empty () const;

            public:
                /**
                 *  Convert BLOB losslessly into a standard C++ type (right now just <vector<Byte>,
                 *  or Streams::BinaryInputStream supported)
                 */
                template    <typename   T>
                nonvirtual  T   As () const;
                /**
                 *  Convert BLOB losslessly into a standard C++ type (right now just <vector<Byte>>
                 *  supported)
                 */
                template    <typename   T>
                nonvirtual  void   As (T* into) const;

            public:
                /**
                 *  Pointers returned by begin(), remain valid for the lifetime of the containing BLOB.
                 */
                nonvirtual  const Byte* begin () const;
                /**
                 *  Pointers returned by end(), remain valid for the lifetime of the containing BLOB.
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
                 *  This is like memcmp() - it returns == 0, if the RHS and LHS are the same, and it
                 *  returns < 0 if the first byte where the two regions differ is less than the first byte
                 *  of the RHS (where they differ).
                 */
                nonvirtual  int      compare (const BLOB& rhs) const;

            public:
                /**
                 */
                bool    operator< (const BLOB& rhs) const;
                /**
                 */
                bool    operator> (const BLOB& rhs) const;
                /**
                 */
                bool    operator== (const BLOB& rhs) const;

            protected:
                /**
                 */
                struct  _IRep;

            private:
#if     !qCompilerAndStdLib_Supports_SharedPtrOfPrivateTypes
            public:
#endif
                struct  BasicRep_;
                struct  ZeroRep_ ;
                struct  AdoptRep_ ;

            private:
                shared_ptr<_IRep>   fRep_;
            };


            template    <>
            void    BLOB::As (vector<Byte>* into) const;
            template    <>
            vector<Byte> BLOB::As () const;


            /**
             * This abstract interface defines the behavior of a BLOB.
             */
            struct  BLOB::_IRep {
                _IRep ();
                virtual ~_IRep ();
                virtual pair<const Byte*, const Byte*>   GetBounds () const =    0;

                NO_COPY_CONSTRUCTOR(_IRep);
                NO_ASSIGNMENT_OPERATOR(_IRep);
            };


            template    <>
            Streams::BinaryInputStream BLOB::As () const;


        }
    }
}




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "BLOB.inl"

#endif  /*_Stroika_Foundation_Memory_BLOB_h_*/
