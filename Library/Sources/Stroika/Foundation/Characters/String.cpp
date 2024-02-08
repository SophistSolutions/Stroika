/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <algorithm>
#include <climits>
#include <cstdarg>
#include <istream>
#include <regex>
#include <string>

#include "../Characters/CString/Utilities.h"
#include "../Configuration/Empty.h"
#include "../Containers/Set.h"
#include "../Containers/Support/ReserveTweaks.h"
#include "../Cryptography/Digest/Algorithm/SuperFastHash.h"
#include "../Debug/Cast.h"
#include "../Execution/Exceptions.h"
#include "../Execution/Throw.h"
#include "../Math/Common.h"
#include "../Memory/BlockAllocated.h"
#include "../Memory/Common.h"
#include "../Memory/Span.h"
#include "../Memory/StackBuffer.h"

#include "RegularExpression.h"
#include "SDKString.h"
#include "StringBuilder.h"

#include "String.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

using Memory::StackBuffer;
using Traversal::Iterator;

namespace {

    /**
     *  Helper for sharing implementation code on string reps
     *  This REP is templated on CHAR_T. The key is that ALL characters for that string fit inside
     *  CHAR_T, so that the implementation can store them as an array, and index.
     *  So mixed 1,2,3 byte characters all get stored in a char32_t array, and a string with all ascii
     *  characters get stored in a char (1byte stride) array.
     * 
     *  \note - the KEY design choice in StringRepHelperAllFitInSize_::Rep<CHAR_T> is that it contains no
     *        multi-code-point characters. This is what allows the simple calculation of array index
     *        to character offset. So use 
     *              StringRepHelperAllFitInSize_::Rep<ASCII> for ascii text
     *              StringRepHelperAllFitInSize_::Rep<LATIN1> for ISOLatin1 text
     *              StringRepHelperAllFitInSize_::Rep<char16_t> for isolatin/anything which is a 2-byte unicode char (not surrogates)
     *              StringRepHelperAllFitInSize_::Rep<char32_t> for anything else - this always works
     */
    struct StringRepHelperAllFitInSize_ : String {
        template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
        struct Rep : public _IRep {
        private:
            using inherited = _IRep;

        protected:
            span<const CHAR_T> _fData;

#if qDebug
        private:
            mutable unsigned int fOutsandingIterators_{};
#endif

        protected:
            Rep () = default;
            Rep (span<const CHAR_T> s)
                requires (not same_as<CHAR_T, char8_t>) // char8 ironically involves 2-byte characters, cuz only ascii encoded as 1 byte
                : _fData{s}
            {
                if constexpr (same_as<CHAR_T, char> or same_as<CHAR_T, char8_t>) {
                    Require (Character::IsASCII (s));
                }
                // Any 8-bit sequence valid for Latin1
                if constexpr (same_as<CHAR_T, char16_t>) {
                    Require (UTFConvert::AllFitsInTwoByteEncoding (s));
                }
            }
            Rep& operator= (span<const CHAR_T> s)
            {
#if qDebug
                Require (fOutsandingIterators_ == 0);
#endif
                if constexpr (same_as<CHAR_T, char> or same_as<CHAR_T, char8_t>) {
                    Require (Character::IsASCII (s));
                }
                if constexpr (same_as<CHAR_T, char16_t>) {
                    Require (UTFConvert::AllFitsInTwoByteEncoding (s));
                }
                _fData = s;
                return *this;
            }

        public:
            // String::_IRep OVERRIDES
            virtual Character GetAt (size_t index) const noexcept override
            {
                Require (index < _fData.size ());
                // NOTE - this is safe because we never construct this type with surrogates
                return Character{static_cast<char32_t> (_fData[index])};
            }
            virtual PeekSpanData PeekData (optional<PeekSpanData::StorageCodePointType> /*preferred*/) const noexcept override
            {
                // IGNORE preferred, cuz we return what is in our REP - since returning a direct pointer to that data - no conversion possible
                if constexpr (same_as<CHAR_T, ASCII>) {
                    return PeekSpanData{PeekSpanData::StorageCodePointType::eAscii, {.fAscii = _fData}};
                }
                if constexpr (same_as<CHAR_T, Latin1>) {
                    return PeekSpanData{PeekSpanData::StorageCodePointType::eSingleByteLatin1, {.fSingleByteLatin1 = _fData}};
                }
                else if constexpr (sizeof (CHAR_T) == 2) {
                    // reinterpret_cast needed cuz of wchar_t case
                    return PeekSpanData{PeekSpanData::StorageCodePointType::eChar16,
                                        {.fChar16 = span<const char16_t>{reinterpret_cast<const char16_t*> (_fData.data ()), _fData.size ()}}};
                }
                else if constexpr (sizeof (CHAR_T) == 4) {
                    // reinterpret_cast needed cuz of wchar_t case
                    return PeekSpanData{PeekSpanData::StorageCodePointType::eChar32,
                                        {.fChar32 = span<const char32_t>{reinterpret_cast<const char32_t*> (_fData.data ()), _fData.size ()}}};
                }
            }

            // Overrides for Iterable<Character>
        public:
            virtual shared_ptr<Iterable<Character>::_IRep> Clone () const override
            {
                AssertNotReached (); // Since String reps now immutable, this should never be called
                return nullptr;
            }
            virtual Traversal::Iterator<value_type> MakeIterator () const override
            {
                // NOTE - UNDETECTED CALLER ERROR - if iterator constructed and used after string rep destroyed (never changed) -- LGP 2023-07-07
                struct MyIterRep_ final : Iterator<Character>::IRep, public Memory::UseBlockAllocationIfAppropriate<MyIterRep_> {
                    span<const CHAR_T> fData_; // clone span (not underlying data)
                    size_t             fIdx_{0};
#if qDebug
                    const Rep* fOwningRep_;
#endif
                    MyIterRep_ (span<const CHAR_T> data
#if qDebug
                                ,
                                const Rep* dbgRep
#endif
                                )
                        : fData_{data}
#if qDebug
                        , fOwningRep_{dbgRep}
#endif
                    {
#if qDebug
                        ++fOwningRep_->fOutsandingIterators_;
#endif
                    }
#if qDebug
                    virtual ~MyIterRep_ () override
                    {
                        Require (fOwningRep_->fOutsandingIterators_ > 0); // if this fails, probably cuz fOwningRep_ destroyed
                        --fOwningRep_->fOutsandingIterators_;
                    }
#endif

                    virtual unique_ptr<Iterator<Character>::IRep> Clone () const override
                    {
                        return make_unique<MyIterRep_> (fData_.subspan (fIdx_)
#if qDebug
                                                            ,
                                                        fOwningRep_
#endif
                        );
                    }
                    virtual void More (optional<Character>* result, bool advance) override
                    {
                        RequireNotNull (result);
                        if (advance) [[likely]] {
                            Require (fIdx_ < fData_.size ());
                            ++fIdx_;
                        }
                        if (fIdx_ < fData_.size ()) [[likely]] {
                            // NOTE - this is safe because we never construct this type with surrogates
                            *result = Character{static_cast<char32_t> (fData_[fIdx_])};
                        }
                        else {
                            *result = nullopt;
                        }
                    }
                    virtual bool Equals (const IRep* rhs) const override
                    {
                        RequireNotNull (rhs);
                        RequireMember (rhs, MyIterRep_);
                        const MyIterRep_* rrhs = Debug::UncheckedDynamicCast<const MyIterRep_*> (rhs);
                        return fData_.data () == rrhs->fData_.data () and fIdx_ == rrhs->fIdx_;
                    }
                };
                return Iterator<Character>
                {
                    make_unique<MyIterRep_> (this->_fData

#if qDebug
                                             ,
                                             this
#endif

                    )
                };
            }
            virtual size_t size () const override
            {
                return _fData.size ();
            }
            virtual bool empty () const override
            {
                return _fData.empty ();
            }
            virtual Traversal::Iterator<value_type> Find (const function<bool (Configuration::ArgByValueType<value_type> item)>& that,
                                                          Execution::SequencePolicy seq) const override
            {
                return inherited::Find (that, seq); // @todo rewrite to operatoe of fData_
            }
        };
    };

