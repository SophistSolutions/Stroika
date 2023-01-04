/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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
#include "../Memory/StackBuffer.h"

#include "RegularExpression.h"
#include "SDKString.h"
#include "StringBuilder.h"

#include "String.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

using Memory::StackBuffer;
using Traversal::Iterator;

// Experimental block allocation scheme for strings. We COULD enahce this to have 2 block sizes - say 16 and 32 characters?
// But experiment with this a bit first, and see how it goes...
//      -- LGP 2011-12-04
#ifndef qString_Private_BufferedStringRep_UseBlockAllocatedForSmallBufStrings
#define qString_Private_BufferedStringRep_UseBlockAllocatedForSmallBufStrings qAllowBlockAllocation
#endif

namespace {

    constexpr bool kUseBlockAllocatedForSmallBufStrings_ = qString_Private_BufferedStringRep_UseBlockAllocatedForSmallBufStrings;

    /**
     *  Helper for sharing implementation code on string reps
     */
    struct StringRepHelper_ : String {
        struct Rep : public _IRep {
        private:
            using inherited = String::_IRep;

        protected:
            Rep (const pair<const wchar_t*, const wchar_t*>& s)
                : inherited{s}
            {
            }
            Rep (const wchar_t* start, const wchar_t* end)
                : inherited{start, end}
            {
            }
        };
    };


    /**
     *  This is a utility class to implement most of the basic String::_IRep functionality.
     *  This implements functions that change the string, but don't GROW it,
     *  since we don't know in general we can (thats left to subtypes).
     *
     *  \note   This class always assure nul-terminated, and so 'capacity' always at least one greater than length.
     *
     *  @todo Explain queer wrapper class cuz protected
     */
    struct BufferedString_ : StringRepHelper_ {
        struct Rep : public StringRepHelper_::Rep, public Memory::UseBlockAllocationIfAppropriate<Rep> {
        private:
            using inherited = StringRepHelper_::Rep;

        private:
            /*
             * magic 'automatic round-capacity-up-to' these (in wchar_ts) - and use block allocation for them...
             * 
             * getconf LEVEL1_DCACHE_LINESIZE; getconf LEVEL2_CACHE_LINESIZE
             * typically returns 64,64 so 16*2 is 32, and 16*4 (unix) is 64. So the blocks allocated will generally fit
             * in a cache line.
             * 
             * Maybe the 'roundup' numbers should differ between UNIX / Windows (2/4 byte wchar_t)?
             */
            static constexpr size_t kNElts1_ = 16;
            static constexpr size_t kNElts2_ = 32;
            static constexpr size_t kNElts3_ = 64;

        private:
            template <size_t SZ>
            struct BufferedStringRepBlock_ {
                static constexpr size_t kNElts = SZ;
                wchar_t                 data[kNElts];
            };

        public:
            Rep (span<const wchar_t> t1)
                : Rep{mkBuf_ (t1)}
            {
            }

        protected:
            Rep ()           = delete;
            Rep (const Rep&) = delete;

        public:
            nonvirtual Rep& operator= (const Rep&) = delete;

        protected:
            /**
             *  The argument wchar_t* strings MAY or MAY NOT be nul-terminated
             */
            Rep (const tuple<const wchar_t*, const wchar_t*, size_t>& strAndCapacity)
                : inherited{make_pair (get<0> (strAndCapacity), get<1> (strAndCapacity))}
                , fCapacity_{get<2> (strAndCapacity)}
            {
            }

        public:
            ~Rep ()
            {
                AssertNotNull (_fStart);
                Assert (fCapacity_ == AdjustCapacity_ (this->_GetLength () + 1)); // see mkBuf_ (size_t length) - and possible optimize to not store fCapacity
                if constexpr (kUseBlockAllocatedForSmallBufStrings_) {
                    Assert (fCapacity_ >= kNElts1_);
                    switch (fCapacity_) {
                        case kNElts1_: {
                            Memory::BlockAllocator<BufferedStringRepBlock_<kNElts1_>>{}.deallocate (reinterpret_cast<BufferedStringRepBlock_<kNElts1_>*> (const_cast<wchar_t*> (_fStart)), 1);
                        } break;
                        case kNElts2_: {
                            Memory::BlockAllocator<BufferedStringRepBlock_<kNElts2_>>{}.deallocate (reinterpret_cast<BufferedStringRepBlock_<kNElts2_>*> (const_cast<wchar_t*> (_fStart)), 1);
                        } break;
                        case kNElts3_: {
                            Memory::BlockAllocator<BufferedStringRepBlock_<kNElts3_>>{}.deallocate (reinterpret_cast<BufferedStringRepBlock_<kNElts3_>*> (const_cast<wchar_t*> (_fStart)), 1);
                        } break;
                        default: {
                            delete[] _fStart;
                        } break;
                    }
                }
                else {
                    delete[] _fStart;
                }
            }

