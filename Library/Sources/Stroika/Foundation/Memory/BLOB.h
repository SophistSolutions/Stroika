/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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
#include "../Memory/SharedPtr.h"
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

    /**
     *      If kBLOBUsesStroikaSharedPtr is true, use Stroika's SharedPtr<> in place of shared_ptr<>. This is an
     *      internal implementaiton detail, and may go away as an option.
     *
     *      This defaults to @see Memory::kSharedPtr_IsFasterThan_shared_ptr
     */
    constexpr bool kBLOBUsesStroikaSharedPtr = Memory::kSharedPtr_IsFasterThan_shared_ptr;

    using namespace std;
    using std::byte;

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
    class BLOB  {
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

    public:
        /**
         *  \par Example Usage
         *      \code
         *           BLOB    b1  =   BLOB { 0x29, 0x14, 0x4a, };
         *      \endcode
         */
        BLOB ();
        // clang-format off
        BLOB (BLOB&& src) noexcept = default;
        BLOB (const BLOB& src) noexcept = default;
        // clang-format on
        template <typename CONTAINER_OF_BYTE, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_BYTE> and (is_convertible_v<typename CONTAINER_OF_BYTE::value_type, byte> or is_convertible_v<typename CONTAINER_OF_BYTE::value_type, uint8_t>)>* = nullptr>
        BLOB (const CONTAINER_OF_BYTE& data);
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
        explicit BLOB (const _SharedIRep& rep);
        explicit BLOB (_SharedIRep&& rep);

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
         *  \par Example Usage
         *      \code
         *          Assert  ((BLOB::Hex ("29144adb4ece20450956e813652fe8d6") == BLOB { 0x29, 0x14, 0x4a, 0xdb, 0x4e, 0xce, 0x20, 0x45, 0x09, 0x56, 0xe8, 0x13, 0x65, 0x2f, 0xe8, 0xd6 }));
         *      \endcode
         */
        static BLOB Hex (const char* b);
        static BLOB Hex (const char* s, const char* e);
        static BLOB Hex (const string& s);
        static BLOB Hex (const string_view& s);

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
        template <typename T, enable_if_t<is_trivially_copyable_v<T>>* = nullptr>
        static BLOB Raw (const T* s, const T* e);
        template <typename T, enable_if_t<is_trivially_copyable_v<T>>* = nullptr>
        static BLOB Raw (const T* s, size_t sz);
        template <typename T, enable_if_t<is_same_v<typename char_traits<T>::char_type, T>>* = nullptr>
        static BLOB Raw (const T* s);
        template <typename T, enable_if_t<is_same_v<typename char_traits<T>::char_type, T>>* = nullptr>
        static BLOB Raw (const basic_string<T>& s);
        template <typename T, enable_if_t<is_trivially_copyable_v<T>>* = nullptr>
        static BLOB Raw (const T& s);

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
        static BLOB Attach (const byte* start, const byte* end);

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
        static BLOB AttachApplicationLifetime (const byte* start, const byte* end);
        template <size_t SIZE>
        static BLOB AttachApplicationLifetime (const byte (&data)[SIZE]);

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
         *          o   vector<byte>
         *          o   vector<uint8_t>
         *          o   pair<const byte*, const byte*>
         *          o   pair<const uint8_t*, const uint8_t*>
         *          o   Streams::InputStream<byte>::Ptr
         *          o   any T where is_trivially_copyable
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
         *          Assert (TextReader::New (blob).ReadAll () == L"hello world");
         *      \endcode
         */
        nonvirtual Characters::String ToString (size_t maxBytesToShow = 80) const;

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
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fAssertExternallySynchronized_;
        _SharedIRep fRep_;
    };

    template <>
    vector<byte> BLOB::As () const;
    template <>
    vector<uint8_t> BLOB::As () const;
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
    void BLOB::As (pair<const byte*, const byte*>* into) const;
    template <>
    void BLOB::As (pair<const uint8_t*, const uint8_t*>* into) const;
    template <>
    void BLOB::As (Streams::InputStream<byte>::Ptr* into) const;

    /**
     * This abstract interface defines the behavior of a BLOB.
     */
    struct BLOB::_IRep {
        _IRep ()                                                  = default;
        _IRep (const _IRep&)                                      = delete;
        virtual ~_IRep ()                                         = default;
        virtual pair<const byte*, const byte*> GetBounds () const = 0;

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