    /**
     *  Simple string rep, which dynamically allocates its storage on the heap, through an indirect pointer reference.
     *  \note   This class may assure nul-terminated (kAddNullTerminator_), and so 'capacity' always at least one greater than length.
     */
    struct DynamicallyAllocatedString : StringRepHelperAllFitInSize_ {
        template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
        struct Rep final : public StringRepHelperAllFitInSize_::Rep<CHAR_T>, public Memory::UseBlockAllocationIfAppropriate<Rep<CHAR_T>> {
        private:
            using inherited = StringRepHelperAllFitInSize_::Rep<CHAR_T>;

        public:
            Rep (span<const CHAR_T> t1)
                : inherited{mkBuf_ (t1)}
            {
            }
            Rep ()           = delete;
            Rep (const Rep&) = delete;

        public:
            nonvirtual Rep& operator= (const Rep&) = delete;

        public:
            virtual ~Rep () override
            {
                delete[] this->_fData.data ();
            }

        private:
            static span<CHAR_T> mkBuf_ (size_t length)
            {
                size_t capacity = AdjustCapacity_ (length);
                Assert (length <= capacity);
                if constexpr (kAddNullTerminator_) {
                    Assert (length + 1 <= capacity);
                }
                CHAR_T* newBuf = new CHAR_T[capacity];
                return span{newBuf, capacity};
            }
            static span<CHAR_T> mkBuf_ (span<const CHAR_T> t1)
            {
                size_t       len = t1.size ();
                span<CHAR_T> buf = mkBuf_ (len); // note buf span is over capacity, not size
                Assert (buf.size () >= len);
                auto result = Memory::CopySpanData (t1, buf);
                if constexpr (kAddNullTerminator_) {
                    Assert (len + 1 <= buf.size ());
                    *(buf.data () + len) = '\0';
                }
                return result; // return span of just characters, even if we have extra NUL-byte (outside span)
            }

        public:
            // String::_IRep OVERRIDES
            virtual const wchar_t* c_str_peek () const noexcept override
            {
                if constexpr (kAddNullTerminator_) {
                    Assert (*(this->_fData.data () + this->_fData.size ()) == '\0'); // dont index into buf cuz we cheat and go one past end on purpose
                    return reinterpret_cast<const wchar_t*> (this->_fData.data ());
                }
                else {
                    return nullptr;
                }
            }

        private:
            // Stick nul-terminator byte just past the end of the span
            static constexpr bool kAddNullTerminator_ = sizeof (CHAR_T) == sizeof (wchar_t); // costs nothing to nul-terminate in this case

        private:
            static size_t AdjustCapacity_ (size_t initialCapacity)
            {
                size_t result = initialCapacity;
                if constexpr (kAddNullTerminator_) {
                    ++result;
                }
                return result;
            }
        };
    };

    /**
     *  Most Stroika strings use this 'rep': FixedCapacityInlineStorageString_
     * 
     *  This String rep is like BufferedString_, except that the storage is inline in one struct/allocation
     *  for better memory allocation performance, and more importantly, better locality of data (more cpu cache friendly)
     */
    struct FixedCapacityInlineStorageString_ : StringRepHelperAllFitInSize_ {
        template <IUNICODECanUnambiguouslyConvertFrom CHAR_T, size_t CAPACITY>
        struct Rep final : public StringRepHelperAllFitInSize_::Rep<CHAR_T>,
                           public Memory::UseBlockAllocationIfAppropriate<Rep<CHAR_T, CAPACITY>> {
        private:
            using inherited = StringRepHelperAllFitInSize_::Rep<CHAR_T>;

        private:
            bool IncludesNullTerminator_ () const
            {
                if constexpr (sizeof (CHAR_T) == sizeof (wchar_t)) {
                    return this->_fData.size () < CAPACITY; // else no room
                }
                else {
                    return false;
                }
            }

        private:
            CHAR_T fBuf_[CAPACITY];

        public:
            Rep (span<const CHAR_T> t1)
                : inherited{}
            {
                // must do this logic after base construction since references data member which doesn't exist
                // til after base class construction. SHOULDNT really matter (since uninitialized data), but on
                // g++-11, and other compilers, detected as vptr UB violation if we access first
                Require (t1.size () <= CAPACITY);
                inherited::operator= (Memory::CopySpanData (t1, span<CHAR_T>{fBuf_}));
                if (IncludesNullTerminator_ ()) {
                    Assert (t1.size () + 1 <= CAPACITY);
                    fBuf_[t1.size ()] = CHAR_T{'\0'};
                }
            }
            Rep ()           = delete;
            Rep (const Rep&) = delete;

        public:
            nonvirtual Rep& operator= (const Rep&) = delete;

        public:
            // String::_IRep OVERRIDES
            virtual const wchar_t* c_str_peek () const noexcept override
            {
                if (IncludesNullTerminator_ ()) {
                    Assert (*(this->_fData.data () + this->_fData.size ()) == '\0'); // dont index into buf cuz we cheat and go one past end on purpose
                    return reinterpret_cast<const wchar_t*> (this->_fData.data ());
                }
                else {
                    return nullptr;
                }
            }
        };
    };

    /**
     *  For static full app lifetime string constants...
     */
    struct StringConstant_ : public StringRepHelperAllFitInSize_ {
        using inherited = String;

        template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
        class Rep final : public StringRepHelperAllFitInSize_::Rep<CHAR_T>, public Memory::UseBlockAllocationIfAppropriate<Rep<CHAR_T>> {
        private:
            using inherited = StringRepHelperAllFitInSize_::Rep<CHAR_T>;

        public:
            Rep (span<const CHAR_T> s)
                : inherited{s} // don't copy memory - but copy raw pointers! So they MUST BE (externally promised) 'externally owned for the application lifetime and constant' - like c++ string constants
            {
                if constexpr (same_as<CHAR_T, wchar_t>) {
#if qDebug
                    const CHAR_T* start = s.data ();
                    const CHAR_T* end   = start + s.size ();
                    // NO - we allow embedded nuls, but require NUL-termination - so this is wrong - Require (start + ::wcslen (start) == end);
                    Require (*end == '\0' and start + ::wcslen (start) <= end);
#endif
                }
            }

        public:
            // String::_IRep OVERRIDES
            virtual const wchar_t* c_str_peek () const noexcept override
            {
                // if used for appropriately sized character (equiv to wchar_t) - then use that nul-terminated string
                // and else return nullptr, so it will get wrapped
                if constexpr (same_as<CHAR_T, wchar_t>) {
                    // we check/require this in CTOR, so should still be true
                    const wchar_t* start = reinterpret_cast<const wchar_t*> (this->_fData.data ());
#if qDebug
                    const wchar_t* end = start + this->_fData.size ();
                    Assert (*end == '\0' and start + ::wcslen (start) <= end); // less or equal because you can call c_str() even through the string has embedded nuls
#endif
                    return start;
                }
                else {
                    return nullptr;
                }
            }
        };
    };

    /*
     *  Used for String{move(some_string)}
     */
    struct StdStringDelegator_ : public StringRepHelperAllFitInSize_ {
        using inherited = String;

        template <IStdBasicStringCompatibleCharacter CHAR_T>
        class Rep final : public StringRepHelperAllFitInSize_::Rep<CHAR_T>, public Memory::UseBlockAllocationIfAppropriate<Rep<CHAR_T>> {
        private:
            using inherited = StringRepHelperAllFitInSize_::Rep<CHAR_T>;

        public:
            Rep (basic_string<CHAR_T>&& s)
                : inherited{span<const CHAR_T>{}}
                , fMovedData_{move (s)}
            {
                inherited::operator= (span{fMovedData_.data (), fMovedData_.size ()}); // must grab after move
            }

        public:
            // String::_IRep OVERRIDES
            virtual const wchar_t* c_str_peek () const noexcept override
            {
                if constexpr (same_as<CHAR_T, wchar_t>) {
                    return fMovedData_.c_str ();
                }
                else {
                    return nullptr;
                }
            }

        private:
            basic_string<CHAR_T> fMovedData_;
        };
    };

    /**
     *  Delegate to original String::Rep, and add in support for c_str ()
     */
    struct StringWithCStr_ : public String {
    public:
        class Rep final : public _IRep, public Memory::UseBlockAllocationIfAppropriate<Rep> {
        private:
            shared_ptr<_IRep> fUnderlyingRep_;
            wstring           fCString_;

