/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_BLOB_h_
#define _Stroika_Foundation_Memory_BLOB_h_ 1

#include "../StroikaPreComp.h"

#include <array>
#include <compare>
#include <memory>
#include <vector>

#include "../Configuration/Common.h"
#include "../Configuration/Concepts.h"
#include "../Debug/AssertExternallySynchronizedMutex.h"
#include "../Memory/Common.h"
#include "../Streams/InputStream.h" // maybe bad - leads to circularity problems but hard to pre-declare InputStream

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 * TODO:
 *      @todo   SHOULD add template CTOR args - but must be careful to say iterator <byte> and
 *              only (or handle differently) random access iterators versus just plain forward iterators.
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
 *      @todo   Add MemoryMappedFileBLOB to Foundation/File section � and have it subclass
 *              BLOB (object slicing) � and have different CTOR, and different virtual Rep
 *              (decide semantics � not clear � readonly)
 *          
 *              MAYBE better static method MemoryMappedFileAsBLOB () instead of separete class since
 *              then no object slicing, and we can document its just like the 'applicationlifetime' constructor - unsafe
 *              if the underlying mapped file data ever changes.
 *
 *      @todo   Do CTOR that uses iterator start/end not just const byte* start, const byte* end.
 *
 */

namespace Stroika::Foundation::Characters {
    class String;
}

namespace Stroika::Foundation::Memory {

    using namespace Configuration;

    /**
     *  A BLOB is a read-only binary region of memory. Once a BLOB is constructed, the data inside cannot
     *  change (except by assignement - being assigned over).
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *      o   Standard Stroika Comparison support (operator<=>,operator==, etc);
     *  
     *      This is like memcmp() - bytewise unsigned comparison
     * 
     *  \note Performance
     *      o   Copying a BLOB is just copying a shared_ptr
     *      o   Allocation should be extremely cheap, due to the use of make_shared<> or allocate_shared<> or 
     *          Memory::UseBlockAllocationIfAppropriate<>
     *          which should use the block allocation storage mechansism, which is generally a lock free very fast allocator.
     *          And the use of InlineBuffer<64> means that allocation of BLOBs of size <= 64 should requite no calls to the
     *          global ::operator new/malloc/free/delete
     */
    class BLOB {
    protected:
        struct _IRep;

    public:
        /**
         *  \par Example Usage
         *      \code
         *           BLOB    b1  =   BLOB{ 0x29, 0x14, 0x4a, };
         *      \endcode
         * 
         * 
         *  \see also FromHex, FromRaw (constructor like static functions with special names to make treatment more clear)
         *  \see also Attach, and AttachAndDelete, more rarely useful (and dangerous).
         */
        BLOB ();
        // clang-format off
        BLOB (BLOB&& src) noexcept = default;
        BLOB (const BLOB& src) noexcept = default;
        // clang-format on
        template <ranges::range CONTAINER_OF_BYTE>
        BLOB (const CONTAINER_OF_BYTE& data)
            requires (is_convertible_v<typename CONTAINER_OF_BYTE::value_type, byte> or
                      is_convertible_v<typename CONTAINER_OF_BYTE::value_type, uint8_t>);
        BLOB (span<const byte> s);
        BLOB (const byte* start, const byte* end);
        BLOB (const uint8_t* start, const uint8_t* end);
        BLOB (const initializer_list<pair<const byte*, const byte*>>& startEndPairs);
        BLOB (const initializer_list<BLOB>& list2Concatenate);
        BLOB (const initializer_list<byte>& bytes);
        BLOB (const initializer_list<uint8_t>& bytes);

    protected:
        /**
         * Subclass BLOB, and provider your own 'rep' type, to create more efficient storage.
         */
        explicit BLOB (const shared_ptr<_IRep>& rep);
        explicit BLOB (shared_ptr<_IRep>&& rep);

    public:
        /**
         */
        nonvirtual BLOB& operator= (BLOB&& rhs) noexcept = default;
        nonvirtual BLOB& operator= (const BLOB& rhs)     = default;

