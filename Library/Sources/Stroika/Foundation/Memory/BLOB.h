/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_BLOB_h_
#define _Stroika_Foundation_Memory_BLOB_h_ 1

#include "../StroikaPreComp.h"

#include <array>
#include <memory>
#include <vector>

#include "../Configuration/Common.h"
#include "../Configuration/Concepts.h"
#include "../Debug/AssertExternallySynchronizedLock.h"
#include "../Memory/Common.h"
#include "../Memory/SharedPtr.h"
#include "../Streams/InputStream.h" // maybe bad - leads to circularity problems but hard to pre-declare InputStream

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 * TODO:
 *      @todo   SHOULD add template CTOR args - but must be careful to say iterator <Byte> and
 *              only (or handle differently) random access iterators versus just plain forward iteraotrs.
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
 *      @todo   Do CTOR that uses iterator start/end not just const Byte* start, const Byte* end.
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Streams {
            //template <typename ELEMENT_TYPE>
            //class InputStream;
        }
        namespace Characters {
            class String;
        }
    }
}

namespace Stroika {
    namespace Foundation {
        namespace Memory {

            /**
             *      If kBLOBUsesStroikaSharedPtr is true, use Stroika's SharedPtr<> in place of std::shared_ptr<>. This is an
             *      internal implementaiton detail, and may go away as an option.
             *
             *      This defaults to @see qStroika_Foundation_Memory_SharedPtr_IsFasterThan_shared_ptr
             */
            constexpr bool kBLOBUsesStroikaSharedPtr = qStroika_Foundation_Memory_SharedPtr_IsFasterThan_shared_ptr;

            using namespace std;

            using namespace Configuration;

            /**
             *  A BLOB is a read-only binary region of memory. Once a BLOB is constructed, the data inside cannot
             *  change (except by assignement - being assigned over).
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
             *
             *  \note   See coding conventions document about operator usage: Compare () and operator<, operator>, etc
             */
            class BLOB : private Debug::AssertExternallySynchronizedLock {
            public:
                /**
                 *  \par Example Usage
                 *      \code
                 *           BLOB    b1  =   BLOB { 0x29, 0x14, 0x4a, };
                 *      \endcode
                 */
                BLOB ();
                BLOB (const BLOB& src) = default;
                BLOB (BLOB&& src);
                template <typename CONTAINER_OF_BYTE, typename ENABLE_IF = typename enable_if<Configuration::has_beginend<CONTAINER_OF_BYTE>::value && std::is_convertible<typename CONTAINER_OF_BYTE::value_type, Byte>::value>::type>
                BLOB (const CONTAINER_OF_BYTE& data);
                BLOB (const Byte* start, const Byte* end);
                BLOB (const initializer_list<pair<const Byte*, const Byte*>>& startEndPairs);
                BLOB (const initializer_list<BLOB>& list2Concatenate);
                BLOB (const initializer_list<Byte>& bytes);

            public:
                /**
                 *  \brief  Convert string of hex bytes to BLOB.
                 *
                 *  Like a constructor, but where you clearly name the intention of how to interpret the
                 *  bytes.
                 *
                 *  Spaces allowed, but treat as array of (possibly space delimited) hex bytes to BLOB.
                 *
                 *  \par Example Usage
                 *      \code
                 *          Assert  ((BLOB::Hex ("29144adb4ece20450956e813652fe8d6") == BLOB { 0x29, 0x14, 0x4a, 0xdb, 0x4e, 0xce, 0x20, 0x45, 0x09, 0x56, 0xe8, 0x13, 0x65, 0x2f, 0xe8, 0xd6 }));
                 *      \endcode
                 */
                static BLOB Hex (const char* b);
                static BLOB Hex (const char* s, const char* e);
                static BLOB Hex (const string& s);

            public:
                /**
                 *  \brief  Convert pointed to data as raw bytes to BLOB.
                 *
                 *  Like a constructor, but where you clearly name the intention of how to interpret the
                 *  bytes.
                 *
                 *  This does little more than a cast (taking into account sizeof T)
                 *
                 *  Overloads taking const char*, or const wchar_t* only, are assumed to be C-Strings (NUL-terminated).
                 */
                template <typename T>
                static BLOB Raw (const T* s, const T* e);
                template <typename T>
                static BLOB Raw (const T* s, size_t sz);
                static BLOB Raw (const char* s);
                static BLOB Raw (const wchar_t* s);
                template <typename CONTAINER_OF_POD_T, typename ENABLE_IF = typename enable_if<Configuration::has_beginend<CONTAINER_OF_POD_T>::value && std::is_pod<typename CONTAINER_OF_POD_T::value_type>::value>::type>
                static BLOB Raw (const CONTAINER_OF_POD_T& s);

            protected:
                struct _IRep;

            protected:
                /**
                 */
                template <typename T>
                using _SharedRepImpl = conditional_t<kBLOBUsesStroikaSharedPtr, Memory::SharedPtr<T>, shared_ptr<T>>;

            protected:
                /**
                 */
                using _SharedIRep = _SharedRepImpl<_IRep>;

            protected:
                /**
                 */
                template <typename T, typename... ARGS_TYPE>
                static _SharedRepImpl<T> _MakeSharedPtr (ARGS_TYPE&&... args);

            protected:
                /**
                 * Subclass BLOB, and provider your own 'rep' type, to create more efficient storage.
                 */
                explicit BLOB (const _SharedIRep& rep);
                explicit BLOB (_SharedIRep&& rep);

            public:
                /**
                 */
                nonvirtual BLOB& operator= (const BLOB& rhs) = default;