        public:
            // Caller MUST ASSURE generates right size of Rep based on size in underlyingRepPDS
            Rep (const shared_ptr<_IRep>& underlyingRep)
                : fUnderlyingRep_{underlyingRep}
                , fCString_{}
            {
                Memory::StackBuffer<wchar_t> possibleUsedBuf;
                auto                         wideSpan = String::GetData<wchar_t> (underlyingRep->PeekData (nullopt), &possibleUsedBuf);
                fCString_.assign (wideSpan.begin (), wideSpan.end ());
            }

            // Overrides for Iterable<Character>
        public:
            virtual shared_ptr<Iterable<Character>::_IRep> Clone () const override
            {
                return fUnderlyingRep_->Clone ();
            }
            virtual Traversal::Iterator<value_type> MakeIterator () const override
            {
                return fUnderlyingRep_->MakeIterator ();
            }
            virtual size_t size () const override
            {
                return fUnderlyingRep_->size ();
            }
            virtual bool empty () const override
            {
                return fUnderlyingRep_->empty ();
            }
            virtual Traversal::Iterator<value_type> Find (const function<bool (Configuration::ArgByValueType<value_type> item)>& that,
                                                          [[maybe_unused]] Execution::SequencePolicy seq) const override
            {
                return fUnderlyingRep_->Find (that, seq);
            }

            // String::_IRep overrides - delegate
        public:
            virtual Character GetAt (size_t index) const noexcept override
            {
                return fUnderlyingRep_->GetAt (index);
            }
            virtual PeekSpanData PeekData ([[maybe_unused]] optional<PeekSpanData::StorageCodePointType> preferred) const noexcept override
            {
                return fUnderlyingRep_->PeekData (preferred);
            }
            virtual const wchar_t* c_str_peek () const noexcept override
            {
                return fCString_.c_str ();
            }
        };
    };
}

namespace {
    template <typename FACET>
    struct deletable_facet_ final : FACET {
        template <typename... Args>
        deletable_facet_ (Args&&... args)
            : FACET{forward<Args> (args)...}
        {
        }
        ~deletable_facet_ () = default;
    };
}

/*
 ********************************************************************************
 ******* Characters::Private_::RegularExpression_GetCompiled ********************
 ********************************************************************************
 */
const wregex& Characters::Private_::RegularExpression_GetCompiled (const RegularExpression& regExp)
{
    return regExp.GetCompiled ();
}

/*
 ********************************************************************************
 ************************************* String ***********************************
 ********************************************************************************
 */
#if !qCompilerAndStdLib_templateConstructorSpecialization_Buggy
template <>
#endif
String::String (const basic_string_view<char>& str)
    : String{(RequireExpression (Character::IsASCII (span{str.data (), str.size ()})),
              Memory::MakeSharedPtr<StringConstant_::Rep<ASCII>> (span{str.data (), str.size ()}))}
{
}

namespace {
    String mkStr_ (const basic_string_view<char8_t>& str)
    {
        // StringConstant_::Rep<char8_t> not supported
        if (Character::IsASCII (span{str.data (), str.size ()})) {
            // saves data ptr - without copying
            return String{basic_string_view{reinterpret_cast<const char*> (str.data ()), str.size ()}};
        }
        else {
            return String{span{str.data (), str.size ()}}; // copies data
        }
    }
}

#if !qCompilerAndStdLib_templateConstructorSpecialization_Buggy
template <>
#endif
String::String (const basic_string_view<char8_t>& str)
    : String{mkStr_ (str)}
{
}

#if !qCompilerAndStdLib_templateConstructorSpecialization_Buggy
template <>
#endif
String::String (const basic_string_view<char16_t>& str)
    : String{Memory::MakeSharedPtr<StringConstant_::Rep<char16_t>> (span{str.data (), str.size ()})}
{
}

#if !qCompilerAndStdLib_templateConstructorSpecialization_Buggy
template <>
#endif
String::String (const basic_string_view<char32_t>& str)
    : String{Memory::MakeSharedPtr<StringConstant_::Rep<char32_t>> (span{str.data (), str.size ()})}
{
}

#if !qCompilerAndStdLib_templateConstructorSpecialization_Buggy
template <>
#endif
String::String (const basic_string_view<wchar_t>& str)
    : String{Memory::MakeSharedPtr<StringConstant_::Rep<wchar_t>> (span{str.data (), str.size ()})}
{
    Require (str.data ()[str.length ()] == 0); // Because Stroika strings provide the guarantee that they can be converted to c_str () - we require the input memory
                                               // for these const strings are also nul-terminated.
    // DONT try to CORRECT this if found wrong, because whenever you use "stuff"sv - the string literal will always
    // be nul-terminated.
    // -- LGP 2019-01-29
}

String String::FromStringConstant (span<const ASCII> s)
{
    Require (Character::IsASCII (s));
    return String{Memory::MakeSharedPtr<StringConstant_::Rep<ASCII>> (s)};
}

String String::FromStringConstant (span<const wchar_t> s)
{
    if constexpr (sizeof (wchar_t) == 2) {
        Require (UTFConvert::AllFitsInTwoByteEncoding (s));
    }
    Require (*(s.data () + s.size ()) == '\0'); // crazy weird requirement, but done cuz "x"sv already does NUL-terminate and we can
        // take advantage of that fact - re-using the NUL-terminator for our own c_str() implementation
    return String{Memory::MakeSharedPtr<StringConstant_::Rep<wchar_t>> (s)};
}

String String::FromStringConstant (span<const char32_t> s)
{
    Require (*(s.data () + s.size ()) == '\0'); // crazy weird requirement, but done cuz "x"sv already does NUL-terminate and we can
        // take advantage of that fact - re-using the NUL-terminator for our own c_str() implementation
    return String{Memory::MakeSharedPtr<StringConstant_::Rep<char32_t>> (s)};
}

String String::FromNarrowString (span<const char> s, const locale& l)
{
    // Note: this could use CodeCvt, but directly using std::codecvt in this case pretty simple, and
    // more efficient this way --LGP 2023-02-14

    // See http://en.cppreference.com/w/cpp/locale/codecvt/~codecvt
    using Destructible_codecvt_byname = deletable_facet_<codecvt_byname<wchar_t, char, mbstate_t>>;
    Destructible_codecvt_byname cvt{l.name ()};

    // http://en.cppreference.com/w/cpp/locale/codecvt/in
    mbstate_t                    mbstate{};
    Memory::StackBuffer<wchar_t> targetBuf{s.size ()};
    const char*                  from_next;
    wchar_t*                     to_next;
    codecvt_base::result         result =
        cvt.in (mbstate, s.data (), s.data () + s.size (), from_next, targetBuf.data (), targetBuf.data () + targetBuf.size (), to_next);
    if (result != codecvt_base::ok) [[unlikely]] {
        static const auto kException_ = Execution::RuntimeErrorException{"Error converting locale multibyte string to UNICODE"sv};
        Execution::Throw (kException_);
    }
    return String{span<const wchar_t>{targetBuf.data (), static_cast<size_t> (to_next - targetBuf.data ())}};
}

shared_ptr<String::_IRep> String::mkEmpty_ ()
{
    static constexpr wchar_t kEmptyCStr_[] = L"";
    // use StringConstant_ since nul-terminated, and for now works better with CSTR - and why allocate anything...
    static const shared_ptr<_IRep> s_ = Memory::MakeSharedPtr<StringConstant_::Rep<wchar_t>> (span{std::begin (kEmptyCStr_), 0});
    return s_;
}