    public:
        /**
         *  \brief  Convert string of hex bytes to BLOB.
         *
         *  Like a constructor, but where you clearly name the intention of how to interpret the
         *  bytes.
         *
         *  Spaces allowed, but treat as array of (possibly space delimited) hex bytes to BLOB.
         * 
         *  Upper/LowerCase OK, but invalid characters generate throw.
         *
         *  \par Example Usage
         *      \code
         *          Assert  ((BLOB::FromHex ("29144adb4ece20450956e813652fe8d6") == BLOB { 0x29, 0x14, 0x4a, 0xdb, 0x4e, 0xce, 0x20, 0x45, 0x09, 0x56, 0xe8, 0x13, 0x65, 0x2f, 0xe8, 0xd6 }));
         *      \endcode
         */
        static BLOB FromHex (const char* b);
        static BLOB FromHex (const char* s, const char* e);
        static BLOB FromHex (span<const char> s);
        static BLOB FromHex (const Characters::String& s);

    public:
        /**
         *  \brief  Convert pointed to data as raw bytes to BLOB.
         *
         *  Like a constructor, but where you clearly name the intention of how to interpret the
         *  bytes.
         *
         *  This does little more than a cast (taking into account sizeof T).
         *
         *  Overloads taking const char*, or const wchar_t* only (char_traits<T>), are assumed to be C-Strings (NUL-terminated).
         *
         *  \note ALL overloads require T is be 'trivially_copyable' - just like memcpy()
         */
        template <typename T>
        static BLOB FromRaw (const T* s, const T* e)
            requires (is_trivially_copyable_v<T>);
        template <typename T>
        static BLOB FromRaw (const T* s, size_t sz)
            requires (is_trivially_copyable_v<T>);
        template <typename T>
        static BLOB FromRaw (const T* s)
            requires (is_same_v<typename char_traits<T>::char_type, T>);
        template <typename T>
        static BLOB FromRaw (const basic_string<T>& s)
            requires (is_same_v<typename char_traits<T>::char_type, T>);
        template <typename T>
        static BLOB FromRaw (const T& s)
            requires (is_trivially_copyable_v<T>);

    public:
        /*
         *  \brief  Create a BLOB from the given data - without copying the data (dangerous if not used carefully, but can be used to efficiently reference constant data).
         *
         *  \note its ILLEGAL and may cause grave disorder, if the caller changes the data passed to Attach() while the derived BLOB (or a copy) exists.
         * 
         *  Typically this is intended to be used to wrap permanant constant data, such as static (text space) read-only data, ROM stuff, etc. It can be used to wrap
         *  data in memory mapped files, but IFF that file mapping will remain permanent (and data immutable). USE WITH CAUTION!
         * 
         *  \see also AttachAndDelete
         */
        template <typename BYTEISH, size_t EXTENT = dynamic_extent>
        static BLOB Attach (span<BYTEISH, EXTENT> s)
            requires (convertible_to<BYTEISH, const byte> or convertible_to<BYTEISH, const uint8_t>);
        template <typename BYTEISH, size_t EXTENT>
        static BLOB Attach (BYTEISH (&data)[EXTENT])
            requires (convertible_to<BYTEISH, const byte> or convertible_to<BYTEISH, const uint8_t>);

    public:
        /*
         *  \brief  like Attach () - but at last reference to BLOB, will call delete[] data
         * 
         *  RARELY useful, but could be needed if you must manually fill in the data after allocation, before wrapping it in a BLOB.
         *  Note - because of how the data is deleted, you must allocate with new byte[nnn].
         */
        static BLOB AttachAndDelete (const byte* s, size_t arrayLen);

    public:
        /**
         *  \req i < size  ();
         */
        nonvirtual byte operator[] (const size_t i) const;

    public:
        /**
         *  Returns true iff the size of the BLOB is zero.
         */
        nonvirtual bool empty () const;

    public:
        /**
         *  Convert BLOB losslessly into a standard C++ type.
         *      Supported Types for 'T' include:
         *          o   span<byte>
         *          o   span<uint8_t>
         *          o   vector<byte>
         *          o   vector<uint8_t>
         *          o   pair<const byte*, const byte*>
         *          o   pair<const uint8_t*, const uint8_t*>
         *          o   Streams::InputStream<byte>::Ptr
         *          o   string      (bytes as characters - note this MAY include NUL-bytes - https://stackoverflow.com/questions/2845769/can-a-stdstring-contain-embedded-nulls)
         *          o   any T where is_trivially_copyable
         * 
         *  \note If T is span<> or pair<> (byte or uint8_t) - this returns INTERNAL pointers into the BLOB storage,
         *        so use with care.
         */
        template <typename T>
        nonvirtual T As () const;
        /**
         *  Convert BLOB losslessly into a standard C++ type.
         *      Supported Types for 'T' include (@see same as As/0)
         */
        template <typename T>
        nonvirtual void As (T* into) const;

