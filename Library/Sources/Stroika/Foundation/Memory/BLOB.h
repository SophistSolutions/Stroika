/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_BLOB_h_
#define _Stroika_Foundation_Memory_BLOB_h_ 1

#include    "../StroikaPreComp.h"

#include    <array>
#include    <memory>
#include    <vector>

#include    "../Configuration/Common.h"
#include    "../Memory/Common.h"
#include    "../Memory/SharedPtr.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Beta">Beta</a>
 *
 * TODO:
 *      @todo   THREADSAFETY BUG - shared_ptr<> not threadsafe, so our envelope needs
 *              to be fixed.
 *
 *              FIX AND DOCUMENT: thresafeaty on assignment!
 *
 *      @todo   SHOULD add template CTOR args - but must be careful to say iterator <Byte> and
 *              only (or handle differntly) random access iterators versus just plain forward iteraotrs.
 *
 *      @todo   Consider the name Attach() and AttachApplicationLifetime (). This was meant to parallel
 *              what we do with Socket::Attach(). Would Adopt() be a better name (in all cases?).
 *
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


            /**
             *  \def qStroika_Foundation_Memory_BLOBUsesStroikaSharedPtr
             *      If true, use Stroika's SharedPtr<> in place of std::shared_ptr<>. This is an
             *      internal implementaiton detail, and may go away as an option.
             *
             *      Empirically, this was slightly faster in the performance regression test.
             */
#ifndef qStroika_Foundation_Memory_BLOBUsesStroikaSharedPtr_
#define qStroika_Foundation_Memory_BLOBUsesStroikaSharedPtr_   1
#endif


            using   namespace   std;


            using   namespace   Configuration;
            using   namespace   Memory;


            /**
             *  A BLOB is a read-only binary region of memory. Once a BLOB is constructed, the data inside cannot
             *  change (except by assignement - being assigned over).
             *
             *  @todo - FIX AND DOCUMENTED FIXED - thresafeaty on assignment! WILL BE SAFE CUZ READONLY (once i fix shared_ptr copy issue)
             *
             *  \note   See coding conventions document about operator usage: Compare () and operator<, operator>, etc
             */
            class   BLOB {
            public:
                /**
                 */
                BLOB ();
                BLOB (const BLOB& src) = default;
                template    <size_t SIZE>
                BLOB (const Byte (&data)[SIZE]);
                BLOB (const initializer_list<Byte>& data);
                template    <size_t SIZE>
                BLOB (const array<Byte, SIZE>& data);
                BLOB (const vector<Byte>& data);
                template    <typename CONTAINER_OF_BYTE>
                explicit BLOB (const CONTAINER_OF_BYTE& data);
                BLOB (const Byte* start, const Byte* end);
                BLOB (const initializer_list<pair<const Byte*, const Byte*>>& startEndPairs);
                BLOB (const initializer_list<BLOB>& list2Concatenate);

            protected:
                struct  _IRep;

            protected:
#if     qStroika_Foundation_Memory_BLOBUsesStroikaSharedPtr_
                using   SharedIRep  =   Memory::SharedPtr<_IRep>;
#else
                using   SharedIRep  =   shared_ptr<_IRep>;
#endif

            protected:
                /**
                 * Subclass BLOB, and provider your own 'rep' type, to create more efficient storage.
                 */
                explicit BLOB (const SharedIRep& rep);
                explicit BLOB (SharedIRep&&  rep);

            public:
                nonvirtual  BLOB& operator= (const BLOB& rhs) = default;

            public:
                /*
                 *  \brief  Create a BLOB from the given data - without copying the data (dangerous).
                 *
                 *  Attach () causes 'move semantics' on the pointer - where
                 *  the BLOB takes over ownership of the pointer, and will call delete[] (start)
                 *  on the 'start' pointer. Note - DANGEROUS IF MISUSED.
                 */
                static  BLOB    Attach (const Byte* start, const Byte* end);

            public:
                /*
                 *  \brief  Create a BLOB from the given data - without copying the data (dangerous), and never deletes
                 *
                 *  AttachApplicationLifetime () causes 'move semantics' on the pointer - where
                 *  the BLOB takes over ownership of the pointer, and will never delete the data.
                 */
                static  BLOB    AttachApplicationLifetime (const Byte* start, const Byte* end);
                template    <size_t SIZE>
                static  BLOB    AttachApplicationLifetime (const Byte (&data)[SIZE]);

            public:
                /**
                 *  Returns true iff the size of the BLOB is zero.
                 */
                nonvirtual  bool    empty () const;

            public:
                /**
                 *  Convert BLOB losslessly into a standard C++ type.
                 *      Supported Types for 'T' include:
                 *          o   vector<Byte>
                 *          o   Streams::BinaryInputStream
                 *          o   pair<const Byte*, const Byte*>
                 */
                template    <typename   T>
                nonvirtual  T   As () const;
                /**
                 *  Convert BLOB losslessly into a standard C++ type.
                 *      Supported Types for 'T' include:
                 *          o   vector<Byte>
                 *          o   Streams::BinaryInputStream
                 *          o   pair<const Byte*, const Byte*>
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
                 *  Returns the number of bytes in the BLOB.
                 */
                nonvirtual  size_t      GetSize () const;

            public:
                /**
                 *  Return true iff the BLOBs compare bytewise equal.
                 *
                 *  This is like memcmp() == 0.
                 */
                nonvirtual  bool    Equals (const BLOB& rhs) const;

            public:
                /**
                 *  This is like memcmp() - it returns == 0, if the RHS and LHS are the same, and it
                 *  returns < 0 if the first byte where the two regions differ is less than the first byte
                 *  of the RHS (where they differ).
                 */
                nonvirtual  int      Compare (const BLOB& rhs) const;

            public:
                /**
                 *  Trivial alias for @see Compare()
                 */
                nonvirtual  int      compare (const BLOB& rhs) const;

            public:
                /**
                 *  Trivial alias for @see GetSize()
                 */
                nonvirtual  size_t      size () const;

            public:
                /**
                 *  Trivial alias for @see GetSize()
                 */
                nonvirtual  size_t      length () const;

            public:
                /**
                 *  Trivial alias BLOB ({*this, rhs});
                 */
                nonvirtual  BLOB    operator+ (const BLOB& rhs) const;

            private:
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
            public:
#endif
                struct  BasicRep_;
                struct  ZeroRep_ ;
                struct  AdoptRep_ ;
                struct  AdoptAppLifetimeRep_ ;

            private:
                SharedIRep   fRep_;
            };


            template    <>
            void    BLOB::As (vector<Byte>* into) const;
            template    <>
            vector<Byte> BLOB::As () const;


            /**
             * This abstract interface defines the behavior of a BLOB.
             *
             *  \note   we use enable_shared_from_this<> for performance reasons, not for any semantic purpose
             *
             */
            struct  BLOB::_IRep