template <typename CHAR_T>
inline auto String::mk_nocheck_ (span<const CHAR_T> s) -> shared_ptr<_IRep>
    requires (same_as<CHAR_T, ASCII> or same_as<CHAR_T, Latin1> or same_as<CHAR_T, char16_t> or same_as<CHAR_T, char32_t>)
{
    // No check means needed checking done before, so these assertions just help enforce that
    if constexpr (same_as<CHAR_T, ASCII>) {
        Require (Character::IsASCII (s)); // avoid later assertion error
    }
    else if constexpr (same_as<CHAR_T, Latin1>) {
        // nothing to check
    }
    else if constexpr (sizeof (CHAR_T) == 2) {
        Require (UTFConvert::AllFitsInTwoByteEncoding (s)); // avoid later assertion error
    }
    else {
        // again - if larger, nothing to check
    }

    /**
     *  We want to TARGET using block-allocator of 64 bytes. This works well for typical (x86) machine
     *  caches, and divides up nicely, and leaves enuf room for a decent number of characters typically.
     * 
     *  So compute/guestimate a few sizes, and add static_asserts to check where we can. Often if these fail
     *  you can just get rid/or fix them. Not truely counted on, just trying ot generate vaguely reasonable
     *  number of characters to use.
     */
    constexpr size_t kBaseOfFixedBufSize_ = sizeof (StringRepHelperAllFitInSize_::Rep<CHAR_T>);
    static_assert (kBaseOfFixedBufSize_ < 64); // this code below assumes, so must re-tune if this ever fails
    if constexpr (qPlatform_Windows and not qDebug) {
        static_assert (kBaseOfFixedBufSize_ == 3 * sizeof (void*));
        if constexpr (sizeof (void*) == 4) {
            static_assert (kBaseOfFixedBufSize_ == 12);
        }
        else if constexpr (sizeof (void*) == 8) {
            static_assert (kBaseOfFixedBufSize_ == 24);
        }
    }
    constexpr size_t kOverheadSizeForMakeShared_ = qPlatform_Windows ? (sizeof (void*) == 4 ? 12 : 16) : sizeof (unsigned long) * 2;
#if qPlatform_Windows
    static_assert (kOverheadSizeForMakeShared_ == sizeof (_Ref_count_base)); // not critically counted on, just to debug/fix sizes
#endif
    static constexpr size_t kNElts1_ = (64 - kBaseOfFixedBufSize_ - kOverheadSizeForMakeShared_) / sizeof (CHAR_T);
    static constexpr size_t kNElts2_ = (96 - kBaseOfFixedBufSize_ - kOverheadSizeForMakeShared_) / sizeof (CHAR_T);
    static constexpr size_t kNElts3_ = (128 - kBaseOfFixedBufSize_ - kOverheadSizeForMakeShared_) / sizeof (CHAR_T);

    // These checks are NOT important, just for documentation/reference
    if constexpr (qPlatform_Windows and sizeof (CHAR_T) == 1 and not qDebug) {
        if constexpr (sizeof (void*) == 4) {
            static_assert (kNElts1_ == 40);
            static_assert (kNElts2_ == 72);
            static_assert (kNElts3_ == 104);
        }
        if constexpr (sizeof (void*) == 8) {
            static_assert (kNElts1_ == 24);
            static_assert (kNElts2_ == 56);
            static_assert (kNElts3_ == 88);
        }
    }

    static_assert (qDebug or kNElts1_ >= 6); // crazy otherwise
    static_assert (kNElts2_ > kNElts1_);     // ""
    static_assert (kNElts3_ > kNElts2_);     // ""

    static_assert (sizeof (FixedCapacityInlineStorageString_::Rep<CHAR_T, kNElts1_>) == 64 - kOverheadSizeForMakeShared_); // not quite guaranteed but close
    static_assert (sizeof (FixedCapacityInlineStorageString_::Rep<CHAR_T, kNElts2_>) == 96 - kOverheadSizeForMakeShared_);  // ""
    static_assert (sizeof (FixedCapacityInlineStorageString_::Rep<CHAR_T, kNElts3_>) == 128 - kOverheadSizeForMakeShared_); // ""

    size_t sz = s.size ();
    if (sz <= kNElts1_) {
        return Memory::MakeSharedPtr<FixedCapacityInlineStorageString_::Rep<CHAR_T, kNElts1_>> (s);
    }
    else if (sz <= kNElts2_) {
        return Memory::MakeSharedPtr<FixedCapacityInlineStorageString_::Rep<CHAR_T, kNElts2_>> (s);
    }
    else if (sz <= kNElts3_) {
        return Memory::MakeSharedPtr<FixedCapacityInlineStorageString_::Rep<CHAR_T, kNElts3_>> (s);
    }
    return Memory::MakeSharedPtr<DynamicallyAllocatedString::Rep<CHAR_T>> (s);
}

template <>
auto String::mk_ (basic_string<char>&& s) -> shared_ptr<_IRep>
{
    Character::CheckASCII (span{s.data (), s.size ()});
    return Memory::MakeSharedPtr<StdStringDelegator_::Rep<ASCII>> (move (s));
}

template <>
auto String::mk_ (basic_string<char16_t>&& s) -> shared_ptr<_IRep>
{
    if (UTFConvert::AllFitsInTwoByteEncoding (Memory::ConstSpan (span{s.data (), s.size ()}))) {
        return Memory::MakeSharedPtr<StdStringDelegator_::Rep<char16_t>> (move (s));
    }
    // copy the data if any surrogates
    Memory::StackBuffer<char32_t> wideUnicodeBuf{Memory::eUninitialized, UTFConvert::ComputeTargetBufferSize<char32_t> (span{s.data (), s.size ()})};
    return mk_nocheck_ (Memory::ConstSpan (UTFConvert::kThe.ConvertSpan (span{s.data (), s.size ()}, span{wideUnicodeBuf})));
}

template <>
auto String::mk_ (basic_string<char32_t>&& s) -> shared_ptr<_IRep>
{
    return Memory::MakeSharedPtr<StdStringDelegator_::Rep<char32_t>> (move (s));
}

template <>
auto String::mk_ (basic_string<wchar_t>&& s) -> shared_ptr<_IRep>
{
    if constexpr (sizeof (wchar_t) == 2) {
        if (UTFConvert::AllFitsInTwoByteEncoding (Memory::ConstSpan (span{s.data (), s.size ()}))) {
            return Memory::MakeSharedPtr<StdStringDelegator_::Rep<wchar_t>> (move (s));
        }
        // copy the data if any surrogates
        Memory::StackBuffer<char32_t> wideUnicodeBuf{Memory::eUninitialized,
                                                     UTFConvert::ComputeTargetBufferSize<char32_t> (span{s.data (), s.size ()})};
        return mk_nocheck_ (Memory::ConstSpan (UTFConvert::kThe.ConvertSpan (span{s.data (), s.size ()}, span{wideUnicodeBuf})));
    }
    else {
        return Memory::MakeSharedPtr<StdStringDelegator_::Rep<wchar_t>> (move (s));
    }
}

String String::Concatenate_ (const String& rhs) const
{
    // KISS, simple default 'fallthru' case
    Memory::StackBuffer<char32_t> ignoredA;
    span                          leftSpan = GetData (&ignoredA);
    Memory::StackBuffer<char32_t> ignoredB;
    span                          rightSpan = rhs.GetData (&ignoredB);
    Memory::StackBuffer<char32_t> buf{Memory::eUninitialized, leftSpan.size () + rightSpan.size ()};
    copy (leftSpan.begin (), leftSpan.end (), buf.data ());
    copy (rightSpan.begin (), rightSpan.end (), buf.data () + leftSpan.size ());
    return mk_ (span{buf});
}

void String::SetCharAt (Character c, size_t i)
{
    // @Todo - redo with check if char is acttually chanigng and if so use
    // mk/4 4 arg string maker instead.??? Or some such...
    Require (i >= 0);
    Require (i < size ());
    // Expensive, but you can use StringBuilder directly to avoid the performance costs
    StringBuilder sb{*this};
    Require (i < size ());
    sb.SetAt (c, i);
    *this = sb.str ();
}

String String::InsertAt (span<const Character> s, size_t at) const
{
    Require (at >= 0);
    Require (at <= size ());
    if (s.empty ()) {
        return *this;
    }
    Memory::StackBuffer<Character> ignored1;
    span<const Character>          thisStrData = GetData (&ignored1);
    StringBuilder                  sb{thisStrData.subspan (0, at)};
    sb.Append (s);
    sb.Append (thisStrData.subspan (at));
    return sb.str ();
}

String String::RemoveAt (size_t from, size_t to) const
{
    Require (from <= to);
    Require (to <= size ());
    if (from == to) {
        return *this;
    }
    if (from == 0) {
        return SubString (to);
    }
    _SafeReadRepAccessor accessor{this};
    size_t               length = accessor._ConstGetRep ().size ();
    if (to == length) {
        return SubString (0, from);
    }
    else {
        Memory::StackBuffer<char32_t> ignored1;
        span                          d = GetData (&ignored1);
        Memory::StackBuffer<char32_t> buf{Memory::eUninitialized, d.size () - (to - from)};
        span<char32_t>                bufSpan{buf.data (), buf.size ()};
        span                          s1 = d.subspan (0, from);
        span                          s2 = d.subspan (to);
        Memory::CopySpanData (s1, bufSpan);
        Memory::CopySpanData (s2, bufSpan.subspan (s1.size ()));
        return String{mk_ (bufSpan)};
    }
}

