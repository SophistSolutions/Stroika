/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include <algorithm>
#include <cstring>
#include <ranges>
#include <type_traits>

#include "Common.h"
#include "Stroika/Foundation/Containers/Support/ReserveTweaks.h"
#include "Stroika/Foundation/Debug/Assertions.h"

namespace Stroika::Foundation::Execution {
    // Instead of #include "Stroika/Foundation/Execution/Throw.h"
    template <equality_comparable_with<nullptr_t> T>
    void ThrowIfNull (T p);
}
namespace Stroika::Foundation::Memory {

    /*
     ********************************************************************************
     ************************** InlineBuffer<T, BUF_SIZE> ***************************
     ********************************************************************************
     */
    template <typename T, size_t BUF_SIZE>
    inline InlineBuffer<T, BUF_SIZE>::InlineBuffer () noexcept
        : fLiveData_{BufferAsT_ ()}
    {
#if qDebug
        Assert (UsingInlinePreallocatedBuffer_ ()); // cuz empty size so always fits
        (void)::memcpy (fGuard1_, kGuard1_, sizeof (kGuard1_));
        (void)::memcpy (fGuard2_, kGuard2_, sizeof (kGuard2_));
#endif
        Invariant ();
    }
    template <typename T, size_t BUF_SIZE>
    inline InlineBuffer<T, BUF_SIZE>::InlineBuffer (size_t nElements)
        : InlineBuffer{}
    {
        resize (nElements);
        Invariant ();
    }
    template <typename T, size_t BUF_SIZE>
    inline InlineBuffer<T, BUF_SIZE>::InlineBuffer (UninitializedConstructorFlag, size_t nElements)
        requires (is_trivially_copyable_v<T>)
        : InlineBuffer{}
    {
        resize_uninitialized (nElements);
        Invariant ();
    }
    template <typename T, size_t BUF_SIZE>
    template <input_iterator ITERATOR_OF_T, sentinel_for<remove_cvref_t<ITERATOR_OF_T>> ITERATOR_OF_T2>
    InlineBuffer<T, BUF_SIZE>::InlineBuffer (const ITERATOR_OF_T& start, ITERATOR_OF_T2&& end)
        : InlineBuffer{}
    {
        static_assert (is_convertible_v<Configuration::ExtractValueType_t<ITERATOR_OF_T>, T>);
#if qCompilerAndStdLib_stdlib_ranges_pretty_broken_Buggy || qCompilerAndStdLib_stdlib_ranges_ComputeDiffSignularToADeref_Buggy
        auto sz = static_cast<size_t> (distance (start, ITERATOR_OF_T{end}));
#else
        auto sz = static_cast<size_t> (ranges::distance (start, forward<ITERATOR_OF_T2> (end)));
#endif
        if (not this->HasEnoughCapacity_ (sz)) [[unlikely]] {
            reserve (sz, true); // reserve not resize() so we can do uninitialized_copy (avoid constructing empty objects to be assigned over)
        }
#if qCompilerAndStdLib_stdlib_ranges_pretty_broken_Buggy || qCompilerAndStdLib_stdlib_ranges_ComputeDiffSignularToADeref_Buggy
        uninitialized_copy (start, ITERATOR_OF_T (end), this->begin ());
#else
        ranges::uninitialized_copy (start, forward<ITERATOR_OF_T2> (end), this->begin (), this->begin () + sz);
#endif
        fSize_ = sz;
        Invariant ();
    }
    template <typename T, size_t BUF_SIZE>
    template <ISpanOfT<T> SPAN_T>
    inline InlineBuffer<T, BUF_SIZE>::InlineBuffer (const SPAN_T& copyFrom)
        : InlineBuffer{}
    {
        if (not this->HasEnoughCapacity_ (copyFrom.size ())) [[unlikely]] {
            reserve (copyFrom.size (), true); // reserve not resize() so we can do uninitialized_copy (avoid constructing empty objects to be assigned over)
        }
        uninitialized_copy (copyFrom.begin (), copyFrom.end (), this->begin ());
        fSize_ = copyFrom.size ();
        Invariant ();
    }
    template <typename T, size_t BUF_SIZE>
    template <size_t FROM_BUF_SIZE>
    inline InlineBuffer<T, BUF_SIZE>::InlineBuffer (const InlineBuffer<T, FROM_BUF_SIZE>& src)
        : InlineBuffer{src.begin (), src.end ()}
    {
    }
    template <typename T, size_t BUF_SIZE>
    inline InlineBuffer<T, BUF_SIZE>::InlineBuffer (const InlineBuffer& src)
        : InlineBuffer{src.begin (), src.end ()}
    {
    }
    template <typename T, size_t BUF_SIZE>
    inline InlineBuffer<T, BUF_SIZE>::InlineBuffer (InlineBuffer&& src)
        : InlineBuffer{}
    {
#if qDebug
        size_t origSize     = src.size ();
        size_t origCapacity = src.capacity ();
#endif
        if (src.UsingInlinePreallocatedBuffer_ ()) [[likely]] {
            // then little to be saved from a move, and technically we really cannot do much, except that we can 'move' the data elements
            // This 'moving' is done via make_move_iterator () - rather that magically makes the uninitialized_copy really move instead of copying
            Assert (capacity () == src.capacity ());
            size_t sz = src.size ();
            uninitialized_copy (std::make_move_iterator (src.begin ()), std::make_move_iterator (src.begin () + sz), this->begin ());
            fSize_ = sz;
        }
        else {
            // OK - we can do some trickery here to steal the underlying pointers
            this->fLiveData_ = src.fLiveData_;
            src.fLiveData_   = src.BufferAsT_ ();
            this->fSize_     = src.fSize_;
            src.fSize_       = 0;
            Ensure (src.fSize_ == 0);
            Ensure (src.capacity () == BUF_SIZE);
        }
#if qDebug
        Ensure (this->size () == origSize);
        Ensure (this->capacity () == origCapacity);
#endif
    }
    template <typename T, size_t BUF_SIZE>
    inline InlineBuffer<T, BUF_SIZE>::~InlineBuffer ()
    {
        Invariant ();
        DestroyElts_ (this->begin (), this->end ());
        if (not UsingInlinePreallocatedBuffer_ ()) [[unlikely]] {
            // we must have used the heap...
            Deallocate_ (LiveDataAsAllocatedBytes_ ());
        }
    }
    template <typename T, size_t BUF_SIZE>
    InlineBuffer<T, BUF_SIZE>& InlineBuffer<T, BUF_SIZE>::operator= (const InlineBuffer& rhs)
    {
        Invariant ();
        if (this != &rhs) {
            // @todo this simple implementation could be more efficient
            DestroyElts_ (this->begin (), this->end ());
            fSize_ = 0;
            if (not this->HasEnoughCapacity_ (rhs.size ())) [[unlikely]] {
                reserve (rhs.size ());
            }
            uninitialized_copy (rhs.begin (), rhs.end (), this->begin ());
            fSize_ = rhs.size ();
            Invariant ();
        }
        return *this;
    }
    template <typename T, size_t BUF_SIZE>
    inline InlineBuffer<T, BUF_SIZE>& InlineBuffer<T, BUF_SIZE>::operator= (InlineBuffer&& rhs)
    {
        Invariant ();
#if qDebug
        size_t origFromSize     = rhs.size ();
        size_t origFromCapacity = rhs.capacity ();
#endif
        // destroy any existing elts (dont bother resetting size til end), and deallocate any RAM in use
        DestroyElts_ (this->begin (), this->end ());
        if (not this->UsingInlinePreallocatedBuffer_ ()) [[unlikely]] {
            Deallocate_ (LiveDataAsAllocatedBytes_ ());
            fLiveData_ = BufferAsT_ ();
        }

        if (rhs.UsingInlinePreallocatedBuffer_ ()) [[likely]] {
            // then little to be saved from a move, and technically we really cannot do much, except that we can 'move' the data elements
            // This 'moving' is done via make_move_iterator () - rather that magically makes the uninitialized_copy really move instead of copying
            Assert (capacity () == rhs.capacity ());
            size_t sz = rhs.size ();
            uninitialized_copy (std::make_move_iterator (rhs.begin ()), std::make_move_iterator (rhs.begin () + sz), this->begin ());
            fSize_ = sz;
        }
        else {
            // OK - we can do some trickery here to steal the underlying pointers
            this->fLiveData_ = rhs.fLiveData_;
            rhs.fLiveData_   = rhs.BufferAsT_ ();
            this->fSize_     = rhs.fSize_;
            rhs.fSize_       = 0;
            Ensure (rhs.fSize_ == 0);
            Ensure (rhs.capacity () == BUF_SIZE);
        }
#if qDebug
        Ensure (this->size () == origFromSize);
        Ensure (this->capacity () == origFromCapacity);
#endif
        return *this;
    }
    template <typename T, size_t BUF_SIZE>
    template <ISpanOfT<T> SPAN_T>
    InlineBuffer<T, BUF_SIZE>& InlineBuffer<T, BUF_SIZE>::operator= (const SPAN_T& copyFrom)
    {
        Invariant ();
        DestroyElts_ (this->begin (), this->end ());
        fSize_ = 0;
        if (not this->HasEnoughCapacity_ (copyFrom.size ())) [[unlikely]] {
            reserve (copyFrom.size ());
        }
        uninitialized_copy (copyFrom.begin (), copyFrom.end (), this->begin ());
        Invariant ();
        return *this;
    }
    template <typename T, size_t BUF_SIZE>
    inline void InlineBuffer<T, BUF_SIZE>::GrowToSize (size_t nElements)
    {
        if (nElements > size ()) {
            resize (nElements);
        }
        Ensure (size () >= nElements);
    }
    template <typename T, size_t BUF_SIZE>
    inline void InlineBuffer<T, BUF_SIZE>::GrowToSize_uninitialized (size_t nElements)
        requires (is_trivially_copyable_v<T>)
    {
        static_assert (is_trivially_copyable_v<T>);
        if (nElements > size ()) {
            resize_uninitialized (nElements);
        }
        Ensure (size () >= nElements);
    }
    template <typename T, size_t BUF_SIZE>
    inline void InlineBuffer<T, BUF_SIZE>::resize (size_t nElements)
    {
        if (nElements > fSize_) {
            // Growing
            if (not this->HasEnoughCapacity_ (nElements)) [[unlikely]] {
                reserve (nElements, true);
            }
            Assert (nElements <= capacity ());
            Assert (this->begin () + fSize_ == this->end ()); // docs/clarity
            auto newEnd = this->begin () + nElements;
            Assert (this->end () < newEnd);
            // uninitialized_fill() guarantees filling all or none - if an exception doing some, it undoes the ones it did (so setting fsize after safe)
            uninitialized_fill (this->end (), newEnd, T{});
            fSize_ = nElements;
            Assert (this->end () == newEnd);
        }
        else if (nElements < fSize_) {
            // Shrinking
            DestroyElts_ (this->begin () + nElements, this->end ());
            fSize_ = nElements;
        }
        Assert (fSize_ == nElements);
        Ensure (size () <= capacity ());
    }
    template <typename T, size_t BUF_SIZE>
    inline void InlineBuffer<T, BUF_SIZE>::resize_uninitialized (size_t nElements)
        requires (is_trivially_copyable_v<T> and is_trivially_destructible_v<T>)
    {
        if (not this->HasEnoughCapacity_ (nElements)) [[unlikely]] {
            reserve (nElements);
        }
        fSize_ = nElements;
        Assert (fSize_ == nElements);
        Ensure (size () <= capacity ());
    }
    template <typename T, size_t BUF_SIZE>
    inline void InlineBuffer<T, BUF_SIZE>::ShrinkTo (size_t nElements)
    {
        Require (nElements <= fSize_);
        if (nElements != fSize_) {
            DestroyElts_ (this->begin () + nElements, this->end ());
            fSize_ = nElements;
        }
        Assert (fSize_ == nElements);
        Ensure (size () <= capacity ());
    }
    template <typename T, size_t BUF_SIZE>
    inline typename InlineBuffer<T, BUF_SIZE>::iterator InlineBuffer<T, BUF_SIZE>::begin () noexcept
    {
        return fLiveData_;
    }
    template <typename T, size_t BUF_SIZE>
    inline typename InlineBuffer<T, BUF_SIZE>::iterator InlineBuffer<T, BUF_SIZE>::end () noexcept
    {
        return fLiveData_ + fSize_;
    }
    template <typename T, size_t BUF_SIZE>
    inline typename InlineBuffer<T, BUF_SIZE>::const_iterator InlineBuffer<T, BUF_SIZE>::begin () const noexcept
    {
        return fLiveData_;
    }
    template <typename T, size_t BUF_SIZE>
    inline typename InlineBuffer<T, BUF_SIZE>::const_iterator InlineBuffer<T, BUF_SIZE>::end () const noexcept
    {
        return fLiveData_ + fSize_;
    }
    template <typename T, size_t BUF_SIZE>
    inline typename InlineBuffer<T, BUF_SIZE>::pointer InlineBuffer<T, BUF_SIZE>::data () noexcept
    {
        return fLiveData_;
    }
    template <typename T, size_t BUF_SIZE>
    inline typename InlineBuffer<T, BUF_SIZE>::const_pointer InlineBuffer<T, BUF_SIZE>::data () const noexcept
    {
        return fLiveData_;
    }
    template <typename T, size_t BUF_SIZE>
    constexpr size_t InlineBuffer<T, BUF_SIZE>::capacity () const noexcept
    {
        // @see class Design Note
        if (UsingInlinePreallocatedBuffer_ ()) [[likely]] {
            return BUF_SIZE;
        }
        else {
            // this case happens precisely in InlineBuffer<T, BUF_SIZE>::reserve() when the capacity is set > BUF_SIZE so initialized then...
            DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wmaybe-uninitialized\""); // RASPI RELEASE COMPILER gcc12 ONLY
            return fCapacityOfFreeStoreAllocation_;
            DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"");
        }
    }
    template <typename T, size_t BUF_SIZE>
    void InlineBuffer<T, BUF_SIZE>::reserve (size_t newCapacity, bool atLeast)
    {
        Require (newCapacity >= size ());
        size_t useNewCapacity = newCapacity;
        size_t oldCapacity    = capacity ();
        if (atLeast) [[likely]] {
            if (useNewCapacity <= oldCapacity) [[likely]] {
                return; // no work todo here....
            }
            // if fits in inline buffer, round up to that size. If exceeding that, use ScalledUpCapcity exponential growth algorithm
            if (useNewCapacity < BUF_SIZE) [[likely]] {
                useNewCapacity = BUF_SIZE;
            }
            else {
                useNewCapacity = Foundation::Containers::Support::ReserveTweaks::GetScaledUpCapacity (useNewCapacity, sizeof (T), BUF_SIZE);
            }
        }
        Invariant ();
        // NOTE - could be upsizing or downsizing, and could be moving to for from allocated or inline buffers
        Assert (useNewCapacity >= newCapacity);
        if (useNewCapacity != oldCapacity) {
            bool oldInPlaceBuffer = oldCapacity <= BUF_SIZE;
            bool newInPlaceBuffer = useNewCapacity <= BUF_SIZE;
            if constexpr (is_trivially_copyable_v<T>) {
                // we only need to copy if going from oldInPlaceBuffer != newInPlaceBuffer, else just use realloc
                if (not oldInPlaceBuffer and not newInPlaceBuffer) {
                    // realloc
                    fLiveData_ = reinterpret_cast<T*> (Reallocate_ (LiveDataAsAllocatedBytes_ (), SizeInBytes_ (useNewCapacity)));
                    fCapacityOfFreeStoreAllocation_ = useNewCapacity;
                }
                else if (oldInPlaceBuffer and newInPlaceBuffer) {
                    // pretty common case - just do nothing
                }
                else if (oldInPlaceBuffer and not newInPlaceBuffer) {
                    // malloc in this case
                    byte* newPtr = Allocate_ (SizeInBytes_ (useNewCapacity));
                    // Initialize new memory from old
                    Assert (this->begin () != reinterpret_cast<T*> (newPtr));
                    Assert (static_cast<size_t> (this->end () - this->begin ()) <= useNewCapacity); // so no possible overflow
                    uninitialized_copy (this->begin (), this->end (), reinterpret_cast<T*> (newPtr));
                    fLiveData_ = reinterpret_cast<T*> (newPtr);
                    if (not newInPlaceBuffer) {
                        fCapacityOfFreeStoreAllocation_ = useNewCapacity;
                    }
                }
                else if (not oldInPlaceBuffer and newInPlaceBuffer) {
                    // was malloced, but not needed anymore - so free
                    byte* newPtr = std::begin (fInlinePreallocatedBuffer_);
                    // Initialize new memory from old
                    Assert (this->begin () != reinterpret_cast<T*> (newPtr));
                    Assert (static_cast<size_t> (this->end () - this->begin ()) <= useNewCapacity); // so no possible overflow
                    uninitialized_copy (this->begin (), this->end (), reinterpret_cast<T*> (newPtr));
                    Deallocate_ (LiveDataAsAllocatedBytes_ ());
                    fLiveData_ = reinterpret_cast<T*> (newPtr);
                }
            }
            else {
                // Only if we changed if using inplace buffer, or if was and is using ramBuffer, and eltCount changed do we need to do anything
                if (oldInPlaceBuffer != newInPlaceBuffer or (not newInPlaceBuffer)) {
                    byte* newPtr = newInPlaceBuffer ? std::begin (fInlinePreallocatedBuffer_) : Allocate_ (SizeInBytes_ (useNewCapacity));

                    // Initialize new memory from old
                    Assert (this->begin () != reinterpret_cast<T*> (newPtr));
                    Assert (static_cast<size_t> (this->end () - this->begin ()) <= useNewCapacity); // so no possible overflow
                    uninitialized_copy (this->begin (), this->end (), reinterpret_cast<T*> (newPtr));

                    // destroy objects in old memory
                    DestroyElts_ (this->begin (), this->end ());

                    // free old memory if needed
                    if (not oldInPlaceBuffer) {
                        Assert (not UsingInlinePreallocatedBuffer_ ());
                        Deallocate_ (LiveDataAsAllocatedBytes_ ());
                    }

                    fLiveData_ = reinterpret_cast<T*> (newPtr);
                    if (not newInPlaceBuffer) {
                        fCapacityOfFreeStoreAllocation_ = useNewCapacity;
                    }
                }
            }
        }
        Ensure ((useNewCapacity <= BUF_SIZE and capacity () == BUF_SIZE) or (useNewCapacity > BUF_SIZE and useNewCapacity == capacity ()));
        Invariant ();
    }
    template <typename T, size_t BUF_SIZE>
    inline size_t InlineBuffer<T, BUF_SIZE>::GetSize () const noexcept
    {
        Ensure (fSize_ <= capacity ());
        return fSize_;
    }
    template <typename T, size_t BUF_SIZE>
    inline size_t InlineBuffer<T, BUF_SIZE>::size () const noexcept
    {
        Ensure (fSize_ <= capacity ());
        return fSize_;
    }
    template <typename T, size_t BUF_SIZE>
    inline bool InlineBuffer<T, BUF_SIZE>::empty () const noexcept
    {
        return fSize_ == 0;
    }
    template <typename T, size_t BUF_SIZE>
    inline typename InlineBuffer<T, BUF_SIZE>::reference InlineBuffer<T, BUF_SIZE>::at (size_t i) noexcept
    {
        Require (i < fSize_);
        return *(fLiveData_ + i);
    }
    template <typename T, size_t BUF_SIZE>
    inline typename InlineBuffer<T, BUF_SIZE>::const_reference InlineBuffer<T, BUF_SIZE>::at (size_t i) const noexcept
    {
        Require (i < fSize_);
        return *(fLiveData_ + i);
    }
    template <typename T, size_t BUF_SIZE>
    inline auto InlineBuffer<T, BUF_SIZE>::operator[] (size_t i) noexcept -> reference
    {
        return at (i);
    }
    template <typename T, size_t BUF_SIZE>
    inline auto InlineBuffer<T, BUF_SIZE>::operator[] (size_t i) const noexcept -> const_reference
    {
        return at (i);
    }
    template <typename T, size_t BUF_SIZE>
    inline void InlineBuffer<T, BUF_SIZE>::push_back (Configuration::ArgByValueType<T> e)
    {
        size_t s    = size ();
        size_t newS = s + 1;
        if (not this->HasEnoughCapacity_ (newS)) [[unlikely]] {
            reserve (newS);
        }
        if constexpr (is_trivially_copyable_v<T>) {
            fLiveData_[s] = e;
        }
        else {
            uninitialized_copy (&e, &e + 1, this->begin () + s);
        }
        ++this->fSize_;
        Invariant ();
    }
    template <typename T, size_t BUF_SIZE>
    template <ISpanOfT<T> SPAN_T>
    void InlineBuffer<T, BUF_SIZE>::push_back (const SPAN_T& copyFrom)
    {
        size_t s    = size ();
        size_t newS = s + copyFrom.size ();
        if (not this->HasEnoughCapacity_ (newS)) [[unlikely]] {
            reserve (newS);
        }
        Assert (this->HasEnoughCapacity_ (newS));
        if constexpr (is_trivially_copyable_v<T>) {
            copy (copyFrom.begin (), copyFrom.end (), this->begin () + s);
        }
        else {
            uninitialized_copy (copyFrom.begin (), copyFrom.end (), this->begin () + s);
        }
        this->fSize_ = newS;
    }
    template <typename T, size_t BUF_SIZE>
    template <ISpan SPAN_T>
    void InlineBuffer<T, BUF_SIZE>::push_back_coerced (const SPAN_T& copyFrom)
    {
        size_t s    = size ();
        size_t newS = s + copyFrom.size ();
        if (not this->HasEnoughCapacity_ (newS)) [[unlikely]] {
            reserve (newS);
        }
        Assert (this->HasEnoughCapacity_ (newS));
        auto outPtr = this->begin () + s;
        if constexpr (is_trivially_copyable_v<T>) {
            for (auto c : copyFrom) {
                *outPtr++ = static_cast<value_type> (c);
            }
        }
        else {
            uninitialized_copy (copyFrom.begin (), copyFrom.end (), outPtr);
        }
        this->fSize_ = newS;
    }
    template <typename T, size_t BUF_SIZE>
    inline void InlineBuffer<T, BUF_SIZE>::clear () noexcept
    {
        resize (0);
    }
    template <typename T, size_t BUF_SIZE>
    inline InlineBuffer<T, BUF_SIZE>::operator T* () noexcept
    {
        AssertNotNull (fLiveData_);
        return fLiveData_;
    }
    template <typename T, size_t BUF_SIZE>
    inline InlineBuffer<T, BUF_SIZE>::operator const T* () const noexcept
    {
        AssertNotNull (fLiveData_);
        return fLiveData_;
    }
    template <typename T, size_t BUF_SIZE>
    inline void InlineBuffer<T, BUF_SIZE>::Invariant () const noexcept
    {
#if qDebug
        Invariant_ ();
#endif
    }
