/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_StringBuilder_h_
#define _Stroika_Foundation_Characters_StringBuilder_h_ 1

#include "../StroikaPreComp.h"

#include "../Debug/AssertExternallySynchronizedMutex.h"
#include "../Memory/InlineBuffer.h"

#include "String.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 * Description:
 *
 *
 * TODO:
 *      @todo   Add 'formatting' like wstringstream - so you can append ints, etc. But unclear how to
 *              format these, so think out carefully
 *
 *              Maybe find a way to re-use the stream inserters from iostream (vector to them?)
 *
 *      @todo   Add InsertAt() methods - like from String class (before I deprecate them).
 *              https://stroika.atlassian.net/browse/STK-34
 *
 *      @todo   Consider adding operator==, and or other String methods - esp so can compare as a value
 *              with String. Or maybe add As<> method, and force compare As<String> ()?
 *
 *      @todo   Think about how to add support for STL manipulator/inserters like endl;
 *
 */

namespace Stroika::Foundation::Characters {

    /**
     *
     *  \note   Uses Execution::ExternallySynchronizedLock - so you must externally assure this isn't updated by
     *          one thread while being accessed on another.
     *
     *  @see String
     *  @see .Net StringBuilder - http://msdn.microsoft.com/en-us/library/system.text.stringbuilder(v=vs.110).aspx
     *  @see Java StringBuilder - http://docs.oracle.com/javase/7/docs/api/java/lang/StringBuilder.html
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    class StringBuilder {
    public:
        /**
         */
        using value_type = Character;

    public:
        /**
         */
        StringBuilder () noexcept            = default;
        StringBuilder (const StringBuilder&) = default;
        StringBuilder (const String& initialValue);
        template <Character_Compatible CHAR_T>
        StringBuilder (span<const CHAR_T> initialValue);

    public:
        nonvirtual StringBuilder& operator= (const StringBuilder& rhs) = default;
        nonvirtual StringBuilder& operator= (const String& rhs);

    public:
        /**
         *  Append the given argument characters to this buffer.
         * 
         *  argument characters can be given by
         *      o   span<unicode (or narrow ASCII) characters>
         *      o   const T* - nul-terminated array of unicode (or narrow ASCII) characters
         *      o   basic_string<unicode (or narrow ASCII) characters>
         *      o   basic_string_view<unicode (or narrow ASCII) characters>
         *      o   String
         *      o   Character
         */
        template <Character_Compatible CHAR_T>
        nonvirtual void Append (span<const CHAR_T> s);
        template <Character_Compatible CHAR_T>
        nonvirtual void Append (const CHAR_T* s);
        template <Character_IsUnicodeCodePointOrPlainChar CHAR_T>
        nonvirtual void Append (const basic_string<CHAR_T>& s);
        template <Character_IsUnicodeCodePointOrPlainChar CHAR_T>
        nonvirtual void Append (const basic_string_view<CHAR_T>& s);
        nonvirtual void Append (const String& s);
        nonvirtual void Append (Character c);
        //    nonvirtual void Append (wchar_t c); // @todo probably deprecate this...

    public:
        /**
         *  Alias for Append
         * 
         *      @todo fix the requires statement - not sure how todo this
         */
        template <typename APPEND_ARG_T>
        nonvirtual StringBuilder& operator+= (APPEND_ARG_T&& a);
        //requires (requires (APPEND_ARG_T a) { Append (a); })

    public:
        /**
         *  Alias for Append
         * 
         *      @todo fix the requires statement - not sure how todo this
         */
        template <typename APPEND_ARG_T>
        nonvirtual StringBuilder& operator<< (APPEND_ARG_T&& a);
        //requires (requires (APPEND_ARG_T a) { Append (a); })

    public:
        /**
         */
        nonvirtual void push_back (Character c);

    public:
        /**
         *  returns number of characters (not bytes, not including any possible NUL-terminator)
         */
        nonvirtual size_t size () const noexcept;

    public:
        /**
         *  Returns true if this is an empty string (aka iff size () == 0);
         */
        nonvirtual bool empty () const noexcept;

    public:
        /**
         */
        nonvirtual Character GetAt (size_t index) const noexcept;

    public:
        /**
         */
        nonvirtual void SetAt (Character item, size_t index) noexcept;

    public:
        /**
         * Only specifically specialized variants are supported. Convert String losslessly into a
         *      o   String or
         *      o   wstring
         */
        template <typename T>
        nonvirtual T As () const;
        template <typename T>
        nonvirtual void As (T* into) const;

    public:
        /*
         *  explicit operator T () provides an alterntive syntax to As<> - depending on user
         *  preference or context. Note - its important that this is explicit - to avoid
         *  creating overload problems.
         */
        nonvirtual explicit operator String () const;
        nonvirtual explicit operator wstring () const;