String String::Remove (Character c) const
{
    String tmp = {*this};
    if (auto o = tmp.Find (c, CompareOptions::eWithCase)) {
        return tmp.RemoveAt (*o);
    }
    return tmp;
}

String String::Remove (const String& subString) const
{
    if (auto o = this->Find (subString, CompareOptions::eWithCase)) {
        return this->SubString (0, *o) + this->SubString (*o + subString.length ());
    }
    return *this;
}

optional<size_t> String::Find (Character c, size_t startAt, CompareOptions co) const
{
    PeekSpanData pds = GetPeekSpanData<ASCII> ();
    // OPTIMIZED PATHS: Common case(s) and should be fast
    if (pds.fInCP == PeekSpanData::StorageCodePointType::eAscii) {
        if (c.IsASCII ()) {
            span<const char> examineSpan = pds.fAscii.subspan (startAt);
            if (co == CompareOptions::eWithCase) {
                if (auto i = std::find (examineSpan.begin (), examineSpan.end (), c.GetAsciiCode ()); i != examineSpan.end ()) {
                    return i - examineSpan.begin () + startAt;
                }
            }
            else {
                char   lc        = c.ToLowerCase ().GetAsciiCode ();
                size_t reportIdx = startAt;
                for (auto ci : examineSpan) {
                    if (tolower (ci) == lc) {
                        return reportIdx;
                    }
                    ++reportIdx;
                }
            }
            return nullopt; // not found, possibly cuz not ascii
        }
    }
    // fallback on more generic algorithm - and copy to full character objects
    //
    // performance notes
    //      Could iterate using CharAt() and that would perform better in the case where you find c early
    //      in a string, and the string is short. The problem with the current code is that it converts the
    //      entire string (could be long) and then might not look at much of the converted data.
    //      on the other hand, if our reps are either 'ascii or char32_t wide' - which we may end up with - then
    //      this isn't too bad - cuz no copying for char32_ case either...
    Memory::StackBuffer<Character> maybeIgnoreBuf;
    span<const Character>          charSpan = GetData (pds, &maybeIgnoreBuf);
    Require (startAt <= charSpan.size ());
    span<const Character> examineSpan = charSpan.subspan (startAt);
    switch (co) {
        case CompareOptions::eCaseInsensitive: {
            Character lcc = c.ToLowerCase ();
            for (auto i = examineSpan.begin (); i != examineSpan.end (); ++i) {
                if (i->ToLowerCase () == lcc) {
                    return startAt + (i - examineSpan.begin ());
                }
            }
        } break;
        case CompareOptions::eWithCase: {
            if (auto i = std::find (examineSpan.begin (), examineSpan.end (), c); i != examineSpan.end ()) {
                return startAt + i - examineSpan.begin ();
            }
        } break;
    }
    return nullopt; // not found any which way
}

optional<size_t> String::Find (const String& subString, size_t startAt, CompareOptions co) const
{
    //@todo: FIX HORRIBLE PERFORMANCE!!!
    _SafeReadRepAccessor accessor{this};
    Require (startAt <= accessor._ConstGetRep ().size ());

    size_t subStrLen = subString.size ();
    if (subStrLen == 0) {
        return (accessor._ConstGetRep ().size () == 0) ? optional<size_t>{} : 0;
    }
    if (accessor._ConstGetRep ().size () < subStrLen) {
        return {}; // important test cuz size_t is unsigned
    }

    size_t limit = accessor._ConstGetRep ().size () - subStrLen;
    switch (co) {
        case CompareOptions::eCaseInsensitive: {
            for (size_t i = startAt; i <= limit; ++i) {
                for (size_t j = 0; j < subStrLen; ++j) {
                    if (accessor._ConstGetRep ().GetAt (i + j).ToLowerCase () != subString[j].ToLowerCase ()) {
                        goto nogood1;
                    }
                }
                return i;
            nogood1:;
            }
        } break;
        case CompareOptions::eWithCase: {
            for (size_t i = startAt; i <= limit; ++i) {
                for (size_t j = 0; j < subStrLen; ++j) {
                    if (accessor._ConstGetRep ().GetAt (i + j) != subString[j]) {
                        goto nogood2;
                    }
                }
                return i;
            nogood2:;
            }
        } break;
    }
    return {};
}

optional<pair<size_t, size_t>> String::Find (const RegularExpression& regEx, size_t startAt) const
{
    Require (startAt <= size ());
    wstring tmp = As<wstring> ();
    Require (startAt < tmp.size ());
    tmp = tmp.substr (startAt);
    wsmatch res;
    regex_search (tmp, res, regEx.GetCompiled ());
    if (res.size () >= 1) {
        return pair<size_t, size_t>{startAt + res.position (), startAt + res.position () + res.length ()};
    }
    return {};
}

vector<size_t> String::FindEach (const String& string2SearchFor, CompareOptions co) const
{
    vector<size_t> result;
    for (optional<size_t> i = Find (string2SearchFor, 0, co); i; i = Find (string2SearchFor, *i, co)) {
        result.push_back (*i);
        *i += string2SearchFor.length (); // this cannot point past end of this string because we FOUND string2SearchFor
    }
    return result;
}

vector<pair<size_t, size_t>> String::FindEach (const RegularExpression& regEx) const
{
    vector<pair<size_t, size_t>> result;
    //@TODO - FIX - IF we get back zero length match
    wstring tmp{As<wstring> ()};
    wsmatch res;
    regex_search (tmp, res, regEx.GetCompiled ());
    size_t nMatches = res.size ();
    result.reserve (nMatches);
    for (size_t mi = 0; mi < nMatches; ++mi) {
        size_t matchLen = res.length (mi); // avoid populating with lots of empty matches - special case of empty search
        if (matchLen != 0) {
            result.push_back (pair<size_t, size_t>{res.position (mi), matchLen});
        }
    }
    return result;
}

vector<RegularExpressionMatch> String::FindEachMatch (const RegularExpression& regEx) const
{
    vector<RegularExpressionMatch> result;
    wstring                        tmp{As<wstring> ()};
    for (wsregex_iterator i = wsregex_iterator{tmp.begin (), tmp.end (), regEx.GetCompiled ()}; i != wsregex_iterator (); ++i) {
        wsmatch match{*i};
        Assert (match.size () != 0);
        size_t                       n = match.size ();
        Containers::Sequence<String> s;
        for (size_t j = 1; j < n; ++j) {
            s.Append (match.str (j));
        }
        result.push_back (RegularExpressionMatch{match.str (0), s});
    }
    return result;
}

vector<String> String::FindEachString (const RegularExpression& regEx) const
{
    vector<String> result;
    wstring        tmp{As<wstring> ()};
    for (wsregex_iterator i = wsregex_iterator{tmp.begin (), tmp.end (), regEx.GetCompiled ()}; i != wsregex_iterator (); ++i) {
        result.push_back (String{i->str ()});
    }
    return result;
}

optional<size_t> String::RFind (Character c) const noexcept
{
    //@todo: FIX HORRIBLE PERFORMANCE!!!
    _SafeReadRepAccessor accessor{this};
    const _IRep&         useRep = accessor._ConstGetRep ();
    size_t               length = useRep.size ();
    for (size_t i = length; i > 0; --i) {
        if (useRep.GetAt (i - 1) == c) {
            return i - 1;
        }
    }
    return nullopt;
}

optional<size_t> String::RFind (const String& subString) const
{
    //@todo: FIX HORRIBLE PERFORMANCE!!!
    /*
     * Do quickie implementation, and don't worry about efficiency...
     */
    size_t subStrLen = subString.size ();
    if (subStrLen == 0) {
        return ((size () == 0) ? optional<size_t>{} : size () - 1);
    }

    size_t limit = size () - subStrLen + 1;
    for (size_t i = limit; i > 0; --i) {
        if (SubString (i - 1, i - 1 + subStrLen) == subString) {
            return i - 1;
        }
    }
    return nullopt;
}