        private:
            static pair<wchar_t*, wchar_t*> mkBuf_ (size_t length)
            {
                size_t   capacity = AdjustCapacity_ (length + 1); // add one for '\0'
                wchar_t* newBuf   = nullptr;
                if constexpr (kUseBlockAllocatedForSmallBufStrings_) {
                    Assert (capacity >= kNElts1_ or capacity >= kNElts2_ or capacity >= kNElts3_);
                }
                DISABLE_COMPILER_MSC_WARNING_START (4065)
                if constexpr (kUseBlockAllocatedForSmallBufStrings_) {
                    switch (capacity) {
                        case kNElts1_: {
                            static_assert (sizeof (BufferedStringRepBlock_<kNElts1_>) == sizeof (wchar_t) * kNElts1_, "sizes should match");
                            newBuf = reinterpret_cast<wchar_t*> (Memory::BlockAllocator<BufferedStringRepBlock_<kNElts1_>>{}.allocate (1));
                        } break;
                        case kNElts2_: {
                            static_assert (sizeof (BufferedStringRepBlock_<kNElts2_>) == sizeof (wchar_t) * kNElts2_, "sizes should match");
                            newBuf = reinterpret_cast<wchar_t*> (Memory::BlockAllocator<BufferedStringRepBlock_<kNElts2_>>{}.allocate (1));
                        } break;
                        case kNElts3_: {
                            static_assert (sizeof (BufferedStringRepBlock_<kNElts3_>) == sizeof (wchar_t) * kNElts3_, "sizes should match");
                            newBuf = reinterpret_cast<wchar_t*> (Memory::BlockAllocator<BufferedStringRepBlock_<kNElts3_>>{}.allocate (1));
                        } break;
                        default: {
                            newBuf = new wchar_t[capacity];
                        } break;
                    }
                }
                else {
                    newBuf = new wchar_t[capacity];
                }
                DISABLE_COMPILER_MSC_WARNING_END (4065)
                return make_pair (newBuf, newBuf + capacity);
            }
            static tuple<const wchar_t*, const wchar_t*, size_t> mkBuf_ (span<const wchar_t> t1)
            {
                size_t                   len    = t1.size ();
                pair<wchar_t*, wchar_t*> result = mkBuf_ (len);
                if (len != 0) {
                    (void)::memcpy (result.first, t1.data (), len * sizeof (wchar_t));
                }
                result.first[len] = '\0';
                return make_tuple (result.first, result.first + len, result.second - result.first);
            }

        public:
            virtual _IterableRepSharedPtr Clone () const override
            {
                AssertNotReached (); // Since String reps now immutable, this should never be called
                return nullptr;
            }

        public:
            virtual const wchar_t* c_str_peek () const noexcept override
            {
                [[maybe_unused]] size_t len = _GetLength ();
                Ensure (_fStart[len] == '\0');
                return _fStart;
            }

        public:
            virtual size_t size () const override
            {
                return _GetLength ();
            }

        private:
            static size_t AdjustCapacity_ (size_t initialCapacity)
            {
                if constexpr (kUseBlockAllocatedForSmallBufStrings_) {
                    static_assert (kNElts1_ <= kNElts2_ and kNElts2_ <= kNElts3_);
                    if (initialCapacity <= kNElts1_) {
                        return kNElts1_;
                    }
                    else if (initialCapacity <= kNElts2_) {
                        return kNElts2_;
                    }
                    else if (initialCapacity <= kNElts3_) {
                        return kNElts3_;
                    }
                }
                size_t result = Containers::Support::ReserveTweaks::GetScaledUpCapacity (initialCapacity, sizeof (Character));
                Ensure (initialCapacity <= result);
                return result;
            }