#if qDebug
    template <typename T, size_t BUF_SIZE>
    void InlineBuffer<T, BUF_SIZE>::Invariant_ () const noexcept
    {
        Assert (capacity () >= size ());
        ValidateGuards_ ();
    }
    template <typename T, size_t BUF_SIZE>
    void InlineBuffer<T, BUF_SIZE>::ValidateGuards_ () const noexcept
    {
        Assert (::memcmp (kGuard1_, fGuard1_, sizeof (kGuard1_)) == 0);
        Assert (::memcmp (kGuard2_, fGuard2_, sizeof (kGuard2_)) == 0);
    }
#endif
    template <typename T, size_t BUF_SIZE>
    inline T* InlineBuffer<T, BUF_SIZE>::BufferAsT_ () noexcept
    {
        return reinterpret_cast<T*> (&fInlinePreallocatedBuffer_[0]);
    }
    template <typename T, size_t BUF_SIZE>
    constexpr const T* InlineBuffer<T, BUF_SIZE>::BufferAsT_ () const noexcept
    {
        return reinterpret_cast<const T*> (&fInlinePreallocatedBuffer_[0]);
    }
    template <typename T, size_t BUF_SIZE>
    inline void InlineBuffer<T, BUF_SIZE>::DestroyElts_ (T* start, T* end) noexcept
    {
        if constexpr (not is_trivially_destructible_v<T>) {
            for (auto i = start; i != end; ++i) {
                destroy_at (i);
            }
        }
    }
    template <typename T, size_t BUF_SIZE>
    inline byte* InlineBuffer<T, BUF_SIZE>::LiveDataAsAllocatedBytes_ () noexcept
    {
        Require (not UsingInlinePreallocatedBuffer_ ());
        return reinterpret_cast<byte*> (fLiveData_);
    }
    template <typename T, size_t BUF_SIZE>
    inline byte* InlineBuffer<T, BUF_SIZE>::Allocate_ (size_t bytes)
    {
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"") // crazy warning from g++-11
        void* p = ::malloc (bytes);
