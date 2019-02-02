/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
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
#include "../Containers/Common.h"
#include "../Containers/Set.h"
#include "../Execution/Exception.h"
#include "../Execution/Exceptions.h"
#include "../Math/Common.h"
#include "../Memory/BlockAllocated.h"
#include "../Memory/Common.h"
#include "../Memory/SmallStackBuffer.h"

#include "Concrete/Private/String_BufferedStringRep.h"
#include "RegularExpression.h"
#include "SDKString.h"
#include "StringBuilder.h"
#include "String_Constant.h"

#include "String.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

using Memory::SmallStackBuffer;
using Memory::SmallStackBufferCommon;
using Traversal::Iterator;
using Traversal::IteratorOwnerID;

namespace {
    // note - we use UseBlockAllocationIfAppropriate iff kIterableUsesStroikaSharedPtr because SharedPtr separately allocates
    // counter and counted object. So we benefit from blockallocation on T. But if using shared_ptr, this uses make_shared that already combines the counter and
    // the shared object...
    class String_BufferedArray_Rep_
        : public Concrete::Private::BufferedStringRep::_Rep,
          public conditional_t<Traversal::kIterableUsesStroikaSharedPtr, Memory::UseBlockAllocationIfAppropriate<String_BufferedArray_Rep_>, Configuration::Empty> {

    private:
        using inherited = Concrete::Private::BufferedStringRep::_Rep;

    public:
        String_BufferedArray_Rep_ (const wchar_t* start, const wchar_t* end)
            : inherited (start, end)
        {
        }
        String_BufferedArray_Rep_ (const wchar_t* start, const wchar_t* end, size_t reserveExtraCharacters)
            : inherited (start, end, reserveExtraCharacters)
        {
        }
        virtual _IterableRepSharedPtr Clone ([[maybe_unused]] IteratorOwnerID forIterableEnvelope) const override
        {
            AssertNotReached (); // Since Strings now immutable, this should never be called
            // Because of 'Design Choice - Iterable<T> / Iterator<T> behavior' in String class docs - we
            // ignore suggested IteratorOwnerID
            return String::MakeSmartPtr<String_BufferedArray_Rep_> (_fStart, _fEnd);
        }
    };
}

#if 0
//cannot do (except for special case at end) as of Stroika v2.0a148 - because we enforce that strings pre-nul-terminated for thread safety reasons (so cannot
// do in const c_str() method - and have unsynchonzied).
namespace   {
    struct String_Substring_ : public String {
        class   MyRep_ : public _IRep , public Memory::UseBlockAllocationIfAppropriate<MyRep_> {
            using inherited = _IRep;
        public:
            MyRep_ (const _SafeReadRepAccessor& savedSP, const wchar_t* start, const wchar_t* end)
                : inherited (start, end)
                , fSaved_ (savedSP)
            {
                Assert (reinterpret_cast<const wchar_t*> (fSaved_._ConstGetRep ().Peek ()) <= _fStart and _fStart <= _fEnd);
            }
            MyRep_ (const MyRep_& from)
                : inherited (from._fStart, from._fEnd)
                , fSaved_ (from.fSaved_)
            {
                Assert (reinterpret_cast<const wchar_t*> (fSaved_._ConstGetRep ().Peek ()) <= _fStart and _fStart <= _fEnd);
            }
            virtual  _IterableRepSharedPtr   Clone (IteratorOwnerID forIterableEnvelope) const override
            {
                AssertNotReached ();    // Since Strings now immutable, this should never be called
                return String_Substring_::MakeSmartPtr<MyRep_> (*this);
            }
        private:
            _SafeReadRepAccessor  fSaved_;
        };
    };
}
#endif

namespace {
    template <typename FACET>
    struct deletable_facet_ : FACET {
        template <typename... Args>
        deletable_facet_ (Args&&... args)
            : FACET (forward<Args> (args)...)
        {
        }
        ~deletable_facet_ () {}
    };
}

/*
 ********************************************************************************
 ****************************** String::_IRep ***********************************
 ********************************************************************************
 */