String String::Replace (size_t from, size_t to, const String& replacement) const
{
    Memory::StackBuffer<wchar_t> ignored;
    span<const wchar_t>          thisSpan = GetData (&ignored);
    Require (from <= to);
    Require (to <= this->size ());
    Assert (to < thisSpan.size ());
    StringBuilder sb{thisSpan.subspan (0, from)};
    sb.Append (replacement);
    sb.Append (thisSpan.subspan (to));
    Ensure (sb.str () == SubString (0, from) + replacement + SubString (to));
    return sb.str ();
}

bool String::StartsWith (const Character& c, CompareOptions co) const
{
    _SafeReadRepAccessor accessor{this};
    if (accessor._ConstGetRep ().size () == 0) {
        return false;
    }
    return Character::EqualsComparer{co}(accessor._ConstGetRep ().GetAt (0), c);
}

bool String::StartsWith (const String& subString, CompareOptions co) const
{
    if (subString.size () > size ()) {
        return false;
    }
#if qDebug
    bool referenceResult = ThreeWayComparer{co}(SubString (0, subString.size ()), subString) == 0;
#endif
    Memory::StackBuffer<Character> maybeIgnoreBuf1;
    Memory::StackBuffer<Character> maybeIgnoreBuf2;
    span<const Character>          subStrData = subString.GetData (&maybeIgnoreBuf1);
    span<const Character>          thisData   = GetData (&maybeIgnoreBuf2);
    bool                           result     = Character::Compare (thisData.subspan (0, subStrData.size ()), subStrData, co) == 0;
#if qDebug
    Ensure (result == referenceResult);
#endif
    return result;
}

bool String::EndsWith (const Character& c, CompareOptions co) const
{
    _SafeReadRepAccessor accessor{this};
    const _IRep&         useRep     = accessor._ConstGetRep ();
    size_t               thisStrLen = useRep.size ();
    if (thisStrLen == 0) {
        return false;
    }
    return Character::EqualsComparer{co}(useRep.GetAt (thisStrLen - 1), c);
}

bool String::EndsWith (const String& subString, CompareOptions co) const
{
    _SafeReadRepAccessor subStrAccessor{&subString};
    _SafeReadRepAccessor accessor{this};
    size_t               thisStrLen = accessor._ConstGetRep ().size ();
    size_t               subStrLen  = subString.size ();
    if (subStrLen > thisStrLen) {
        return false;
    }
#if qDebug
    bool referenceResult = String::EqualsComparer{co}(SubString (thisStrLen - subStrLen, thisStrLen), subString);
#endif
    Memory::StackBuffer<Character> maybeIgnoreBuf1;
    Memory::StackBuffer<Character> maybeIgnoreBuf2;
    span<const Character>          subStrData = subString.GetData (&maybeIgnoreBuf1);
    span<const Character>          thisData   = GetData (&maybeIgnoreBuf2);
    bool                           result     = Character::Compare (thisData.subspan (thisStrLen - subStrLen), subStrData, co) == 0;
#if qDebug
    Ensure (result == referenceResult);
#endif
    return result;
}

bool String::Matches (const RegularExpression& regEx) const
{
    wstring tmp{As<wstring> ()};
    return regex_match (tmp.begin (), tmp.end (), regEx.GetCompiled ());
}

bool String::Matches (const RegularExpression& regEx, Containers::Sequence<String>* matches) const
{
    RequireNotNull (matches);
    //tmphack
    wstring tmp{As<wstring> ()};
    wsmatch base_match;
    if (regex_match (tmp, base_match, regEx.GetCompiled ())) {
        matches->clear ();
        for (size_t i = 1; i < base_match.size (); ++i) {
            matches->Append (base_match[i].str ());
        }
        return true;
    }
    return false;
}

String String::ReplaceAll (const RegularExpression& regEx, const String& with) const
{
    return String{regex_replace (As<wstring> (), regEx.GetCompiled (), with.As<wstring> ())};
}

String String::ReplaceAll (const String& string2SearchFor, const String& with, CompareOptions co) const
{
    Require (not string2SearchFor.empty ());
    // simplistic quickie impl...
    String           result{*this};
    optional<size_t> i{0};
    while ((i = result.Find (string2SearchFor, *i, co))) {
        result = result.SubString (0, *i) + with + result.SubString (*i + string2SearchFor.length ());
        *i += with.length ();
    }
    return result;
}

String String::ReplaceAll (const function<bool (Character)>& replaceCharP, const String& with) const
{
    StringBuilder sb;
    for (Character i : *this) {
        if (replaceCharP (i)) {
            sb << with;
        }
        else {
            sb << i;
        }
    }
    return sb.str ();
}

String String::ReplaceAll (const Containers::Set<Character>& charSet, const String& with) const
{
    StringBuilder sb;
    for (Character i : *this) {
        if (charSet.Contains (i)) {
            sb << with;
        }
        else {
            sb << i;
        }
    }
    return sb.str ();
}

String String::NoramlizeTextToNL () const
{
    PeekSpanData                   pds = GetPeekSpanData<ASCII> ();
    Memory::StackBuffer<Character> maybeIgnoreBuf;
    span<const Character>          charSpan = GetData (pds, &maybeIgnoreBuf);
    StringBuilder                  sb;
    bool                           everChanged{false};
    for (auto ci = charSpan.begin (); ci != charSpan.end (); ++ci) {
        Character c = *ci;
        if (c == '\r') {
            // peek at next character - and if we have a CRLF sequence - then advance pointer
            // (so we skip next NL) and pretend this was an NL..
            if (ci + 1 != charSpan.end () and *(ci + 1) == '\n') {
                ++ci;
            }
            everChanged = true;
            c           = '\n';
        }
        sb << c;
    }
    if (everChanged) {
        return sb.str ();
    }
    else {
        return *this;
    }
}

Containers::Sequence<String> String::Tokenize (const function<bool (Character)>& isTokenSeperator, bool trim) const
{
    Containers::Sequence<String> r;
    bool                         inToken = false;
    StringBuilder                curToken;
    size_t                       len = size ();
    for (size_t i = 0; i != len; ++i) {
        Character c          = GetCharAt (i);
        bool      newInToken = not isTokenSeperator (c);
        if (inToken != newInToken) {
            if (inToken) {
                String s{curToken.str ()};
                if (trim) {
                    s = s.Trim ();
                }
                r += s;
                curToken.clear ();
                inToken = false;
            }
            else {
                inToken = true;
            }
        }
        if (inToken) {
            curToken << c;
        }
    }
    if (inToken) {
        String s{curToken.str ()};
        if (trim) {
            s = s.Trim ();
        }
        r += s;
    }
    return r;
}

Containers::Sequence<String> String::Tokenize (const Containers::Set<Character>& delimiters, bool trim) const
{
    /*
     *  @todo Inefficient impl, to encourage code saving. Do more efficently.
     */
    return Tokenize ([delimiters] (Character c) -> bool { return delimiters.Contains (c); }, trim);
}

String String::SubString_ (const _SafeReadRepAccessor& thisAccessor, size_t from, size_t to) const
{
    constexpr bool kWholeStringOptionization_ =
        false; // empirically, this costs about 1%. My WAG is that 1% cost not a good tradeoff cuz I dont think this gets triggered that often - LGP 2023-09-26
    Require (from <= to);
    Require (to <= this->size ());

    // Could do this more simply, but since this function is a bottleneck, handle each representation case separately
    if (from == to) [[unlikely]] {
        return mkEmpty_ ();
    }
    PeekSpanData psd = thisAccessor._ConstGetRep ().PeekData (nullopt);
    switch (psd.fInCP) {
        case PeekSpanData::eAscii: {
            if constexpr (kWholeStringOptionization_) {
                if (from == 0 and to == psd.fAscii.size ()) [[unlikely]] {
                    return *this; // unclear if this optimization is worthwhile
                }
            }
            return mk_nocheck_ (psd.fAscii.subspan (from, to - from)); // no check cuz we already know its all ASCII and nothing smaller
        }
        case PeekSpanData::eSingleByteLatin1: {
            if constexpr (kWholeStringOptionization_) {
                if (from == 0 and to == psd.fSingleByteLatin1.size ()) [[unlikely]] {
                    return *this; // unclear if this optimization is worthwhile
                }
            }
            return mk_ (psd.fSingleByteLatin1.subspan (from, to - from)); // note still needs to re-examine text, cuz subset maybe pure ascii (etc)
        }
        case PeekSpanData::eChar16: {
            if constexpr (kWholeStringOptionization_) {
                if (from == 0 and to == psd.fChar16.size ()) [[unlikely]] {
                    return *this; // unclear if this optimization is worthwhile
                }
            }
            return mk_ (psd.fChar16.subspan (from, to - from)); // note still needs to re-examine text, cuz subset maybe pure ascii (etc)
        }
        case PeekSpanData::eChar32: {
            if constexpr (kWholeStringOptionization_) {
                if (from == 0 and to == psd.fChar32.size ()) [[unlikely]] {
                    return *this; // unclear if this optimization is worthwhile
                }
            }
            return mk_ (psd.fChar32.subspan (from, to - from)); // note still needs to re-examine text, cuz subset maybe pure ascii (etc)
        }
        default:
            AssertNotReached ();
            return String{};
    }
}