#if qCompilerAndStdLib_release_bld_error_bad_obj_offset_Buggy
        if (p == nullptr) {
            throw bad_alloc{};
        }
#else
        Execution::ThrowIfNull (p);
#endif
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"") // crazy warning from g++-11
        return reinterpret_cast<byte*> (p);
    }
    template <typename T, size_t BUF_SIZE>
    inline void InlineBuffer<T, BUF_SIZE>::Deallocate_ (byte* bytes) noexcept
    {
        if (bytes != nullptr) [[likely]] {
            ::free (bytes);
        }
    }
    template <typename T, size_t BUF_SIZE>
    inline byte* InlineBuffer<T, BUF_SIZE>::Reallocate_ (byte* bytes, size_t n)
        requires (is_trivially_copyable_v<T>)
    {
        if (n == 0) {
            Deallocate_ (bytes);
            return nullptr;
        }
        else {
            if (bytes == nullptr) {
                return Allocate_ (n);
            }
            else {
                byte* p = reinterpret_cast<byte*> (::realloc (bytes, n));
#if qCompilerAndStdLib_release_bld_error_bad_obj_offset_Buggy
                if (p == nullptr) {
                    throw bad_alloc{};
                }
#else
                Execution::ThrowIfNull (p);
#endif
                return p;
            }
        }
    }
    template <typename T, size_t BUF_SIZE>
    constexpr bool InlineBuffer<T, BUF_SIZE>::UsingInlinePreallocatedBuffer_ () const noexcept
    {
        return fLiveData_ == BufferAsT_ ();
    }

}
