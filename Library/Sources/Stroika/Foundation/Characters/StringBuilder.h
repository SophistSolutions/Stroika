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
 *      @todo   Think about how to add support for STL manipulator/inserters like endl;
 *
 */

namespace Stroika::Foundation::Characters {

    /**
     *  \brief rarely used - defaults generally fine
     * 
     *  Maybe easy to support all at once.
     */
    template <Character_UNICODECanAlwaysConvertTo BUF_CHAR_T = char32_t, size_t INLINE_BUF_SIZE = 128>
    struct StringBuilder_Options {
        static constexpr size_t kInlineBufferSize = INLINE_BUF_SIZE;
        using BufferElementType                   = BUF_CHAR_T;
    };

    /**
     *  @see String
     *  @see .Net StringBuilder - http://msdn.microsoft.com/en-us/library/system.text.stringbuilder(v=vs.110).aspx
     *  @see Java StringBuilder - http://docs.oracle.com/javase/7/docs/api/java/lang/StringBuilder.html
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename OPTIONS = StringBuilder_Options<>>
    class StringBuilder {
    public:
        /**
         */
        using value_type = Character;

    public:
        static constexpr size_t kInlineBufferSize = OPTIONS::kInlineBufferSize;

    public:
        using BufferElementType = typename OPTIONS::BufferElementType;
        static_assert (Character_UNICODECanAlwaysConvertTo<BufferElementType>);

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
         * 
         *  This function appends as IF the argument was converted to a UNICODE string, and then
         *  appended.
         */
        template <Character_Compatible CHAR_T>
        nonvirtual void Append (span<const CHAR_T> s);
        template <Character_Compatible CHAR_T>
        nonvirtual void Append (span<CHAR_T> s);
        template <Character_Compatible CHAR_T>
        nonvirtual void Append (const CHAR_T* s);
        template <Character_IsUnicodeCodePointOrPlainChar CHAR_T>
        nonvirtual void Append (const basic_string<CHAR_T>& s);
        template <Character_IsUnicodeCodePointOrPlainChar CHAR_T>
        nonvirtual void Append (const basic_string_view<CHAR_T>& s);
        nonvirtual void Append (const String& s);
        template <Character_UNICODECanUnambiguouslyConvertFrom CHAR_T>
        nonvirtual void Append (CHAR_T c);

    public:
        /**
         *  Alias for Append
         */
        template <typename APPEND_ARG_T>
        nonvirtual auto operator+= (APPEND_ARG_T&& a)
            -> StringBuilder& requires (requires (StringBuilder& s, APPEND_ARG_T&& a) { s.Append (forward<APPEND_ARG_T> (a)); })
#if qCompilerAndStdLib_template_Requires_templateDeclarationMatchesOutOfLine_Buggy
        {
            Append (forward<APPEND_ARG_T> (a));
            return *this;
        }
#else
        ;
#endif

        public :
            /**
             *  Alias for Append
             */
            template <typename APPEND_ARG_T>
            nonvirtual auto
            operator<< (APPEND_ARG_T&& a)
                -> StringBuilder& requires (requires (StringBuilder& s, APPEND_ARG_T&& a) { s.Append (forward<APPEND_ARG_T> (a)); })
#if qCompilerAndStdLib_template_Requires_templateDeclarationMatchesOutOfLine_Buggy
        {
            Append (forward<APPEND_ARG_T> (a));
            return *this;
        }
#else
        ;
#endif

            public :
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
        template <typename RESULT_T>
        nonvirtual RESULT_T As () const
            requires (is_same_v<RESULT_T, String> or is_same_v<RESULT_T, wstring>);
        template <typename RESULT_T>
        nonvirtual void As (RESULT_T* into) const
            requires (is_same_v<RESULT_T, String> or is_same_v<RESULT_T, wstring>);

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

#if 0
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
#endif

    public:
        [[deprecated ("DESUPPORTED Since v3.0d1, so we can change internal buffer rep")]] const wchar_t* begin ();
        [[deprecated ("DESUPPORTED Since v3.0d1, so we can change internal buffer rep")]] const wchar_t* end ();
        [[deprecated ("DESUPPORTED Since v3.0d1, so we can change internal buffer rep")]] const wchar_t* c_str () const;
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
        // Super unclear what size to use. Default is 1024 characters.
        // Using smaller may provide better processor cache friendliness
        // and frequently its enuf. And when not, could be much more
        // @todo maybe expose this parameter in StringBuilder TEMPLATE
        mutable Memory::InlineBuffer<BufferElementType, kInlineBufferSize> fData_{}; // not nul-terminated
    };
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "StringBuilder.inl"

#endif /*_Stroika_Foundation_Characters_StringBuilder_h_*/