String String::Repeat (unsigned int count) const
{
    switch (count) {
        case 0:
            return String{};
        case 1:
            return *this;
        case 2:
            return *this + *this;
        default: {
            StringBuilder result;
            for (unsigned int i = 0; i < count; ++i) {
                result << *this;
            }
            return result.str ();
        }
    }
}

String String::LTrim (bool (*shouldBeTrimmmed) (Character)) const
{
    RequireNotNull (shouldBeTrimmmed);
    _SafeReadRepAccessor accessor{this};
    size_t               length = accessor._ConstGetRep ().size ();
    for (size_t i = 0; i < length; ++i) {
        if (not(*shouldBeTrimmmed) (accessor._ConstGetRep ().GetAt (i))) {
            if (i == 0) {
                // no change in string
                return *this;
            }
            else {
                return SubString (i, length);
            }
        }
    }
    // all trimmed
    return String{};
}

String String::RTrim (bool (*shouldBeTrimmmed) (Character)) const
{
    RequireNotNull (shouldBeTrimmmed);
    _SafeReadRepAccessor accessor{this};
    ptrdiff_t            length         = accessor._ConstGetRep ().size ();
    ptrdiff_t            endOfFirstTrim = length;
    for (; endOfFirstTrim != 0; --endOfFirstTrim) {
        if ((*shouldBeTrimmmed) (accessor._ConstGetRep ().GetAt (endOfFirstTrim - 1))) {
            // keep going backwards
        }
        else {
            break;
        }
    }
    if (endOfFirstTrim == 0) {
        return String{}; // all trimmed
    }
    else if (endOfFirstTrim == length) {
        return *this; // nothing trimmed
    }
    else {
        return SubString (0, endOfFirstTrim);
    }
}

String String::Trim (bool (*shouldBeTrimmmed) (Character)) const
{
    RequireNotNull (shouldBeTrimmmed);
    /*
     * This could be implemented more efficient, but this is simpler for now..
     */
    return LTrim (shouldBeTrimmmed).RTrim (shouldBeTrimmmed);
}

String String::StripAll (bool (*removeCharIf) (Character)) const
{
    //@todo - fix - horribly impl..
    RequireNotNull (removeCharIf);

    // TODO: optimize special case where removeCharIf is always false
    //
    // Walk string and find first character we need to remove
    String result{*this};
    size_t n = result.size ();
    for (size_t i = 0; i < n; ++i) {
        Character c = result[i];
        if (removeCharIf (c)) {
            // on first removal, clone part of string done so far, and start appending
            String tmp = result.SubString (0, i);
            // Now keep iterating IN THIS LOOP appending characters and return at the end of this loop
            ++i;
            for (; i < n; ++i) {
                c = result[i];
                if (not removeCharIf (c)) {
                    tmp += c;
                }
            }
            return tmp;
        }
    }
    return *this; // if we NEVER get removeCharIf return false, just clone this
}

String String::Join (const Iterable<String>& list, const String& separator)
{
    StringBuilder result;
    for (const String& i : list) {
        result << i << separator;
    }
    if (result.empty ()) {
        return result.str ();
    }
    else {
        return result.str ().SubString (0, -static_cast<int> (separator.size ()));
    }
}

String String::ToLowerCase () const
{
    StringBuilder        result;
    bool                 changed{false}; // if no change, no need to allocate new object
    _SafeReadRepAccessor accessor{this};
    PeekSpanData         psd = accessor._ConstGetRep ().PeekData (nullopt);
    if (psd.fInCP == PeekSpanData::eAscii) [[likely]] {
        // optimization but other case would work no matter what
        for (auto c : psd.fAscii) {
            if (isupper (c)) {
                changed = true;
                result.push_back (tolower (c));
            }
            else {
                result.push_back (c);
            }
        }
    }
    else {
        Memory::StackBuffer<Character> maybeIgnoreBuf1;
        for (Character c : GetData (psd, &maybeIgnoreBuf1)) {
            if (c.IsUpperCase ()) {
                changed = true;
                result.push_back (c.ToLowerCase ());
            }
            else {
                result.push_back (c);
            }
        }
    }
    if (changed) {
        return result.str ();
    }
    else {
        return *this;
    }
}

String String::ToUpperCase () const
{
    StringBuilder        result;
    bool                 changed{false}; // if no change, no need to allocate new object
    _SafeReadRepAccessor accessor{this};
    PeekSpanData         psd = accessor._ConstGetRep ().PeekData (nullopt);
    if (psd.fInCP == PeekSpanData::eAscii) [[likely]] {
        // optimization but other case would work no matter what
        for (auto c : psd.fAscii) {
            if (islower (c)) {
                changed = true;
                result.push_back (toupper (c));
            }
            else {
                result.push_back (c);
            }
        }
    }
    else {
        Memory::StackBuffer<Character> maybeIgnoreBuf1;
        for (Character c : GetData (psd, &maybeIgnoreBuf1)) {
            if (c.IsLowerCase ()) {
                changed = true;
                result.push_back (c.ToUpperCase ());
            }
            else {
                result.push_back (c);
            }
        }
    }
    if (changed) {
        return result.str ();
    }
    else {
        return *this;
    }
}

bool String::IsWhitespace () const
{
    // It is all whitespace if the first non-whitespace character is 'EOF'
    return not Find ([] (Character c) -> bool { return not c.IsWhitespace (); });
}

String String::LimitLength (size_t maxLen, StringShorteningPreference keepPref, const String& ellipsis) const
{
    // @todo Consider making this the 'REFERENCE' impl, and doing a specific one with a specific StringBuilder, and doing
    // the trim/split directly, if I see this show up in a profile, for performance sake --LGP 2023-12-11
    if (length () < maxLen) [[likely]] {
        return *this; // frequent optimization
    }
    String operateOn = [&] () {
        switch (keepPref) {
            case StringShorteningPreference::ePreferKeepLeft:
                return LTrim ();
            case StringShorteningPreference::ePreferKeepRight:
                return RTrim ();
            case StringShorteningPreference::ePreferKeepMid:
                return Trim (); // not sure we need to trim - but probably best
            default:
                RequireNotReached ();
                return *this;
        }
    }();
    if (operateOn.length () <= maxLen) {
        return operateOn;
    }
    size_t useLen = [&] () {
        size_t useLen           = maxLen;
        size_t ellipsisTotalLen = ellipsis.length ();
        if (keepPref == StringShorteningPreference::ePreferKeepMid) {
            ellipsisTotalLen *= 2;
        }
        if (useLen > ellipsisTotalLen) {
            useLen -= ellipsisTotalLen;
        }
        else {
            useLen = 0;
        }
        return useLen;
    }();
    switch (keepPref) {
        case StringShorteningPreference::ePreferKeepLeft:
            return operateOn.substr (0, useLen) + ellipsis;
        case StringShorteningPreference::ePreferKeepRight:
            return ellipsis + operateOn.substr (operateOn.length () - useLen);
        case StringShorteningPreference::ePreferKeepMid:
            return ellipsis + operateOn.substr (operateOn.length () / 2 - useLen / 2, useLen) + ellipsis;
        default:
            RequireNotReached ();
            return *this;
    }
}