#if     qStroika_Foundation_Memory_BLOBUsesStroikaSharedPtr_
                    : public Memory::enable_shared_from_this<BLOB::_IRep>
#else
                    : public std::enable_shared_from_this<BLOB::_IRep>
#endif
            {
                _IRep ();
                _IRep (const _IRep&) = delete;
                virtual ~_IRep ();
                virtual pair<const Byte*, const Byte*>   GetBounds () const =    0;

                nonvirtual  const _IRep& operator= (const _IRep&) = delete;
            };


            template    <>
            Streams::BinaryInputStream BLOB::As () const;


            /**
             *  operator indirects to BLOB::Compare()
             */
            bool operator< (const BLOB& lhs, const BLOB& rhs);

            /**
             *  operator indirects to BLOB::Compare()
             */
            bool operator<= (const BLOB& lhs, const BLOB& rhs);

            /**
             *  operator indirects to BLOB::Equals()
             */
            bool operator== (const BLOB& lhs, const BLOB& rhs);

            /**
             *  operator indirects to BLOB::Equals()
             */
            bool operator!= (const BLOB& lhs, const BLOB& rhs);

            /**
             *  operator indirects to BLOB::Compare()
             */
            bool operator>= (const BLOB& lhs, const BLOB& rhs);

            /**
             *  operator indirects to BLOB::Compare()
             */
            bool operator> (const BLOB& lhs, const BLOB& rhs);


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
