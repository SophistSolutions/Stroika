/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_StringBuilder_h_
#define _Stroika_Foundation_Characters_StringBuilder_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/Memory/InlineBuffer.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 * Description:
 *
 * TODO:
 *      @todo   Add InsertAt() methods - like from String class (before I deprecate them).
 *              https://stroika.atlassian.net/browse/STK-34
 *
 *      @todo   Think about how to add support for STL manipulator/inserters like endl;
 * 
 *      @todo   Consider adding (back) reserve/capacity methods, but be sure to document these are in units of 
 *              BufferElementType not characters.
 */
namespace Stroika::Foundation::Characters {
    template <typename T>
    String UnoverloadedToString (const T& t);
}

namespace Stroika::Foundation::Characters {

    namespace Private_ {
        template <typename T>
        concept IToString = requires (T t) {
            {
                UnoverloadedToString (t)
            } -> convertible_to<Characters::String>;
        };
    }

    /**
     *  \brief rarely used  directly - defaults generally fine
     * 
     *  BUF_CHAR_T of char32_t probably does better if definitely using alot of wide unicode characters.
     *  BUF_CHAR_T of char8_t probably best for mostly ASCII text. Note - GetCharAt/SetCharAt very slow
     *  unless using char32_t.
     * 
     *  Maybe easy to support all at once.
     */
    template <IUNICODECanAlwaysConvertTo BUF_CHAR_T = char8_t, size_t INLINE_BUF_SIZE = 128>
    struct StringBuilder_Options {
        /**
         *  Note that kInlineBufferSize is measured in 'buffer elements' - not (ncessarily the same as) bytes or Characters.
         */
        static constexpr size_t kInlineBufferSize = INLINE_BUF_SIZE;

        /**
         */
        using BufferElementType = BUF_CHAR_T;
    };

    /**
     *  \brief Similar to String, but intended to more efficiently construct a String. Mutable type (String is largly immutable).
     * 
     *  @see String
     *  @see .Net StringBuilder - http://msdn.microsoft.com/en-us/library/system.text.stringbuilder(v=vs.110).aspx
     *  @see Java StringBuilder - http://docs.oracle.com/javase/7/docs/api/java/lang/StringBuilder.html
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename OPTIONS = StringBuilder_Options<>>
    class [[nodiscard]] StringBuilder {
    public:
        /**
         */
        using value_type = Character;

    public:
        static constexpr size_t kInlineBufferSize = OPTIONS::kInlineBufferSize;

    public:
        using BufferElementType = typename OPTIONS::BufferElementType;
        static_assert (IUNICODECanAlwaysConvertTo<BufferElementType>);

    public:
        /**
         */
        StringBuilder () noexcept            = default;
        StringBuilder (const StringBuilder&) = default;
        StringBuilder (const String& initialValue);
        template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
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
        template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
        nonvirtual void Append (span<const CHAR_T> s);
        template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
        nonvirtual void Append (span<CHAR_T> s);
        template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
        nonvirtual void Append (const CHAR_T* s);
        template <IStdBasicStringCompatibleCharacter CHAR_T>
        nonvirtual void Append (const basic_string<CHAR_T>& s)
            requires (IUNICODECanUnambiguouslyConvertFrom<CHAR_T>);
        template <IStdBasicStringCompatibleCharacter CHAR_T>
        nonvirtual void Append (const basic_string_view<CHAR_T>& s)
            requires (IUNICODECanUnambiguouslyConvertFrom<CHAR_T>);
        nonvirtual void Append (const String& s);
        template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
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
            if constexpr (requires (StringBuilder& s, APPEND_ARG_T&& a) { s.Append (forward<APPEND_ARG_T> (a)); }) {
                Append (forward<APPEND_ARG_T> (a));
            }
            else {
                Append (Characters::UnoverloadedToString (forward<APPEND_ARG_T> (a)));
            }
            return *this;
        }
#else
        ;
#endif

        // clang-format off
    public:
        /**
         *  Alias for Append if that would work, and otherwise alias for Append (ToString(arg)), if that would work;
         */
        template <typename APPEND_ARG_T>
        nonvirtual auto
        operator<< (APPEND_ARG_T&& a)
            -> StringBuilder& requires (Characters::Private_::IToString<APPEND_ARG_T> or requires (StringBuilder& s, APPEND_ARG_T&& a) { s.Append (forward<APPEND_ARG_T> (a)); })
#if qCompilerAndStdLib_template_Requires_templateDeclarationMatchesOutOfLine_Buggy
        {
            if constexpr (requires (StringBuilder& s, APPEND_ARG_T&& a) { s.Append (forward<APPEND_ARG_T> (a)); }) {
                Append (forward<APPEND_ARG_T> (a));
            }
            else {
                Append (Characters::UnoverloadedToString (forward<APPEND_ARG_T> (a)));
            }
            return *this;
        }
#else
        ;
#endif

    public:
        /**
         */
        nonvirtual void push_back (Character c);
        // clang-format on

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
         *  \note - this can be very slow if OPTIONS::BufferElementType != char32_t
         */
        nonvirtual Character GetAt (size_t index) const noexcept;

    public:
        /**
         *  \note - this can be very slow if OPTIONS::BufferElementType != char32_t
         */
        nonvirtual void SetAt (Character item, size_t index) noexcept;

    public:
        /**
         *  Change the size of this object to sz = where sz must be <= size()
         */
        nonvirtual void ShrinkTo (size_t sz) noexcept;

    public:
        /**
         * Only specifically specialized variants are supported. Convert String losslessly into a
         *      o   String or
         *      o   wstring
         *      o   u8string
         *      o   u16string
         *      o   u32string
         */
        template <typename RESULT_T>
        nonvirtual RESULT_T As () const
            requires (Configuration::IAnyOf<RESULT_T, String, wstring, u8string, u16string, u32string>);

    public:
        /*
         *  explicit operator T () provides an alternative syntax to As<> - depending on user
         *  preference or context. Note - its important that this is explicit - to avoid
         *  creating overload problems.
         */
        nonvirtual /*explicit*/ operator String () const;
        nonvirtual explicit operator wstring () const;
        nonvirtual explicit operator u8string () const;
        nonvirtual explicit operator u16string () const;
        nonvirtual explicit operator u32string () const;

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
         *  STL-ish alias for size () - number of characters, not bytes or code-points...
         */
        nonvirtual size_t length () const noexcept;

    public:
        /**
         *  ONLY valid til the next non-const call to StringBuilder.
         *  See also GetData (to select a different charType).
         */
        nonvirtual span<BufferElementType> data ();
        nonvirtual span<const BufferElementType> data () const;

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
        template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
        nonvirtual span<const CHAR_T> GetData (Memory::StackBuffer<CHAR_T>* probablyIgnoredBuf) const
            requires (not is_const_v<CHAR_T>);

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
        template <typename RESULT_T>
        [[deprecated ("Since Stroika v3.0d2 - use As/0")]] void As (RESULT_T* into) const
            requires (is_same_v<RESULT_T, String> or is_same_v<RESULT_T, wstring>)
        {
            RequireNotNull (into);
            if constexpr (is_same_v<RESULT_T, String>) {
                *into = str ();
            }
            if constexpr (is_same_v<RESULT_T, wstring>) {
                *into = str ().template As<wstring> ();
            }
        }

    private:
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fAssertExternallySyncrhonized_;

    private:
        Memory::InlineBuffer<BufferElementType, kInlineBufferSize> fData_{}; // not nul-terminated
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "StringBuilder.inl"

#endif /*_Stroika_Foundation_Characters_StringBuilder_h_*/