    public:
        /**
         *  Return a string of hex bytes - two characters per byte, lower case HEX characters.
         *
         *  \par Example Usage
         *      \code
         *          Assert  ((BLOB::Hex ("29144adb4ece20450956e813652fe8d6").AsHex () == "29144adb4ece20450956e813652fe8d6"));
         *      \endcode
         * 
         *  \see also FromHex ()
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
         *  \note - this is legal to call even on an empty BLOB (but may return nullptr)
         */
        nonvirtual const byte* begin () const;

    public:
        /**
         *  Pointers returned by end(), remain valid for the lifetime of the containing BLOB.
         *  \note - this is legal to call even on an empty BLOB (but may return nullptr)
         */
        nonvirtual const byte* end () const;

    public:
        /**
         *  Returns the number of bytes in the BLOB.
         */
        nonvirtual size_t GetSize () const;

    public:
        /**
         */
        nonvirtual strong_ordering operator<=> (const BLOB& rhs) const;

    public:
        /**
         */
        nonvirtual bool operator== (const BLOB& rhs) const;

    private:
        static strong_ordering TWC_ (const BLOB& lhs, const BLOB& rhs); // utility code share between c++17 and c++20 versions

    public:
        /**
         * @todo cleanup and switch to spans!!
         */
        nonvirtual const byte* data () const;

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
         *
         *  \note this displays the BLOB as hex bytes. To interpret as a string, try 
         *      \code
         *          Assert (TextReader::New (blob).ReadAll () == "hello world");
         *      \endcode
         */
        nonvirtual Characters::String ToString (size_t maxBytesToShow = 80) const;

    public:
        /**
         *  Trivial alias BLOB ({*this, rhs});
         */
        nonvirtual BLOB operator+ (const BLOB& rhs) const;

    public:
        [[deprecated ("Since Stroika v3.0d4 use span")]] static BLOB Attach (const byte* start, const byte* end)
        {
            return Attach (span{start, end});
        }
        [[deprecated ("Since Stroika v3.0d4 use Attach")]] static BLOB AttachApplicationLifetime (const byte* start, const byte* end)
        {
            return Attach (span{start, end});
        }
        template <size_t SIZE>
        [[deprecated ("Since Stroika v3.0d4 use Attach")]] static BLOB AttachApplicationLifetime (const byte (&data)[SIZE])
        {
            return Attach (span{data, SIZE});
        }
        template <typename... ARGS>
        [[deprecated ("Since Stroika v3.0d5 use FromHex")]] static BLOB Hex (ARGS... args)
        {
            return FromHex (args...);
        }
        template <typename... ARGS>
        [[deprecated ("Since Stroika v3.0d5 use FromRaw")]] static BLOB Raw (ARGS... args)
        {
            return FromRaw (args...);
        }

    private:
        struct BasicRep_;
        struct ZeroRep_;
        struct AdoptRep_;          // e.g. for static constexpr arrays
        struct AdoptAndDeleteRep_; // for user allocated new byte[]....

    private:
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
        shared_ptr<_IRep>                                              fRep_;
    };

    template <>
    vector<byte> BLOB::As () const;
    template <>
    vector<uint8_t> BLOB::As () const;
    template <>
    span<const byte> BLOB::As () const;
    template <>
    span<const uint8_t> BLOB::As () const;
    template <>
    pair<const byte*, const byte*> BLOB::As () const;
    template <>
    pair<const uint8_t*, const uint8_t*> BLOB::As () const;
    template <>
    Streams::InputStream<byte>::Ptr BLOB::As () const;

    template <>
    void BLOB::As (vector<byte>* into) const;
    template <>
    void BLOB::As (vector<uint8_t>* into) const;
    template <>
    void BLOB::As (span<const byte>* into) const;
    template <>
    void BLOB::As (span<const uint8_t>* into) const;
    template <>
    void BLOB::As (pair<const byte*, const byte*>* into) const;
    template <>
    void BLOB::As (pair<const uint8_t*, const uint8_t*>* into) const;
    template <>
    void BLOB::As (Streams::InputStream<byte>::Ptr* into) const;

    /**
     * This abstract interface defines the behavior of a BLOB.
     */
    struct BLOB::_IRep {
        _IRep ()                                    = default;
        _IRep (const _IRep&)                        = delete;
        virtual ~_IRep ()                           = default;
        virtual span<const byte> GetBounds () const = 0;

        nonvirtual const _IRep& operator= (const _IRep&) = delete;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "BLOB.inl"

#endif /*_Stroika_Foundation_Memory_BLOB_h_*/