string String::AsNarrowString (const locale& l) const
{
    // Note: this could use CodeCvt, but directly using std::codecvt in this case pretty simple, and
    // more efficient this way --LGP 2023-02-14

    // See http://en.cppreference.com/w/cpp/locale/codecvt/~codecvt
    using Destructible_codecvt_byname = deletable_facet_<codecvt_byname<wchar_t, char, mbstate_t>>;
    Destructible_codecvt_byname cvt{l.name ()};

    Memory::StackBuffer<wchar_t> maybeIgnoreBuf1;
    span<const wchar_t>          thisData = GetData (&maybeIgnoreBuf1);
    // http://en.cppreference.com/w/cpp/locale/codecvt/out
    mbstate_t                 mbstate{};
    const wchar_t*            from_next;
    char*                     to_next;
    Memory::StackBuffer<char> into{Memory::eUninitialized, thisData.size () * 5}; // not sure what size is always big enuf
    codecvt_base::result      result =
        cvt.out (mbstate, thisData.data (), thisData.data () + thisData.size (), from_next, into.data (), into.end (), to_next);
    if (result != codecvt_base::ok) [[unlikely]] {
        static const auto kException_ = Execution::RuntimeErrorException{"Error converting locale multibyte string to UNICODE"sv};
        Execution::Throw (kException_);
    }
    return string{into.data (), to_next};
}

string String::AsNarrowString (const locale& l, AllowMissingCharacterErrorsFlag) const
{
    // Note: this could use CodeCvt, but directly using std::codecvt in this case pretty simple, and
    // more efficient this way --LGP 2023-02-14

    // See http://en.cppreference.com/w/cpp/locale/codecvt/~codecvt
    using Destructible_codecvt_byname = deletable_facet_<codecvt_byname<wchar_t, char, mbstate_t>>;
    Destructible_codecvt_byname cvt{l.name ()};

    Memory::StackBuffer<wchar_t> maybeIgnoreBuf1;
    span<const wchar_t>          thisData = GetData (&maybeIgnoreBuf1);
    // http://en.cppreference.com/w/cpp/locale/codecvt/out
    mbstate_t                 mbstate{};
    Memory::StackBuffer<char> into{Memory::eUninitialized, thisData.size () * 5}; // not sure what size is always big enuf
    const wchar_t*            readFrom  = thisData.data ();
    char*                     intoIndex = into.data ();
Again:
    const wchar_t* from_next{nullptr};
    char*          to_next{nullptr};
    codecvt_base::result result = cvt.out (mbstate, readFrom, thisData.data () + thisData.size (), from_next, intoIndex, into.end (), to_next);
    if (result != codecvt_base::ok) [[unlikely]] {
        if (from_next != thisData.data () + thisData.size ()) {
            readFrom  = from_next + 1; // unclear how much to skip (due to surrogates) - but likely this is a good guess
            *to_next  = '?';           // write 'bad' character
            intoIndex = to_next + 1;
            goto Again;
        }
    }
    return string{into.data (), to_next};
}

void String::erase (size_t from)
{
    *this = RemoveAt (from, size ());
}

void String::erase (size_t from, size_t count)
{
    // https://stroika.atlassian.net/browse/STK-445
    // @todo - NOT ENVELOPE THREADSAFE
    // MUST ACQUIRE ACCESSOR HERE - not just that RemoteAt threadsafe - but must SYNC at this point - need AssureExternallySycnonized stuff here!!!
    //
    // TODO: Double check STL definition - but I think they allow for count to be 'too much' - and silently trim to end...
    size_t max2Erase = static_cast<size_t> (max (static_cast<ptrdiff_t> (0), static_cast<ptrdiff_t> (size ()) - static_cast<ptrdiff_t> (from)));
    *this = RemoveAt (from, from + min (count, max2Erase));
}

const wchar_t* String::c_str () const noexcept
{
    // UNSAFE - DEPRECATED  - lose before v3 actually released -- LGP 2023-06-28
    return const_cast<String*> (this)->c_str ();
}
const wchar_t* String::c_str ()
{
    // Rarely used mechanism, of replacing the underlying rep, for the iterable, as needed
    _SafeReadRepAccessor accessor{this};
    const wchar_t*       result = accessor._ConstGetRep ().c_str_peek ();
    if (result == nullptr) {
        _fRep  = Memory::MakeSharedPtr<StringWithCStr_::Rep> (accessor._ConstGetRepSharedPtr ());
        result = _SafeReadRepAccessor{this}._ConstGetRep ().c_str_peek ();
        AssertNotNull (result);
    }
    EnsureNotNull (result);
    Ensure (result[size ()] == '\0' or (::wcslen (result) > size () and sizeof (wchar_t) == 2)); // if there are surrogates, wcslen () might be larger than size
    return result;
}

[[noreturn]] void String::ThrowInvalidAsciiException_ ()
{
    static const auto kException_ = Execution::RuntimeErrorException{"Error converting non-ascii text to string"sv};
    Execution::Throw (kException_);
}

/*
 ********************************************************************************
 ****************************** StringCombiner **********************************
 ********************************************************************************
 */
String StringCombiner::operator() (const String& lhs, const String& rhs, bool isLast) const
{
    StringBuilder sb{lhs};
    if (isLast and fSpecialSeparatorForLastPair) [[unlikely]] {
        sb << *fSpecialSeparatorForLastPair;
    }
    else {
        sb << fSeparator;
    }
    sb << rhs;
    return sb.str ();
}

/*
 ********************************************************************************
 ******************* Iterable<Characters::String>::Join *************************
 ********************************************************************************
 */
namespace Stroika::Foundation::Traversal {
    // specialized as performance optimization
    template <>
    Characters::String Iterable<Characters::String>::Join (const Characters::String& separator, const optional<Characters::String>& finalSeparator) const
    {
        using namespace Characters;
        String referenceResult = this->Join (Iterable<String>::kDefaultToStringConverter<String>,
                                             Characters::StringCombiner{.fSeparator = separator, .fSpecialSeparatorForLastPair = finalSeparator});

        return referenceResult;
    }
}

/*
 ********************************************************************************
 ********************************** operator<< **********************************
 ********************************************************************************
 */
wostream& Characters::operator<< (wostream& out, const String& s)
{
    Memory::StackBuffer<wchar_t> maybeIgnoreBuf1;
    span<const wchar_t>          sData = s.GetData (&maybeIgnoreBuf1);
    out.write (sData.data (), sData.size ());
    return out;
}

/*
 ********************************************************************************
 *********** hash<Stroika::Foundation::Characters::String> **********************
 ********************************************************************************
 */
size_t std::hash<String>::operator() (const String& arg) const
{
    using namespace Cryptography::Digest;
    using DIGESTER = Digester<Algorithm::SuperFastHash>; // pick arbitrarily which algorithm to use for now -- err on the side of quick and dirty
    static constexpr DIGESTER kDigester_{};
    // Note this could easily use char8_t, wchar_t, char32_t, or whatever. Choose char8_t on the theorey that
    // this will most often avoid a copy, and making the most often case faster is probably a win. Also, even close, it
    // will have less 'empty space' and be more compact, so will digest faster.
    Memory::StackBuffer<char8_t> maybeIgnoreBuf1;
    span<const char8_t>          s = arg.GetData (&maybeIgnoreBuf1);
    if (s.empty ()) {
        static const size_t kZeroDigest_ = kDigester_ (nullptr, nullptr);
        return kZeroDigest_;
    }
    else {
        return kDigester_ (as_bytes (s));
    }
}

/*
 ********************************************************************************
 ******************** DataExchange::DefaultSerializer<String> *******************
 ********************************************************************************
 */
Memory::BLOB DataExchange::DefaultSerializer<String>::operator() (const String& arg) const
{
    //
    // Could have used char8_t, char16_t, or char32_t here quite plausibly. Chose char8_t for several reasons:
    //      >   Nearly always smallest representation (assuming most data is ascii)
    //      >   It is cross-platform/portable - not byte order dependent (NOT a promise going forward, so maybe
    //          not a good thing - but a thing)
    //      >   Since we expect most data reps to be ascii, this will involve the least copying, most likely, in
    //          the GetData call
    //
    Memory::StackBuffer<char8_t> maybeIgnoreBuf1;
    return Memory::BLOB{as_bytes (arg.GetData (&maybeIgnoreBuf1))};
}