    public:
        /**
         */
        nonvirtual void clear () noexcept;

    public:
        /**
         *  mimic wstringstream method
         */
        nonvirtual String str () const;

    public:
        /**
         *  STL-ish alias for size ()
         */
        nonvirtual size_t length () const noexcept;

    public:
        /**
         *  \brief access a span of data located inside the StringBuilder. Return internal pointer, or pointer internal to possiblyUsedBuffer
         * 
         *  \note Lifetime of resulting span is ONLY until the next change to the StackBuffer OR the StringBuilder.
         *  \note The pointer MIGHT refer to data inside the (possibly resized) StackBuffer, or be internal to the StringBuilder
         * 
         *  The point of this queer API is too allow accessing the internal data by pointer, but allow StringBuilder to change
         *  its internal representation (not necessarily matching the kind of string being requested).
         * 
         *  \note Caller should ignore the size of possiblyUsedBuffer; its for internal use inside of GetData() - and may not match the size
         *        of the resulting string/span. Note also, the span will not in general be NUL-terminated.
         * 
         *  \note Why use this function?
         *        You would think the point of StringBuilder was to - well - build a string - right? So why not use the str() API.
         *        Well, that allocates memory, which must be freed, and that is not cost free. For some short-lived strings, it CAN
         *        be cheaper to just peek at the constructed in memory stack based String already being produced in this StringBuilder.
         *      
         *        But this needs to be done in a way with data hiding (so we can change the internal represnetation of the StringBuilder class as needed)
         *        and with respect for the possability that the string could be large (so break out of any small-string optimizations).
         * 
         *        Passing in a reference to the 'StackBuffer' class is a compromise among all these considerations. The only cost
         *        is initializing a pointer, and checking that pointer on destruction, if no memory allocation is needed.
         * 
         *  \par Example Usage:
         *      \code
         *          Memory::StackBuffer<wchar_t> probablyIgnoredBuf;
         *          span<const wchar_t> s = sb.GetData (&probablyIgnoredBuf);
         *      \encode
         * 
         *      \code
         *          Memory::StackBuffer<wchar_t> probablyIgnoredBuf;
         *          out.Write (sb.GetData (&probablyIgnoredBuf));
         *      \encode
         */
        template <Character_Compatible CHAR_T>
        nonvirtual span<const CHAR_T> GetData (Memory::StackBuffer<CHAR_T>* probablyIgnoredBuf) const
            requires (not is_const_v<CHAR_T>);

    public:
        /**
         *  Returns the amount of space reserved - before memory allocation will be needed to grow. 
         *
         *  @see reserve
         */
        nonvirtual size_t capacity () const noexcept;

    public:
        /**
         *  Provide a hint as to how much (contiguous) space to reserve. There is no need to call
         *  this but when the total size is known in advance, it can improve performance.
         *
         *  @see capacity
         */
        nonvirtual void reserve (size_t newCapacity);

    public:
        [[deprecated ("DESUPPORTED Since v3.0d1, so we can change internal buffer rep")]] const wchar_t* begin ();
        [[deprecated ("DESUPPORTED Since v3.0d1, so we can change internal buffer rep")]] const wchar_t* end ();
        [[deprecated ("DESUPPORTED Since v3.0d1, so we can change internal buffer rep")]] const wchar_t* c_str () const;
#if 0
        {
            // @todo PROBABLY DEPREACTE else -fix the constness at least - and make this WriteContext...
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fAssertExternallySyncrhonized_};
            fData_.GrowToSize_uninitialized (fLength_ + 1);
            fData_[fLength_] = '\0';
            return fData_.begin ();
        }
#endif
        [[deprecated ("Since Stroika v3.0d1, use span{} argument")]] StringBuilder (const wchar_t* start, const wchar_t* end)
        {
            Append (span{start, end});
        }
        [[deprecated ("Since Stroika v3.0d1, use span{} argument")]] void Append (const char16_t* s, const char16_t* e)
        {
            Append (span{s, e});
        }
        [[deprecated ("Since Stroika v3.0d1, use span{} argument")]] void Append (const char32_t* s, const char32_t* e)
        {
            Append (span{s, e});
        }
        [[deprecated ("Since Stroika v3.0d1, use span{} argument")]] void Append (const wchar_t* s, const wchar_t* e)
        {
            Append (span{s, e});
        }
        [[deprecated ("Since Stroika v3.0d1, use span{} argument")]] void Append (const Character* s, const Character* e)
        {
            Append (span{s, e});
        }

    private:
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fAssertExternallySyncrhonized_;

    private:
        mutable Memory::InlineBuffer<char32_t> fData_{0};   // maybe nul-terminated
        size_t                                 fLength_{0}; // seperate from Buffer<>::size () ** but why **
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "StringBuilder.inl"

#endif /*_Stroika_Foundation_Characters_StringBuilder_h_*/