        private:
            // includes INCLUDES nul/EOS char
            // @todo add constexpr config var here, and play with it: COULD have recomputed this from the length in the DTOR (only place its used) - but that recompute could be moderately expensive, so just
            // save it (perhaps reconsider as CPU faster and memory bandwidth more limiting)
            size_t fCapacity_;
        };
    };
}

namespace {
    /**
     *  For static full app lifetime string constants...
     */
    struct StringConstant_ : public StringRepHelper_ {
        using inherited = String;

        class Rep : public StringRepHelper_::Rep, public Memory::UseBlockAllocationIfAppropriate<Rep> {
        private:
            using inherited = StringRepHelper_::Rep;

        public:
            Rep (span<const wchar_t> s)
                : inherited{s.data (), s.data () + s.size ()} // don't copy memory - but copy raw pointers! So they MUST BE (externally promised) 'externally owned for the application lifetime and constant' - like c++ string constants
            {
                const wchar_t* start = s.data ();
                const wchar_t* end   = start + s.size ();
                // NO - we allow embedded nuls, but require NUL-termination - so this is wrong - Require (start + ::wcslen (start) == end);
                Require (*end == '\0' and start + ::wcslen (start) <= end);
            }
            virtual _IterableRepSharedPtr Clone () const override
            {
                AssertNotReached (); // Since String reps now immutable, this should never be called
                return nullptr;
            }
            virtual const wchar_t* c_str_peek () const noexcept override
            {
                // This class ALWAYS constructed with String_ExternalMemoryOwnership_ApplicationLifetime and ALWAYS with NUL-terminated string
                // NO - we allow embedded nuls, but require NUL-termination - so this is wrong - Assert (_fStart + ::wcslen (_fStart) == _fEnd);
                Assert (*_fEnd == '\0' and _fStart + ::wcslen (_fStart) <= _fEnd);
                return _fStart;
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
        ~deletable_facet_ () {}
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
 ****************************** String::_IRep ***********************************
 ********************************************************************************
 */
Traversal::Iterator<Character> String::_IRep::MakeIterator () const
{
    struct MyIterRep_ final : Iterator<Character>::IRep, public Memory::UseBlockAllocationIfAppropriate<MyIterRep_> {
        _SharedPtrIRep fStr; // effectively RO, since if anyone modifies, our copy will remain unchanged
        size_t         fCurIdx;
        MyIterRep_ (const _SharedPtrIRep& r, size_t idx = 0)
            : fStr{r}
            , fCurIdx{idx}
        {
            Require (fCurIdx <= fStr->_GetLength ());
        }
        virtual Iterator<Character>::RepSmartPtr Clone () const override
        {
            return Iterator<Character>::MakeSmartPtr<MyIterRep_> (fStr, fCurIdx);
        }
        virtual void More (optional<Character>* result, bool advance) override
        {
            RequireNotNull (result);
            if (advance) [[likely]] {
                Require (fCurIdx <= fStr->_GetLength ());
                ++fCurIdx;
            }
            if (fCurIdx < fStr->_GetLength ()) [[likely]] {
                *result = fStr->GetAt (fCurIdx);
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
            AssertNotNull (rrhs);
            Require (fStr == rrhs->fStr); // from same string object
            return fCurIdx == rrhs->fCurIdx;
        }
    };
    _SharedPtrIRep sharedContainerRep = const_cast<String::_IRep*> (this)->shared_from_this ();
    return Iterator<Character>{Iterator<Character>::MakeSmartPtr<MyIterRep_> (sharedContainerRep)};
}

size_t String::_IRep::size () const
{
    Assert (_fStart <= _fEnd);
    return _fEnd - _fStart;
}

bool String::_IRep::empty () const
{
    Assert (_fStart <= _fEnd);
    return _fEnd == _fStart;
}

void String::_IRep::Apply (const function<void (Configuration::ArgByValueType<value_type> item)>& doToElement) const
{
    _Apply (doToElement);
}

auto String::_IRep::Find (const function<bool (Configuration::ArgByValueType<value_type> item)>& that) const -> Traversal::Iterator<value_type>
{
    return _Find (that);
}

/*
 ********************************************************************************
 ************************************* String ***********************************
 ********************************************************************************
 */
static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");

String::String (const basic_string_view<wchar_t>& str)
    : String{MakeSmartPtr<StringConstant_::Rep> (span{str.data (), str.size ()})}
{
    Require (str.data ()[str.length ()] == 0); // Because Stroika strings provide the guarantee that they can be converted to c_str () - we require the input memory
                                               // for these const strings are also nul-terminated.
                                               // DONT try to CORRECT this if found wrong, because whenever you use "stuff"sv - the string literal will always
                                               // be nul-terminated.
                                               // -- LGP 2019-01-29
}

String String::FromStringConstant (const span<const wchar_t> s)
{
    Require (*(s.data () + s.size ()) == '\0'); // crazy weird requirement, but done cuz L"x"sv already does NUL-terminate and we can
                                                // take advantage of that fact - re-using the NUL-terminator for our own c_str() implementation
    return String{MakeSmartPtr<StringConstant_::Rep> (span{s.data (), s.size ()})};
}

String String::FromNarrowString (span<const char> s, const locale& l)
{
    // See http://en.cppreference.com/w/cpp/locale/codecvt/~codecvt
    using Destructible_codecvt_byname = deletable_facet_<codecvt_byname<wchar_t, char, mbstate_t>>;
    Destructible_codecvt_byname cvt{l.name ()};

    // http://en.cppreference.com/w/cpp/locale/codecvt/in
    mbstate_t            mbstate{};
    size_t               externalSize = s.size ();
    wstring              resultWStr (externalSize, '\0');
    const char*          from_next;
    wchar_t*             to_next;
    codecvt_base::result result = cvt.in (mbstate, s.data (), s.data () + s.size (), from_next, &resultWStr[0], &resultWStr[resultWStr.size ()], to_next);
    if (result != codecvt_base::ok) [[unlikely]] {
        static const auto kException_ = Execution::RuntimeErrorException{L"Error converting locale multibyte string to UNICODE"sv};
        Execution::Throw (kException_);
    }
    resultWStr.resize (to_next - &resultWStr[0]);
    return resultWStr;
}

String::_SharedPtrIRep String::mkEmpty_ ()
{
    static const _SharedPtrIRep s_ = MakeSmartPtr<BufferedString_::Rep> (span<const wchar_t>{});
    return s_;
}

template <>
auto String::mk_ (span<const wchar_t> s) -> _SharedPtrIRep
{
    return MakeSmartPtr<BufferedString_::Rep> (s);
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

String String::InsertAt (const Character* from, const Character* to, size_t at) const
{
    Require (at >= 0);
    Require (at <= size ());
    Require (from <= to);
    Require (from != nullptr or from == to);
    Require (to != nullptr or from == to);
    if (from == to) {
        return *this;
    }
    Memory::StackBuffer<Character> ignored1;
    span<const Character>          thisStrData = GetData (&ignored1);
    StringBuilder                  sb{thisStrData.subspan (0, at)};
    sb.Append (span{from, to});
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
    else if (from == 0) {
        return SubString (to);
    }
    _SafeReadRepAccessor accessor{this};
    size_t               length = accessor._ConstGetRep ()._GetLength ();
    if (to == length) {
        return SubString (0, from);
    }
    else {
        pair<const Character*, const Character*> d = accessor._ConstGetRep ().GetData ();
        const wchar_t*                           p = reinterpret_cast<const wchar_t*> (d.first);
        return String{mk_ (span{p, from}, span{p + to, p + length})};
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

optional<size_t> String::Find (Character c, size_t startAt, CompareOptions co) const
{
    PeekSpanData pds = GetPeekSpanData<char> ();
    // OPTIMIZED PATHS: Common case and should be fast
    if (pds.fInCP == PeekSpanData::StorageCodePointType::eAscii) {
        if (c.IsASCII ()) {
            if (co == CompareOptions::eWithCase) {
                span<const char> examineSpan = pds.fAscii.subspan (startAt);
                if (auto i = std::find (examineSpan.begin (), examineSpan.end (), c.GetAsciiCode ()); i != examineSpan.end ()) {
                    return i - examineSpan.begin () + startAt;
                }
            }
        }
        return nullopt; // not found, possibly cuz not ascii
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
    Require (startAt <= accessor._ConstGetRep ()._GetLength ());

    size_t subStrLen = subString.size ();
    if (subStrLen == 0) {
        return (accessor._ConstGetRep ()._GetLength () == 0) ? optional<size_t>{} : 0;
    }
    if (accessor._ConstGetRep ()._GetLength () < subStrLen) {
        return {}; // important test cuz size_t is unsigned
    }

    size_t limit = accessor._ConstGetRep ()._GetLength () - subStrLen;
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

#if 0
vector<String>  String::Find (const String& string2SearchFor, CompareOptions co) const
{
    AssertNotReached ();
    vector<String>  result;
    wstring tmp     =   As<wstring> ();
    wregex  regExp  =   wregex (string2SearchFor.As<wstring> ());
    wsmatch res;
    regex_search (tmp, res, regExp);
    result.reserve (res.size ());
    for (auto i = res.begin (); i != res.end (); ++i) {
        result.push_back (String{*i});
    }
    return result;
}
#endif
optional<size_t> String::RFind (Character c) const noexcept
{
    //@todo: FIX HORRIBLE PERFORMANCE!!!
    _SafeReadRepAccessor accessor{this};
    const _IRep&         useRep = accessor._ConstGetRep ();
    size_t               length = useRep._GetLength ();
    for (size_t i = length; i > 0; --i) {
        if (useRep.GetAt (i - 1) == c) {
            return i - 1;
        }
    }
    return {};
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
    return {};
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
    if (accessor._ConstGetRep ()._GetLength () == 0) {
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
    Assert (result == referenceResult);
    return result;
}

bool String::EndsWith (const Character& c, CompareOptions co) const
{
    _SafeReadRepAccessor accessor{this};
    const _IRep&         useRep     = accessor._ConstGetRep ();
    size_t               thisStrLen = useRep._GetLength ();
    if (thisStrLen == 0) {
        return false;
    }
    return Character::EqualsComparer{co}(useRep.GetAt (thisStrLen - 1), c);
}

bool String::EndsWith (const String& subString, CompareOptions co) const
{
    _SafeReadRepAccessor subStrAccessor{&subString};
    _SafeReadRepAccessor accessor{this};
    size_t               thisStrLen = accessor._ConstGetRep ()._GetLength ();
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
    Assert (result == referenceResult);
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
            sb += with;
        }
        else {
            sb += i;
        }
    }
    return sb.str ();
}

String String::ReplaceAll (const Containers::Set<Character>& charSet, const String& with) const
{
    StringBuilder sb;
    for (Character i : *this) {
        if (charSet.Contains (i)) {
            sb += with;
        }
        else {
            sb += i;
        }
    }
    return sb.str ();
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
            curToken += c;
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
    return Tokenize (
        [delimiters] (Character c) -> bool {
            return delimiters.Contains (c);
        },
        trim);
}

String String::SubString_ (const _SafeReadRepAccessor& thisAccessor, size_t thisLen, size_t from, size_t to) const
{
    Require (from <= to);
    Require (to <= thisLen);
    Require (thisLen == this->size ());
    const wchar_t* start = reinterpret_cast<const wchar_t*> (thisAccessor._ConstGetRep ()._Peek ()) + from;
    size_t         len   = to - from;
    const wchar_t* end   = start + len;
    Assert (start <= end);
    // Slightly faster to re-use pre-built empty string
    if (start == end) {
        return mkEmpty_ ();
    }
    if (len == thisLen) {
        Assert (from == 0); // because we require from/to subrange of thisLen, so if equal, must be full range
        return *this;
    }
    return mk_ (span{start, end});
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
                result += *this;
            }
            return result.str ();
        }
    }
}

String String::LTrim (bool (*shouldBeTrimmmed) (Character)) const
{
    RequireNotNull (shouldBeTrimmmed);
    _SafeReadRepAccessor accessor{this};
    size_t               length = accessor._ConstGetRep ()._GetLength ();
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
    ptrdiff_t            length         = accessor._ConstGetRep ()._GetLength ();
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
    for (String i : list) {
        result += i;
        result += separator;
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
    _SafeReadRepAccessor accessor{this};
    size_t               n         = accessor._ConstGetRep ()._GetLength ();
    bool                 anyChange = false;
    for (size_t i = 0; i < n; ++i) {
        Character c = accessor._ConstGetRep ().GetAt (i);
        if (c.IsUpperCase ()) {
            anyChange = true;
            result.Append (c.ToLowerCase ());
        }
        else {
            result.Append (c);
        }
    }
    return anyChange ? result.str () : *this;
}

String String::ToUpperCase () const
{
    StringBuilder        result;
    _SafeReadRepAccessor accessor{this};
    size_t               n         = accessor._ConstGetRep ()._GetLength ();
    bool                 anyChange = false;
    for (size_t i = 0; i < n; ++i) {
        Character c = accessor._ConstGetRep ().GetAt (i);
        if (c.IsLowerCase ()) {
            anyChange = true;
            result.Append (c.ToUpperCase ());
        }
        else {
            result.Append (c);
        }
    }
    return anyChange ? result.str () : *this;
}

bool String::IsWhitespace () const
{
    // It is all whitespace if the first non-whatspace character is 'EOF'
    return not Find ([] (Character c) -> bool { return not c.IsWhitespace (); });
}

String String::LimitLength (size_t maxLen, bool keepLeft) const
{
#if qCompiler_vswprintf_on_elispisStr_Buggy
    static const String kELIPSIS_{L"..."sv};
#else
    static const String kELIPSIS_{L"\u2026"sv}; // OR L"..."
#endif
    return LimitLength (maxLen, keepLeft, kELIPSIS_);
}

String String::LimitLength (size_t maxLen, bool keepLeft, const String& ellipsis) const
{
    if (length () < maxLen) {
        return *this; // frequent optimization
    }
    String tmp = keepLeft ? RTrim () : LTrim ();
    if (tmp.length () <= maxLen) {
        return tmp;
    }
    size_t useLen = maxLen;
    if (useLen > ellipsis.length ()) {
        useLen -= ellipsis.length ();
    }
    else {
        useLen = 0;
    }
    if (keepLeft) {
        return tmp.substr (0, useLen) + ellipsis;
    }
    else {
        return ellipsis + tmp.substr (tmp.length () - useLen);
    }
}

void String::AsNarrowString (const locale& l, string* into) const
{
    // See http://en.cppreference.com/w/cpp/locale/codecvt/~codecvt
    using Destructible_codecvt_byname = deletable_facet_<codecvt_byname<wchar_t, char, mbstate_t>>;
    Destructible_codecvt_byname cvt{l.name ()};
    wstring                     wstr = As<wstring> ();
    // http://en.cppreference.com/w/cpp/locale/codecvt/out
    mbstate_t mbstate{};
    into->resize (wstr.size () * cvt.max_length (), '\0');
    const wchar_t*       from_next;
    char*                to_next;
    codecvt_base::result result = cvt.out (mbstate, &wstr[0], &wstr[wstr.size ()], from_next, &(*into)[0], &(*into)[into->size ()], to_next);
    if (result != codecvt_base::ok) [[unlikely]] {
        static const auto kException_ = Execution::RuntimeErrorException{L"Error converting locale multibyte string to UNICODE"sv};
        Execution::Throw (kException_);
    }
    into->resize (to_next - &(*into)[0]);
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
    *this            = RemoveAt (from, from + min (count, max2Erase));
}

void String::ThrowInvalidAsciiException_ ()
{
    static const auto kException_ = Execution::RuntimeErrorException{L"Error converting non-ascii text to string"sv};
    Execution::Throw (kException_);
}

/*
 ********************************************************************************
 ******************************** operator"" _k *********************************
 ********************************************************************************
 */
String Characters::operator"" _k (const wchar_t* s, size_t len)
{
    return String::FromStringConstant (span<const wchar_t>{s, len});
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
 *********************************** operator+ **********************************
 ********************************************************************************
 */
String Characters::operator+ (const wchar_t* lhs, const String& rhs)
{
    RequireNotNull (lhs);
    Memory::StackBuffer<wchar_t> ignored1;
    auto                         rhsDataSpan = rhs.GetData (&ignored1);
    return String{String::mk_ (span{lhs, ::wcslen (lhs)}, rhsDataSpan)};
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
    // Note this could easily use char8_t, wchar_t, char32_t, or whatever. Choose char8_t on the theorey that
    // this will most often avoid a copy, and making the most often case faster is probably a win. Also, even close, it
    // will have less 'empty space' and be more compact, so will digest faster.
    Memory::StackBuffer<char8_t> maybeIgnoreBuf1;
    span<const char8_t>          s = arg.GetData (&maybeIgnoreBuf1);
    if (s.empty ()) {
        static const size_t kZeroDigest_ = DIGESTER{}(nullptr, nullptr);
        return kZeroDigest_;
    }
    else {
        return DIGESTER{}(as_bytes (s));
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