Traversal::Iterator<Character> String::_IRep::MakeIterator ([[maybe_unused]] IteratorOwnerID suggestedOwner) const
{
    struct MyIterRep_ : Iterator<Character>::IRep, public Memory::UseBlockAllocationIfAppropriate<MyIterRep_> {
        _SharedPtrIRep fStr; // effectively RO, since if anyone modifies, our copy will remain unchanged
        size_t         fCurIdx;
        MyIterRep_ (const _SharedPtrIRep& r, size_t idx = 0)
            : fStr (r)
            , fCurIdx (idx)
        {
            Require (fCurIdx <= fStr->_GetLength ());
        }
        virtual Iterator<Character>::RepSmartPtr Clone () const override
        {
            return Iterator<Character>::MakeSmartPtr<MyIterRep_> (fStr, fCurIdx);
        }
        virtual IteratorOwnerID GetOwner () const override
        {
            // Simple to enforce Iterator<> compare semantics, but nothing else needed
            return fStr.get ();
        }
        virtual void More (optional<Character>* result, bool advance) override
        {
            RequireNotNull (result);
            if (advance)
                [[LIKELY_ATTR]]
                {
                    Require (fCurIdx <= fStr->_GetLength ());
                    fCurIdx++;
                }
            if (fCurIdx < fStr->_GetLength ())
                [[LIKELY_ATTR]]
                {
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
            const MyIterRep_* rrhs = dynamic_cast<const MyIterRep_*> (rhs);
            AssertNotNull (rrhs);
            Require (fStr == rrhs->fStr); // from same string object
            return fCurIdx == rrhs->fCurIdx;
        }
    };
// Because of 'Design Choice - Iterable<T> / Iterator<T> behavior' in String class docs - we
// ignore suggested IteratorOwnerID - which explains the arg to Clone () below
#if qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
    return Iterator<Character> (Iterator<Character>::MakeSmartPtr<MyIterRep_> (dynamic_pointer_cast<_SharedPtrIRep::element_type> (const_cast<String::_IRep*> (this)->shared_from_this ())));
#else
    return Iterator<Character> (Iterator<Character>::MakeSmartPtr<MyIterRep_> (const_cast<String::_IRep*> (this)->shared_from_this ()));
#endif
}

size_t String::_IRep::GetLength () const
{
    Assert (_fStart <= _fEnd);
    return _fEnd - _fStart;
}

bool String::_IRep::IsEmpty () const
{
    Assert (_fStart <= _fEnd);
    return _fEnd == _fStart;
}

void String::_IRep::Apply (_APPLY_ARGTYPE doToElement) const
{
    _Apply (doToElement);
}

Traversal::Iterator<Character> String::_IRep::FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const
{
    return _FindFirstThat (doToElement, suggestedOwner);
}

/*
 ********************************************************************************
 ************************************* String ***********************************
 ********************************************************************************
 */
static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");

#if __cpp_char8_t >= 201811L
String::String (const char8_t* cString)
    : String (cString, cString + Characters::CString::Length (cString))
{
    RequireNotNull (cString);
}
#endif

String::String (const char16_t* cString)
    : String (cString, cString + Characters::CString::Length (cString))
{
    RequireNotNull (cString);
}

String::String (const char32_t* cString)
    : String (cString, cString + CString::Length (cString))
{
}

String::String (const basic_string_view<wchar_t>& str)
    : String{String_Constant{str}} // safe to avoid copying string rep here - just point at memory for string
{
    Require (str.data ()[str.length ()] == 0); // Because Stroika strings provide the guarantee that they can be converted to c_str () - we require the input memory
                                               // for these const strings are also nul-terminated.
                                               // DONT try to CORRECT this if found wrong, because whenever you use "stuff"sv - the string literal will always
                                               // be nul-terminated.
                                               // -- LGP 2019-01-29
}

namespace {
    inline String mkWS_ (const Traversal::Iterable<Character>& src)
    {
        StringBuilder r;
        for (const auto&& i : src) {
            Containers::ReserveSpeedTweekAdd1 (r);
            r.push_back (i.As<wchar_t> ());
        }
        return r.str ();
    }
    inline String mkWS_ (const Character& src)
    {
        wchar_t c = src.As<wchar_t> ();
        return String (&c, &c + 1);
    }
}

String::String (const Iterable<Character>& src)
    : String (mkWS_ (src))
{
}

String::String (const Character& c)
    : String (mkWS_ (c))
{
}

String String::FromUTF8 (const char* from, const char* to)
{
    RequireNotNull (from);
    RequireNotNull (to);
    Require (from <= to);
    size_t                    cvtBufSize = UTFConvert::QuickComputeConversionOutputBufferSize<UTFConvert::UTF8, wchar_t> (from, to);
    SmallStackBuffer<wchar_t> buf{SmallStackBufferCommon::eUninitialized, cvtBufSize};
    wchar_t*                  outStr = buf.begin ();
    UTFConvert::Convert (&from, to, &outStr, buf.end (), UTFConvert::lenientConversion);
    return String{buf.begin (), outStr};
}
String String::FromSDKString (const SDKChar* from)
{
    RequireNotNull (from);
    // @todo FIX PERFORMANCE
    return String (SDKString2Wide (from));
}

String String::FromSDKString (const SDKChar* from, const SDKChar* to)
{
// @todo FIX PERFORMANCE
#if qTargetPlatformSDKUseswchar_t
    return String (from, to);
#else
    wstring tmp;
    NarrowStringToWide (from, to, GetDefaultSDKCodePage (), &tmp);
    return String (tmp);
#endif
}

String String::FromSDKString (const SDKString& from)
{
// @todo FIX PERFORMANCE
#if qTargetPlatformSDKUseswchar_t
    return String (from);
#else
    return String (NarrowStringToWide (from, GetDefaultSDKCodePage ()));
#endif
}

String String::FromNarrowSDKString (const char* from)
{
    RequireNotNull (from);
    // @todo FIX PERFORMANCE
    return NarrowSDKStringToWide (from);
}

String String::FromNarrowSDKString (const char* from, const char* to)
{
    // @todo FIX PERFORMANCE
    wstring tmp;
    NarrowStringToWide (from, to, GetDefaultSDKCodePage (), &tmp);
    return String (tmp);
}

String String::FromNarrowSDKString (const string& from)
{
    // @todo FIX PERFORMANCE
    return NarrowSDKStringToWide (from);
}

String String::FromNarrowString (const char* from, const char* to, const locale& l)
{
    // See http://en.cppreference.com/w/cpp/locale/codecvt/~codecvt
    using Destructible_codecvt_byname = deletable_facet_<codecvt_byname<wchar_t, char, mbstate_t>>;
    Destructible_codecvt_byname cvt{l.name ()};

    // http://en.cppreference.com/w/cpp/locale/codecvt/in
    mbstate_t            mbstate{};
    size_t               externalSize = to - from;
    wstring              resultWStr (externalSize, '\0');
    const char*          from_next;
    wchar_t*             to_next;
    codecvt_base::result result = cvt.in (mbstate, from, to, from_next, &resultWStr[0], &resultWStr[resultWStr.size ()], to_next);
    if (result != codecvt_base::ok)
        [[UNLIKELY_ATTR]]
        {
            Execution::Throw (Execution::Exception (L"Error converting locale multibyte string to UNICODE"sv));
        }
    resultWStr.resize (to_next - &resultWStr[0]);
    return resultWStr;
}

String String::FromASCII (const char* from, const char* to)
{
    RequireNotNull (from);
    Require (from <= to);
    SmallStackBuffer<wchar_t> buf{SmallStackBufferCommon::eUninitialized, static_cast<size_t> (to - from)};
    wchar_t*                  pOut = buf.begin ();
    for (const char* i = from; i != to; ++i, pOut++) {
        Require (isascii (*i));
        *pOut = *i;
    }
    return String{buf.begin (), pOut};
}

String String::FromISOLatin1 (const char* start, const char* end)
{
    /*
     *  From http://unicodebook.readthedocs.io/encodings.html
     *      "For example, ISO-8859-1 are the first 256 Unicode code points (U+0000-U+00FF)."
     */
    const char*               s = start;
    const char*               e = end;
    SmallStackBuffer<wchar_t> buf{SmallStackBufferCommon::eUninitialized, static_cast<size_t> (e - s)};
    wchar_t*                  pOut = buf.begin ();
    for (const char* i = s; i != e; ++i, pOut++) {
        *pOut = *i;
    }
    return String{buf.begin (), pOut};
}

String::_SharedPtrIRep String::mkEmpty_ ()
{
    static constexpr wchar_t    kEmptyStr_[1] = {};
    static const _SharedPtrIRep s_            = mk_ (std::begin (kEmptyStr_), std::begin (kEmptyStr_));
    return s_;
}

String::_SharedPtrIRep String::mk_ (const wchar_t* start, const wchar_t* end)
{
    RequireNotNull (start);
    RequireNotNull (end);
    Require (start <= end);
    return MakeSmartPtr<String_BufferedArray_Rep_> (start, end);
}

String::_SharedPtrIRep String::mk_ (const wchar_t* start1, const wchar_t* end1, const wchar_t* start2, const wchar_t* end2)
{
    RequireNotNull (start1);
    RequireNotNull (end1);
    Require (start1 <= end1);
    RequireNotNull (start2);
    RequireNotNull (end2);
    Require (start2 <= end2);
    size_t len1 = end1 - start1;
    auto   sRep = MakeSmartPtr<String_BufferedArray_Rep_> (start1, end1, (end2 - start2));
    if (start2 != end2)
        [[LIKELY_ATTR]]
        {
            sRep->InsertAt (reinterpret_cast<const Character*> (start2), reinterpret_cast<const Character*> (end2), len1);
        }
    return sRep;
}

String::_SharedPtrIRep String::mk_ (const char16_t* from, const char16_t* to)
{
    RequireNotNull (from);
    RequireNotNull (to);
    Require (from <= to);
    if constexpr (sizeof (char16_t) == sizeof (wchar_t)) {
        return mk_ (reinterpret_cast<const wchar_t*> (from), reinterpret_cast<const wchar_t*> (to));
    }
    else {
        size_t                    cvtBufSize = UTFConvert::QuickComputeConversionOutputBufferSize<char16_t, wchar_t> (from, to);
        SmallStackBuffer<wchar_t> buf{SmallStackBufferCommon::eUninitialized, cvtBufSize};
        wchar_t*                  outStr = buf.begin ();
        UTFConvert::Convert (&from, to, &outStr, buf.end (), UTFConvert::lenientConversion);
        return mk_ (buf.begin (), outStr);
    }
}

String::_SharedPtrIRep String::mk_ (const char32_t* from, const char32_t* to)
{
    RequireNotNull (from);
    RequireNotNull (to);
    Require (from <= to);
    if constexpr (sizeof (char32_t) == sizeof (wchar_t)) {
        return mk_ (reinterpret_cast<const wchar_t*> (from), reinterpret_cast<const wchar_t*> (to));
    }
    else {
        size_t                    cvtBufSize = UTFConvert::QuickComputeConversionOutputBufferSize<char32_t, wchar_t> (from, to);
        SmallStackBuffer<wchar_t> buf{SmallStackBufferCommon::eUninitialized, cvtBufSize};
        wchar_t*                  outStr = buf.begin ();
        UTFConvert::Convert (&from, to, &outStr, buf.end (), UTFConvert::lenientConversion);
        return mk_ (buf.begin (), outStr);
    }
}

String String::Concatenate (const String& rhs) const
{
    _SafeReadRepAccessor                     thisAccessor{this};
    pair<const Character*, const Character*> lhsD   = thisAccessor._ConstGetRep ().GetData ();
    size_t                                   lhsLen = lhsD.second - lhsD.first;
    if (lhsLen == 0) {
        return rhs;
    }
    _SafeReadRepAccessor                     rhsAccessor{&rhs};
    pair<const Character*, const Character*> rhsD = rhsAccessor._ConstGetRep ().GetData ();
    if (rhsD.first == rhsD.second)
        [[UNLIKELY_ATTR]]
        {
            return *this;
        }
    return String (mk_ (reinterpret_cast<const wchar_t*> (lhsD.first), reinterpret_cast<const wchar_t*> (lhsD.second),
                        reinterpret_cast<const wchar_t*> (rhsD.first), reinterpret_cast<const wchar_t*> (rhsD.second)));
}

String String::Concatenate (const wchar_t* appendageCStr) const
{
    RequireNotNull (appendageCStr);
    _SafeReadRepAccessor                     thisAccessor{this};
    pair<const Character*, const Character*> lhsD   = thisAccessor._ConstGetRep ().GetData ();
    size_t                                   lhsLen = lhsD.second - lhsD.first;
    if (lhsLen == 0)
        [[UNLIKELY_ATTR]]
        {
            return String (appendageCStr);
        }
    return String (mk_ (reinterpret_cast<const wchar_t*> (lhsD.first), reinterpret_cast<const wchar_t*> (lhsD.second),
                        appendageCStr, appendageCStr + ::wcslen (appendageCStr)));
}

void String::SetCharAt (Character c, size_t i)
{
    // @Todo - redo with check if char is acttually chanigng and if so use
    // mk/4 4 arg string maker instead.??? Or some such...
    Require (i >= 0);
    Require (i < GetLength ());
    // Expensive, but you can use StringBuilder directly to avoid the performance costs
    StringBuilder sb{*this};
    Require (i < GetLength ());
    sb.SetAt (c, i);
    *this = sb.str ();
}

String String::InsertAt (const Character* from, const Character* to, size_t at) const
{
    Require (at >= 0);
    Require (at <= GetLength ());
    Require (from <= to);
    Require (from != nullptr or from == to);
    Require (to != nullptr or from == to);
    if (from == to) {
        return *this;
    }
    _SafeReadRepAccessor                     copyAccessor{this};
    pair<const Character*, const Character*> d    = copyAccessor._ConstGetRep ().GetData ();
    auto                                     sRep = MakeSmartPtr<String_BufferedArray_Rep_> (reinterpret_cast<const wchar_t*> (d.first), reinterpret_cast<const wchar_t*> (d.second), (to - from));
    sRep->InsertAt (from, to, at);
    return String (move (sRep));
}

String String::RemoveAt (size_t from, size_t to) const
{
    Require (from <= to);
    Require (to <= GetLength ());
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
        return String (mk_ (p, p + from, p + to, p + length));
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
    //@todo could improve performance with strength reduction
    _SafeReadRepAccessor accessor{this};
    Require (startAt <= accessor._ConstGetRep ()._GetLength ());
    size_t length = accessor._ConstGetRep ()._GetLength ();
    switch (co) {
        case CompareOptions::eCaseInsensitive: {
            for (size_t i = startAt; i < length; i++) {
                if (accessor._ConstGetRep ().GetAt (i).ToLowerCase () == c.ToLowerCase ()) {
                    return i;
                }
            }
        } break;
        case CompareOptions::eWithCase: {
            for (size_t i = startAt; i < length; i++) {
                if (accessor._ConstGetRep ().GetAt (i) == c) {
                    return i;
                }
            }
        } break;
    }
    return {};
}

optional<size_t> String::Find (const String& subString, size_t startAt, CompareOptions co) const
{
    //@todo: FIX HORRIBLE PERFORMANCE!!!
    _SafeReadRepAccessor accessor{this};
    Require (startAt <= accessor._ConstGetRep ()._GetLength ());

    size_t subStrLen = subString.GetLength ();
    if (subStrLen == 0) {
        return (accessor._ConstGetRep ()._GetLength () == 0) ? optional<size_t>{} : 0;
    }
    if (accessor._ConstGetRep ()._GetLength () < subStrLen) {
        return {}; // important test cuz size_t is unsigned
    }

    size_t limit = accessor._ConstGetRep ()._GetLength () - subStrLen;
    switch (co) {
        case CompareOptions::eCaseInsensitive: {
            for (size_t i = startAt; i <= limit; i++) {
                for (size_t j = 0; j < subStrLen; j++) {
                    if (accessor._ConstGetRep ().GetAt (i + j).ToLowerCase () != subString[j].ToLowerCase ()) {
                        goto nogood1;
                    }
                }
                return i;
            nogood1:;
            }
        } break;
        case CompareOptions::eWithCase: {
            for (size_t i = startAt; i <= limit; i++) {
                for (size_t j = 0; j < subStrLen; j++) {
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
    const String threadSafeCopy{*this};
    Require (startAt <= threadSafeCopy.GetLength ());
    wstring tmp = threadSafeCopy.As<wstring> ();
    Require (startAt < tmp.size ());
    tmp = tmp.substr (startAt);
    wsmatch res;
    regex_search (tmp, res, regEx.GetCompiled ());
    if (res.size () >= 1) {
        return pair<size_t, size_t> (startAt + res.position (), startAt + res.position () + res.length ());
    }
    return {};
}

vector<size_t> String::FindEach (const String& string2SearchFor, CompareOptions co) const
{
    vector<size_t> result;
    const String   threadSafeCopy{*this};
    for (optional<size_t> i = threadSafeCopy.Find (string2SearchFor, 0, co); i; i = threadSafeCopy.Find (string2SearchFor, *i, co)) {
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
            result.push_back (pair<size_t, size_t> (res.position (mi), matchLen));
        }
    }
    return result;
}

vector<RegularExpressionMatch> String::FindEachMatch (const RegularExpression& regEx) const
{
    vector<RegularExpressionMatch> result;
    wstring                        tmp{As<wstring> ()};
    for (wsregex_iterator i = wsregex_iterator (tmp.begin (), tmp.end (), regEx.GetCompiled ()); i != wsregex_iterator (); ++i) {
        wsmatch match{*i};
        Assert (match.size () != 0);
        size_t                       n = match.size ();
        Containers::Sequence<String> s;
        for (size_t j = 1; j < n; ++j) {
            s.Append (match.str (j));
        }
        result.push_back (RegularExpressionMatch (match.str (0), s));
    }
    return result;
}

vector<String> String::FindEachString (const RegularExpression& regEx) const
{
    vector<String> result;
    wstring        tmp{As<wstring> ()};
#if 1
    for (wsregex_iterator i = wsregex_iterator (tmp.begin (), tmp.end (), regEx.GetCompiled ()); i != wsregex_iterator (); ++i) {
        result.push_back (String{i->str ()});
    }
#else
    wsmatch res;
    if (regex_search (tmp, res, regEx.GetCompiled ())) {
        result.reserve (res.size ());
        for (auto i : res) {
            result.push_back (String{i});
        }
    }
#endif
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
        result.push_back (String (*i));
    }
    return result;
}
#endif
optional<size_t> String::RFind (Character c) const
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
    const String threadSafeCopy{*this};
    //@todo: FIX HORRIBLE PERFORMANCE!!!
    /*
     * Do quickie implementation, and don't worry about efficiency...
     */
    size_t subStrLen = subString.GetLength ();
    if (subStrLen == 0) {
        return ((threadSafeCopy.GetLength () == 0) ? optional<size_t>{} : threadSafeCopy.GetLength () - 1);
    }

    size_t limit = threadSafeCopy.GetLength () - subStrLen + 1;
    for (size_t i = limit; i > 0; --i) {
        if (threadSafeCopy.SubString (i - 1, i - 1 + subStrLen) == subString) {
            return i - 1;
        }
    }
    return {};
}

String String::Replace (size_t from, size_t to, const String& replacement) const
{
    // @todo rewrite using StriingBuilder (performance)
    return SubString (0, from) + replacement + SubString (to);
}

bool String::StartsWith (const Character& c, CompareOptions co) const
{
    _SafeReadRepAccessor accessor{this};
    if (accessor._ConstGetRep ()._GetLength () == 0) {
        return false;
    }
    return accessor._ConstGetRep ().GetAt (0).Compare (c, co) == 0;
}

bool String::StartsWith (const String& subString, CompareOptions co) const
{
    _SafeReadRepAccessor accessor{this};
    size_t               subStrLen = subString.GetLength ();
    if (subStrLen > accessor._ConstGetRep ()._GetLength ()) {
        return false;
    }
#if qDebug
    bool referenceResult = (SubString (0, subString.GetLength ()).Compare (subString, co) == 0); // this check isnt threadsafe - redo
#endif
    const Character*                         subStrStart = reinterpret_cast<const Character*> (subString.c_str ());
    pair<const Character*, const Character*> thisData    = accessor._ConstGetRep ().GetData ();
    bool                                     result      = (Character::Compare (thisData.first, thisData.first + subStrLen, subStrStart, subStrStart + subStrLen, co) == 0);
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
    return useRep.GetAt (thisStrLen - 1).Compare (c, co) == 0;
}

bool String::EndsWith (const String& subString, CompareOptions co) const
{
    _SafeReadRepAccessor accessor{this};
    size_t               thisStrLen = accessor._ConstGetRep ()._GetLength ();
    size_t               subStrLen  = subString.GetLength ();
    if (subStrLen > thisStrLen) {
        return false;
    }
#if qDebug
    bool referenceResult = (SubString (thisStrLen - subStrLen, thisStrLen).Compare (subString, co) == 0); // this check isnt threadsafe - redo
#endif
    const Character*                         subStrStart = reinterpret_cast<const Character*> (subString.c_str ());
    pair<const Character*, const Character*> thisData    = accessor._ConstGetRep ().GetData ();
    bool                                     result      = (Character::Compare (thisData.first + thisStrLen - subStrLen, thisData.first + thisStrLen, subStrStart, subStrStart + subStrLen, co) == 0);
    Assert (result == referenceResult);
    return result;
}

bool String::Match (const RegularExpression& regEx) const
{
    wstring tmp{As<wstring> ()};
    return regex_match (tmp.begin (), tmp.end (), regEx.GetCompiled ());
}

String String::ReplaceAll (const RegularExpression& regEx, const String& with) const
{
    return String (regex_replace (As<wstring> (), regEx.GetCompiled (), with.As<wstring> ()));
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

String String::ReplaceAll (const function<bool(Character)>& replaceCharP, const String& with) const
{
    StringBuilder sb;
    for (const Character&& i : *this) {
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
    for (const Character&& i : *this) {
        if (charSet.Contains (i)) {
            sb += with;
        }
        else {
            sb += i;
        }
    }
    return sb.str ();
}

String String::FilteredString (const Iterable<Character>& badCharacters, optional<Character> replacement) const
{
    StringBuilder sb;
    for (const Character&& i : *this) {
        if (badCharacters.Contains (i)) {
            if (replacement) {
                sb += *replacement;
            }
        }
        else {
            sb += i;
        }
    }
    return sb.str ();
}

String String::FilteredString (const RegularExpression& badCharacters, optional<Character> replacement) const
{
    return ReplaceAll (badCharacters, replacement.has_value () ? String (*replacement) : L"");
}

String String::FilteredString (const function<bool(Character)>& badCharacterP, optional<Character> replacement) const
{
    StringBuilder sb;
    for (const Character&& i : *this) {
        if (badCharacterP (i)) {
            if (replacement) {
                sb += *replacement;
            }
        }
        else {
            sb += i;
        }
    }
    return sb.str ();
}

Containers::Sequence<String> String::Tokenize (const function<bool(Character)>& isTokenSeperator, bool trim) const
{
    String                       tmp{*this}; // quickie thread-safety - do diffenrtly soon with new thread safety model... -- LGP 2014-10-31
    Containers::Sequence<String> r;
    bool                         inToken = false;
    StringBuilder                curToken;
    size_t                       len = tmp.size ();
    for (size_t i = 0; i != len; ++i) {
        Character c          = tmp[i];
        bool      newInToken = not isTokenSeperator (c);
        if (inToken != newInToken) {
            if (inToken) {
                String s{curToken.str ()};
                if (trim) {
                    s = s.Trim ();
                }
                r.Append (s);
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
        r.Append (s);
    }
    return r;
}

Containers::Sequence<String> String::Tokenize (const Containers::Set<Character>& delimiters, bool trim) const
{
    /*
     *  @todo Inefficient impl, to encourage code saving. Do more efficently.
     */
    return Tokenize (
        [delimiters](Character c) -> bool {
            return delimiters.Contains (c);
        },
        trim);
}

String String::SubString_ (const _SafeReadRepAccessor& thisAccessor, size_t thisLen, size_t from, size_t to)
{
    Require (from <= to);
    Require (to <= thisLen);
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
#if 0
        // @todo SEE TODO in String.h
        // @todo - FIX - MUST RETURN thisAccessor AS STRING
#endif
    }
#if 0
    // If we are using kOptimizedForStringsLenLessOrEqualTo - so the string data gets stored in the rep anyhow, thats probably
    // faster than adding a ref to the original string object? Not too sure of this optimization
    if (len <= String_BufferedArray_Rep_::kOptimizedForStringsLenLessOrEqualTo) {
        return mk_ (start, end);
    }
#endif
    return mk_ (start, end);
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
    // @todo - NOT ENVELOPE THREADSAFE (BUT MOSTLY OK)
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
    return String ();
}

String String::RTrim (bool (*shouldBeTrimmmed) (Character)) const
{
    // @todo - NOT THREADAFE - BUGGY - MUST USE ACCESSOR TO RECONSTRUCT WHAT WE STRING WE RETURN!!! - EVEN FOR SUBSTR
    // @todo - NOT ENVELOPE THREADSAFE (BUT MOSTLY OK)
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
    size_t n = result.GetLength ();
    for (size_t i = 0; i < n; ++i) {
        Character c = result[i];
        if ((removeCharIf) (c)) {
            // on first removal, clone part of string done so far, and start appending
            String tmp = result.SubString (0, i);
            // Now keep iterating IN THIS LOOP appending characters and return at the end of this loop
            i++;
            for (; i < n; ++i) {
                c = result[i];
                if (not(removeCharIf) (c)) {
                    tmp += c;
                }
            }
            return tmp;
        }
    }
    return *this; // if we NEVER get removeCharIf return false, just clone this
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
    return not FindFirstThat ([](Character c) -> bool { return not c.IsWhitespace (); });
}

String String::LimitLength (size_t maxLen, bool keepLeft) const
{
#if 1
    static const String kELIPSIS_{String_Constant (L"\u2026")};
#else
    static const String kELIPSIS_{String_Constant (L"...")};
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
    if (result != codecvt_base::ok)
        [[UNLIKELY_ATTR]]
        {
            Execution::Throw (Execution::Exception (L"Error converting locale multibyte string to UNICODE"sv));
        }
    into->resize (to_next - &(*into)[0]);
}

template <>
void String::AsUTF8 (string* into) const
{
    RequireNotNull (into);
    _SafeReadRepAccessor                     accessor{this};
    pair<const Character*, const Character*> lhsD = accessor._ConstGetRep ().GetData ();
    Assert (sizeof (Character) == sizeof (wchar_t));
    const wchar_t*                     wcp        = (const wchar_t*)lhsD.first;
    const wchar_t*                     wcpe       = (const wchar_t*)lhsD.second;
    size_t                             cvtBufSize = UTFConvert::QuickComputeConversionOutputBufferSize<wchar_t, UTFConvert::UTF8> (wcp, wcpe);
    SmallStackBuffer<UTFConvert::UTF8> buf{SmallStackBufferCommon::eUninitialized, cvtBufSize};
    UTFConvert::UTF8*                  outStr = buf.begin ();
    UTFConvert::Convert (&wcp, wcpe, &outStr, buf.end (), UTFConvert::lenientConversion);
    into->assign (buf.begin (), outStr);
}

void String::AsUTF16 (u16string* into) const
{
    RequireNotNull (into);
    _SafeReadRepAccessor accessor{this};
    size_t               n{accessor._ConstGetRep ()._GetLength ()};
    const Character*     cp = accessor._ConstGetRep ()._Peek ();
    if constexpr (sizeof (wchar_t) == sizeof (char16_t)) {
        Assert (sizeof (Character) == sizeof (char16_t));
        const char16_t* wcp = (const char16_t*)cp;
        into->assign (wcp, wcp + n);
    }
    else {
        Assert (sizeof (Character) == sizeof (wchar_t));
        const wchar_t*             wcp        = (const wchar_t*)cp;
        size_t                     cvtBufSize = UTFConvert::QuickComputeConversionOutputBufferSize<wchar_t, char16_t> (wcp, wcp + n);
        SmallStackBuffer<char16_t> buf{SmallStackBufferCommon::eUninitialized, cvtBufSize};
        char16_t*                  outStr = buf.begin ();
        UTFConvert::Convert (&wcp, wcp + n, &outStr, buf.end (), UTFConvert::lenientConversion);
        into->assign (buf.begin (), outStr);
    }
}

void String::AsUTF32 (u32string* into) const
{
    RequireNotNull (into);
    _SafeReadRepAccessor accessor{this};
    size_t               n{accessor._ConstGetRep ()._GetLength ()};
    const Character*     cp = accessor._ConstGetRep ()._Peek ();
    if constexpr (sizeof (wchar_t) == sizeof (char32_t)) {
        Assert (sizeof (Character) == sizeof (char32_t));
        const char32_t* wcp = (const char32_t*)cp;
        into->assign (wcp, wcp + n);
    }
    else {
        Assert (sizeof (Character) == sizeof (wchar_t));
        const wchar_t*             wcp        = (const wchar_t*)cp;
        size_t                     cvtBufSize = UTFConvert::QuickComputeConversionOutputBufferSize<wchar_t, char32_t> (wcp, wcp + n);
        SmallStackBuffer<char32_t> buf{SmallStackBufferCommon::eUninitialized, cvtBufSize};
        char32_t*                  outStr = buf.begin ();
        UTFConvert::Convert (&wcp, wcp + n, &outStr, buf.end (), UTFConvert::lenientConversion);
        into->assign (buf.begin (), outStr);
    }
}

void String::AsSDKString (SDKString* into) const
{
    RequireNotNull (into);
    _SafeReadRepAccessor                     accessor{this};
    pair<const Character*, const Character*> lhsD = accessor._ConstGetRep ().GetData ();
#if qTargetPlatformSDKUseswchar_t
    into->assign (reinterpret_cast<const wchar_t*> (lhsD.first), reinterpret_cast<const wchar_t*> (lhsD.second));
#else
    WideStringToNarrow (reinterpret_cast<const wchar_t*> (lhsD.first), reinterpret_cast<const wchar_t*> (lhsD.second), GetDefaultSDKCodePage (), into);
#endif
}

void String::AsNarrowSDKString (string* into) const
{
    RequireNotNull (into);
    _SafeReadRepAccessor                     accessor{this};
    pair<const Character*, const Character*> lhsD = accessor._ConstGetRep ().GetData ();
    WideStringToNarrow (reinterpret_cast<const wchar_t*> (lhsD.first), reinterpret_cast<const wchar_t*> (lhsD.second), GetDefaultSDKCodePage (), into);
}

template <>
void String::AsASCII (string* into) const
{
    RequireNotNull (into);
    into->clear ();
    _SafeReadRepAccessor accessor{this};
    size_t               len = accessor._ConstGetRep ().GetLength ();
    into->reserve (len);
    for (size_t i = 0; i < len; ++i) {
        Assert (accessor._ConstGetRep ().GetAt (i).IsASCII ());
        into->push_back (static_cast<char> (accessor._ConstGetRep ().GetAt (i).GetCharacterCode ()));
    }
}

void String::erase (size_t from)
{
    *this = RemoveAt (from, GetLength ());
}

void String::erase (size_t from, size_t count)
{
    // https://stroika.atlassian.net/browse/STK-445
    // @todo - NOT ENVELOPE THREADSAFE
    // MUST ACQUIRE ACCESSOR HERE - not just that RemoteAt threadsafe - but must SYNC at this point - need AssureExternallySycnonized stuff here!!!
    //
    // TODO: Double check STL definition - but I think they allow for count to be 'too much' - and silently trim to end...
    size_t max2Erase = static_cast<size_t> (max (static_cast<ptrdiff_t> (0), static_cast<ptrdiff_t> (GetLength ()) - static_cast<ptrdiff_t> (from)));
    *this            = RemoveAt (from, from + min (count, max2Erase));
}

/*
 ********************************************************************************
 ********************************** operator<< **********************************
 ********************************************************************************
 */
wostream& Characters::operator<< (wostream& out, const String& s)
{
    // Tried two impls, but first empirically appears quicker.
    // HOWERVER - THIS IS INTRINSICALLY NOT THREASDAFE (if s changed while another thread writin it)
    String::_SafeReadRepAccessor             thisAccessor{&s};
    pair<const Character*, const Character*> p = thisAccessor._ConstGetRep ().GetData ();
    out.write (reinterpret_cast<const wchar_t*> (p.first), p.second - p.first);
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
    String::_SafeReadRepAccessor             rhsAccessor{&rhs};
    pair<const Character*, const Character*> rhsD     = rhsAccessor._ConstGetRep ().GetData ();
    size_t                                   lhsLen   = ::wcslen (lhs);
    size_t                                   totalLen = lhsLen + (rhsD.second - rhsD.first);
    auto                                     sRep     = String::MakeSmartPtr<String_BufferedArray_Rep_> (reinterpret_cast<const wchar_t*> (lhs), reinterpret_cast<const wchar_t*> (lhs + lhsLen), totalLen);
    if (rhsD.first != rhsD.second) {
        sRep->InsertAt (rhsD.first, rhsD.second, lhsLen);
    }
    return String (move (sRep));
}

/*
 ********************************************************************************
 *********** Characters::MakeModuleDependency_String ****************************
 ********************************************************************************
 */
Execution::ModuleDependency Characters::MakeModuleDependency_String ()
{
    return Execution::ModuleInitializer<String_ModuleInit_>::GetDependency ();
}

/*
 ********************************************************************************
 ******************** Characters::String_ModuleInit_ ****************************
 ********************************************************************************
 */
String_ModuleInit_::String_ModuleInit_ ()
    : fBlockAllocationDependency (Memory::MakeModuleDependency_BlockAllocator ())
{
}