            public:
                /*
                 *  \brief  Create a BLOB from the given data - without copying the data (dangerous).
                 *
                 *  Attach () causes 'move semantics' on the pointer - where
                 *  the BLOB takes over ownership of the pointer, and will call delete[] (start)
                 *  on the 'start' pointer. Note - DANGEROUS IF MISUSED.
                 *
                 *  \req (start == nullptr and end == nullptr) or (start != nullptr and end != nullptr)
                 *  \req (start <= end)
                 *
                 *  @see AttachApplicationLifetime
                 */
                static BLOB Attach (const Byte* start, const Byte* end);

            public:
                /*
                 *  \brief  Create a BLOB from the given data - without copying the data (dangerous), and never deletes
                 *
                 *  AttachApplicationLifetime () may save and use pointer indefinitely, but will never modify what it
                 *  points to nor delete it. The caller \em must do likewise.
                 *
                 *  \req (start == nullptr and end == nullptr) or (start != nullptr and end != nullptr)
                 *  \req (start <= end)
                 *
                 *  @see Attach
                 */
                static BLOB AttachApplicationLifetime (const Byte* start, const Byte* end);
                template <size_t SIZE>
                static BLOB AttachApplicationLifetime (const Byte (&data)[SIZE]);

            public:
                /**
                 *  Returns true iff the size of the BLOB is zero.
                 */
                nonvirtual bool empty () const;

            public:
                /**
                 *  Convert BLOB losslessly into a standard C++ type.
                 *      Supported Types for 'T' include:
                 *          o   vector<Byte>
                 *          o   Streams::InputStream<Byte>::Ptr
                 *          o   pair<const Byte*, const Byte*>
                 */
                template <typename T>
                nonvirtual T As () const;
                /**
                 *  Convert BLOB losslessly into a standard C++ type.
                 *      Supported Types for 'T' include:
                 *          o   vector<Byte>
                 *          o   Streams::InputStream<Byte>::Ptr
                 *          o   pair<const Byte*, const Byte*>
                 */
                template <typename T>
                nonvirtual void As (T* into) const;

            public:
                /**
                 *  Return a string of hex bytes - two characters per byte, lower case HEX characters.
                 *
                 *  \par Example Usage
                 *      \code
                 *          Assert  ((BLOB::Hex ("29144adb4ece20450956e813652fe8d6").AsHex () == L"29144adb4ece20450956e813652fe8d6"));
                 *      \endcode
                 */
                nonvirtual Characters::String AsHex (size_t maxBytesToShow = numeric_limits<size_t>::max ()) const;

            public:
                /**
                 *  Return a BLOB made by concatenating this BLOB count times.
                 */
                nonvirtual BLOB Repeat (unsigned int count) const;

            public:
                /**
                 *  Return a BLOB made from the given sub-range of bytes.
                 *      \req startAt <= endAt
                 *      \req endAt < GetSize ()
                 */
                nonvirtual BLOB Slice (size_t startAt, size_t endAt) const;

            public:
                /**
                 *  Pointers returned by begin(), remain valid for the lifetime of the containing BLOB.
                 */
                nonvirtual const Byte* begin () const;

            public:
                /**
                 *  Pointers returned by end(), remain valid for the lifetime of the containing BLOB.
                 */
                nonvirtual const Byte* end () const;

            public:
                /**
                 *  Returns the number of bytes in the BLOB.
                 */
                nonvirtual size_t GetSize () const;

            public:
                /**
                 *  Return true iff the BLOBs compare bytewise equal.
                 *
                 *  This is like memcmp() == 0.
                 */
                nonvirtual bool Equals (const BLOB& rhs) const;

            public:
                /**
                 *  This is like memcmp() - it returns == 0, if the RHS and LHS are the same, and it
                 *  returns < 0 if the first byte where the two regions differ is less than the first byte
                 *  of the RHS (where they differ).
                 */
                nonvirtual int Compare (const BLOB& rhs) const;

            public:
                /**
                 *  Trivial alias for @see Compare()
                 */
                nonvirtual int compare (const BLOB& rhs) const;

            public:
                /**
                 *  Trivial alias for @see GetSize()
                 */
                nonvirtual size_t size () const;

            public:
                /**
                 *  Trivial alias for @see GetSize()
                 */
                nonvirtual size_t length () const;

            public:
                /**
                 *  @see Characters::ToString()
                 *  Return a debug-friendly, display version of the current BLOB. This is not guaranteed parseable or usable except for debugging.
                 */
                nonvirtual Characters::String ToString (size_t maxBytesToShow = numeric_limits<size_t>::max ()) const;

            public:
                /**
                 *  Trivial alias BLOB ({*this, rhs});
                 */
                nonvirtual BLOB operator+ (const BLOB& rhs) const;

            private:
                struct BasicRep_;
                struct ZeroRep_;
                struct AdoptRep_;
                struct AdoptAppLifetimeRep_;

            private:
                _SharedIRep fRep_;
            };

            template <>
            void BLOB::As (vector<Byte>* into) const;
            template <>
            vector<Byte> BLOB::As () const;

            /**
             * This abstract interface defines the behavior of a BLOB.
             *
             *  \note   we use enable_shared_from_this<> for performance reasons, not for any semantic purpose
             */
            struct BLOB::_IRep : conditional_t<kBLOBUsesStroikaSharedPtr, Memory::enable_shared_from_this<BLOB::_IRep>, std::enable_shared_from_this<BLOB::_IRep>> {
                _IRep ()                                                  = default;
                _IRep (const _IRep&)                                      = delete;
                virtual ~_IRep ()                                         = default;
                virtual pair<const Byte*, const Byte*> GetBounds () const = 0;

                nonvirtual const _IRep& operator= (const _IRep&) = delete;
            };

            template <>
            Streams::InputStream<Byte>::Ptr BLOB::As () const;

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
#include "BLOB.inl"

#endif /*_Stroika_Foundation_Memory_BLOB_h_*/
