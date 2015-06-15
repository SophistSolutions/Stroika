/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <algorithm>
#include    <cstdarg>
#include    <climits>
#include    <istream>
#include    <string>
#if     !qCompilerAndStdLib_regex_Buggy
#include    <regex>
#endif

#include    "../Containers/Common.h"
#include    "../Execution/Exceptions.h"
#include    "../Execution/StringException.h"
#include    "../Math/Common.h"
#include    "../Memory/Common.h"
#include    "../Memory/BlockAllocated.h"

#include    "RegularExpression.h"
#include    "Concrete/Private/String_BufferedStringRep.h"
#include    "SDKString.h"
#include    "StringBuilder.h"
#include    "String_Constant.h"

#include    "String.h"





using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;


using   Traversal::IteratorOwnerID;
using   Traversal::Iterator;







namespace   {
    class   String_BufferedArray_Rep_ : public Concrete::Private::BufferedStringRep::_Rep {
    private:
        using   inherited   =   Concrete::Private::BufferedStringRep::_Rep;

    public:
#if     qString_Private_BufferedStringRep_UseBlockAllocatedForSmallBufStrings
        DEFINE_CONSTEXPR_CONSTANT (size_t, kOptimizedForStringsLenLessOrEqualTo, Concrete::Private::BufferedStringRepBlock_::kNElts);

#else
        DEFINE_CONSTEXPR_CONSTANT (size_t, kOptimizedForStringsLenLessOrEqualTo, 0);
#endif

    public:
        String_BufferedArray_Rep_ (const wchar_t* start, const wchar_t* end)
            : inherited (start, end)
        {
        }
        String_BufferedArray_Rep_ (const wchar_t* start, const wchar_t* end, size_t reserve)
            : inherited (start, end, reserve)
        {
        }
        virtual  _IterableSharedPtrIRep   Clone (IteratorOwnerID forIterableEnvelope) const override
        {
            AssertNotReached ();    // Since Strings now immutable, this should never be called
            // Because of 'Design Choice - Iterable<T> / Iterator<T> behavior' in String class docs - we
            // ignore suggested IteratorOwnerID
            return _IterableSharedPtrIRep (new String_BufferedArray_Rep_ (_fStart, _fEnd));
        }
#if     qStroika_Foundation_Traveral_IterableUsesStroikaSharedPtr
    public:
        DECLARE_USE_BLOCK_ALLOCATION(String_BufferedArray_Rep_);
#endif
    };
}













namespace   {
    struct String_Substring_ : public String {
        class   MyRep_ : public _IRep {
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
            virtual  _IterableSharedPtrIRep   Clone (IteratorOwnerID forIterableEnvelope) const override
            {
                AssertNotReached ();    // Since Strings now immutable, this should never be called
                return String_Substring_::MakeSharedPtr<MyRep_> (*this);
            }
        private:
            _SafeReadRepAccessor  fSaved_;
        public:
            DECLARE_USE_BLOCK_ALLOCATION(MyRep_);
        };
    };
}






namespace {
    template    <typename FACET>
    struct  deletable_facet_ : FACET {
        template    <typename ...Args>
        deletable_facet_ (Args&& ...args) : FACET (std::forward<Args> (args)...) {}
        ~deletable_facet_() {}
    };
}







#if     !qCompilerAndStdLib_regex_Buggy
namespace   {
    inline  constexpr   regex_constants::syntax_option_type mkOption_ (RegularExpression::SyntaxType st)
    {
        return (st == RegularExpression::SyntaxType::eECMAScript ? regex_constants::ECMAScript : regex_constants::basic);
    }
}
#endif




/*
 ********************************************************************************
 ****************************** String::_IRep ***********************************
 ********************************************************************************
 */
Traversal::Iterator<Character>  String::_IRep::MakeIterator (IteratorOwnerID suggestedOwner) const
{
    struct MyIterRep_ : Iterator<Character>::IRep {
        _SharedPtrIRep  fStr;           // effectively RO, since if anyone modifies, our copy will remain unchanged
        size_t          fCurIdx;
        MyIterRep_ (const _SharedPtrIRep& r, size_t idx = 0)
            : fStr (r)
            , fCurIdx (idx)
        {
            Require (fCurIdx <= fStr->_GetLength ());
        }
        virtual SharedIRepPtr   Clone () const override
        {
            return Iterator<Character>::MakeSharedPtr<MyIterRep_> (fStr, fCurIdx);
        }
        virtual IteratorOwnerID GetOwner () const override
        {
            // Simple to enforce Iterator<> compare semantics, but nothing else needed
            return fStr.get ();
        }
        virtual void    More (Memory::Optional<Character>* result, bool advance) override
        {
            RequireNotNull (result);
            if (advance) {
                Require (fCurIdx <= fStr->_GetLength ());
                fCurIdx++;
            }
            if (fCurIdx < fStr->_GetLength ()) {
                *result = fStr->GetAt (fCurIdx);
            }
            else {
                result->clear ();
            }
        }
        virtual bool    Equals (const IRep* rhs) const override
        {
            RequireNotNull (rhs);
            RequireMember (rhs, MyIterRep_);
            const MyIterRep_* rrhs =   dynamic_cast<const MyIterRep_*> (rhs);
            AssertNotNull (rrhs);
            Require (fStr == rrhs->fStr);   // from same string object
            return fCurIdx == rrhs->fCurIdx;
        }
        DECLARE_USE_BLOCK_ALLOCATION (MyIterRep_);
    };
    // Because of 'Design Choice - Iterable<T> / Iterator<T> behavior' in String class docs - we
    // ignore suggested IteratorOwnerID - which explains the arg to Clone () below
#if     qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
    return Iterator<Character> (Iterator<Character>::SharedIRepPtr (Iterator<Character>::MakeSharedPtr<MyIterRep_> (dynamic_pointer_cast<_SharedPtrIRep::element_type> (const_cast<String::_IRep*> (this)->shared_from_this ()))));
#else
    return Iterator<Character> (Iterator<Character>::SharedIRepPtr (Iterator<Character>::MakeSharedPtr<MyIterRep_> (const_cast<String::_IRep*> (this)->shared_from_this ())));
#endif
}

size_t  String::_IRep::GetLength () const
{
    Assert (_fStart <= _fEnd);
    return _fEnd - _fStart;
}

bool  String::_IRep::IsEmpty () const
{
    Assert (_fStart <= _fEnd);
    return _fEnd == _fStart;
}

void  String::_IRep::Apply (_APPLY_ARGTYPE doToElement) const
{
    _Apply (doToElement);
}

Traversal::Iterator<Character>  String::_IRep::FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const
{
    return _FindFirstThat (doToElement, suggestedOwner);
}

const wchar_t*  String::_IRep::c_str_peek () const  noexcept
{
    return nullptr;
}
















/*
 ********************************************************************************
 ************************************* String ***********************************
 ********************************************************************************
 */
static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
#if     !qCompilerAndStdLib_constexpr_Buggy
constexpr   size_t    String::kBadIndex;
#endif

String::String (const char16_t* cString)
    : inherited (mkEmpty_ ())
{
    RequireNotNull (cString);
    // Horrible, but temporarily OK impl
    for (const char16_t* i = cString; *i != '\0'; ++i) {
        Append (*i);
    }
#if     qDebug
    _ConstGetRep (); // just make sure non-null and right type
#endif
}

String::String (const char32_t* cString)
    : inherited (mkEmpty_ ())
{
    RequireNotNull (cString);
    // Horrible, but temporarily OK impl
    for (const char32_t* i = cString; *i != '\0'; ++i) {
        Append (*i);
    }
#if     qDebug
    _ConstGetRep (); // just make sure non-null and right type
#endif
}

namespace {
    wstring mkWS_ (const Traversal::Iterable<Character>& src)
    {
        wstring r;
        for (auto i : src) {
            Containers::ReserveSpeedTweekAdd1 (r);
            r.push_back (i.As<wchar_t> ());
        }
        return r;
    }
}

String::String (const Iterable<Character>& src)
    : String (mkWS_ (src))      // @todo SLOPPY INEFFICIENT IMPLEMENTATION!
{
}

namespace {
    wstring mkWS_ (const Character& src)
    {
        wchar_t c = src.As<wchar_t> ();
        return wstring  (&c, &c + 1);
    }
}

String::String (const Character& c)
    : String (mkWS_ (c))      // @todo SLOPPY INEFFICIENT IMPLEMENTATION!
{
}

String  String::FromUTF8 (const char* from)
{
    RequireNotNull (from);
    // @todo FIX PERFORMANCE
    return String (UTF8StringToWide (from));
}

String  String::FromUTF8 (const char* from, const char* to)
{
    // @todo FIX PERFORMANCE
    wstring tmp;
    NarrowStringToWide (from, to, kCodePage_UTF8, &tmp);
    return String (move (tmp));
}

String  String::FromUTF8 (const std::string& from)
{
    // @todo FIX PERFORMANCE
    return String (UTF8StringToWide (from));
}

String  String::FromSDKString (const SDKChar* from)
{
    RequireNotNull (from);
    // @todo FIX PERFORMANCE
    return String (SDKString2Wide (from));
}

String  String::FromSDKString (const SDKChar* from, const SDKChar* to)
{
    // @todo FIX PERFORMANCE
#if     qTargetPlatformSDKUseswchar_t
    return String (from, to);
#else
    wstring tmp;
    NarrowStringToWide (from, to, GetDefaultSDKCodePage (), &tmp);
    return String (tmp);
#endif
}

String  String::FromSDKString (const basic_string<SDKChar>& from)
{
    // @todo FIX PERFORMANCE
#if     qTargetPlatformSDKUseswchar_t
    return String (from);
#else
    return String (NarrowStringToWide (from, GetDefaultSDKCodePage ()));
#endif
}

String  String::FromNarrowSDKString (const char* from)
{
    RequireNotNull (from);
    // @todo FIX PERFORMANCE
    return NarrowSDKStringToWide (from);
}

String  String::FromNarrowSDKString (const char* from, const char* to)
{
    // @todo FIX PERFORMANCE
    wstring tmp;
    NarrowStringToWide (from, to, GetDefaultSDKCodePage (), &tmp);
    return String (tmp);
}

String  String::FromNarrowSDKString (const string& from)
{
    // @todo FIX PERFORMANCE
    return NarrowSDKStringToWide (from);
}

String  String::FromNarrowString (const char* from, const char* to, const locale& l)
{
    // See http://en.cppreference.com/w/cpp/locale/codecvt/~codecvt
    using Destructible_codecvt_byname = deletable_facet_<codecvt_byname<wchar_t, char, std::mbstate_t>>;
#if     qCompilerAndStdLib_codecvtbyname_mising_string_ctor_Buggy
    Destructible_codecvt_byname cvt {l.name ().c_str () };
#else
    Destructible_codecvt_byname cvt {l.name () };
#endif

    // http://en.cppreference.com/w/cpp/locale/codecvt/in
    mbstate_t mbstate {};
    size_t externalSize = to - from;
    std::wstring resultWStr(externalSize, '\0');
    const char* from_next;
    wchar_t* to_next;
    codecvt_base::result result = cvt.in (mbstate, from, to, from_next, &resultWStr[0], &resultWStr[resultWStr.size()], to_next);
    if (result != codecvt_base::ok) {
        Execution::DoThrow (Execution::StringException (L"Error converting locale multibyte string to UNICODE"));
    }
    resultWStr.resize (to_next - &resultWStr[0]);
    return resultWStr;
}

String  String::FromAscii (const char* from)
{
    RequireNotNull (from);
    // @todo FIX PERFORMANCE
#if     qDebug
    for (auto i = from; i != from; ++i) {
        Require (isascii (*i));
    }
#endif
    return ASCIIStringToWide (from);
}

String  String::FromAscii (const string& from)
{
    // @todo FIX PERFORMANCE
#if     qDebug
    for (auto i = from.begin (); i != from.end (); ++i) {
        Require (isascii (*i));
    }
#endif
    return ASCIIStringToWide (from);
}

String::_SharedPtrIRep  String::mkEmpty_ ()
{
    static  _SharedPtrIRep s_   =   mk_ (nullptr, nullptr, 1);  // so room for NUL-char
    return s_;
}

String::_SharedPtrIRep  String::mk_ (const wchar_t* start, const wchar_t* end)
{
    return MakeSharedPtr<String_BufferedArray_Rep_> (start, end);
}

String::_SharedPtrIRep  String::mk_ (const wchar_t* start, const wchar_t* end, size_t reserveLen)
{
    return MakeSharedPtr<String_BufferedArray_Rep_> (start, end, reserveLen);
}

String::_SharedPtrIRep  String::mk_ (const wchar_t* start1, const wchar_t* end1, const wchar_t* start2, const wchar_t* end2)
{
    size_t  len1        =   end1 - start1;
    size_t  totalLen    =   len1 + (end2 - start2);
    auto sRep = MakeSharedPtr<String_BufferedArray_Rep_> (start1, end1, totalLen);
    sRep->InsertAt (reinterpret_cast<const Character*> (start2), reinterpret_cast<const Character*> (end2), len1);
    return sRep;
}

String  String::Concatenate (const String& rhs) const
{
    _SafeReadRepAccessor    thisAccessor { this };
    pair<const Character*, const Character*> lhsD   =   thisAccessor._ConstGetRep ().GetData ();
    size_t  lhsLen  =   lhsD.second - lhsD.first;
    if (lhsLen == 0) {
        return rhs;
    }
    _SafeReadRepAccessor    rhsAccessor { &rhs };
    pair<const Character*, const Character*> rhsD   =   rhsAccessor._ConstGetRep ().GetData ();
    if (rhsD.first == rhsD.second) {
        return *this;
    }
    return String (
               mk_ (
                   reinterpret_cast<const wchar_t*> (lhsD.first), reinterpret_cast<const wchar_t*> (lhsD.second),
                   reinterpret_cast<const wchar_t*> (rhsD.first), reinterpret_cast<const wchar_t*> (rhsD.second)
               )
           );
}

String  String::Concatenate (const wchar_t* appendageCStr) const
{
    RequireNotNull (appendageCStr);
    _SafeReadRepAccessor    thisAccessor { this };
    pair<const Character*, const Character*> lhsD   =   thisAccessor._ConstGetRep ().GetData ();
    size_t  lhsLen  =   lhsD.second - lhsD.first;
    if (lhsLen == 0) {
        return String (appendageCStr);
    }
    return String (
               mk_ (
                   reinterpret_cast<const wchar_t*> (lhsD.first), reinterpret_cast<const wchar_t*> (lhsD.second),
                   appendageCStr, appendageCStr + ::wcslen (appendageCStr)
               )
           );
}

void    String::SetCharAt (Character c, size_t i)
{
    // @Todo - redo with check if char is acttually chanigng and if so use
    // mk/4 4 arg string maker instead.??? Or some such...
    Require (i >= 0);
    Require (i < GetLength ());
    // Expensive, but you can use StringBuilder directly to avoid the performance costs
    StringBuilder sb { *this };
    Require (i < GetLength ());
    sb.SetAt (c, i);
    *this = sb.str ();
}

String    String::InsertAt (const Character* from, const Character* to, size_t at) const
{
    Require (at >= 0);
    Require (at <= GetLength ());
    Require (from <= to);
    Require (from != nullptr or from == to);
    Require (to != nullptr or from == to);
    if (from == to) {
        return *this;
    }
    _SafeReadRepAccessor copyAccessor { this };
    pair<const Character*, const Character*> d = copyAccessor._ConstGetRep ().GetData ();
    auto sRep = MakeSharedPtr<String_BufferedArray_Rep_> (reinterpret_cast<const wchar_t*> (d.first), reinterpret_cast<const wchar_t*> (d.second), (d.second - d.first) + (to - from));
    sRep->InsertAt (from, to, at);
    return String (move (sRep));
}

String        String::RemoveAt (size_t from, size_t to) const
{
    Require (from <= to);
    Require (to <= GetLength ());
    _SafeReadRepAccessor accessor { this };
    size_t length = accessor._ConstGetRep ()._GetLength ();
    if (from == to) {
        return *this;   // @todo - fix to return accessor.AsString()
    }
    else if (from == 0) {
        return SubString (to);
    }
    else if (to == length) {
        return SubString (0, from);
    }
    else {
        pair<const Character*, const Character*> d = accessor._ConstGetRep ().GetData ();
        const wchar_t* p = reinterpret_cast<const wchar_t*> (d.first);
        return String (mk_ (p, p + from, p + to, p + length));
    }
}

String    String::Remove (Character c) const
{
    String  tmp     =   { *this };
    size_t  index   =   tmp.Find (c, CompareOptions::eWithCase);
    if (index != kBadIndex) {
        return tmp.RemoveAt (index);
    }
    return tmp;
}

size_t  String::Find (Character c, size_t startAt, CompareOptions co) const
{
    //@todo could improve performance with strength reduction
    _SafeReadRepAccessor    accessor { this };
    Require (startAt <= accessor._ConstGetRep ()._GetLength ());
    size_t length = accessor._ConstGetRep ()._GetLength ();
    switch (co) {
        case CompareOptions::eCaseInsensitive: {
                for (size_t i = startAt; i < length; i++) {
                    if (accessor._ConstGetRep ().GetAt (i).ToLowerCase () == c.ToLowerCase ()) {
                        return i;
                    }
                }
            }
            break;
        case CompareOptions::eWithCase: {
                for (size_t i = startAt; i < length; i++) {
                    if (accessor._ConstGetRep ().GetAt (i) == c) {
                        return i;
                    }
                }
            }
            break;
    }
    return kBadIndex;
}

size_t  String::Find (const String& subString, size_t startAt, CompareOptions co) const
{
    //TODO: FIX HORRIBLE PERFORMANCE!!!
    _SafeReadRepAccessor    accessor { this };
    Require (startAt <= accessor._ConstGetRep ()._GetLength ());

    size_t  subStrLen   =   subString.GetLength ();
    if (subStrLen == 0) {
        return (accessor._ConstGetRep ()._GetLength () == 0) ? kBadIndex : 0;
    }
    if (accessor._ConstGetRep ()._GetLength () < subStrLen) {
        return kBadIndex;   // important test cuz size_t is unsigned
    }

    size_t  limit       =   accessor._ConstGetRep ()._GetLength () - subStrLen;
    switch (co) {
        case CompareOptions::eCaseInsensitive: {
                for (size_t i = startAt; i <= limit; i++) {
                    for (size_t j = 0; j < subStrLen; j++) {
                        if (accessor._ConstGetRep ().GetAt (i + j).ToLowerCase () != subString[j].ToLowerCase ()) {
                            goto nogood1;
                        }
                    }
                    return i;
nogood1:
                    ;
                }
            }
            break;
        case CompareOptions::eWithCase: {
                for (size_t i = startAt; i <= limit; i++) {
                    for (size_t j = 0; j < subStrLen; j++) {
                        if (accessor._ConstGetRep ().GetAt (i + j) != subString[j]) {
                            goto nogood2;
                        }
                    }
                    return i;
nogood2:
                    ;
                }
            }
            break;
    }
    return kBadIndex;
}

#if     !qCompilerAndStdLib_regex_Buggy
pair<size_t, size_t>  String::Find (const RegularExpression& regEx, size_t startAt) const
{
    const String  threadSafeCopy  { *this };
    Require (startAt <= threadSafeCopy.GetLength ());
    Assert (startAt == 0);  // else NYI
    wstring tmp     =   threadSafeCopy.As<wstring> ();
    std::wsmatch res;
    regex_search (tmp, res, regEx.GetCompiled ());
    if (res.size () >= 1) {
        return pair<size_t, size_t> (res.position (), res.position () + res.length ());
    }
    return pair<size_t, size_t> (kBadIndex, kBadIndex);
}
#endif

vector<size_t>  String::FindEach (const String& string2SearchFor, CompareOptions co) const
{
    vector<size_t>  result;
    const String  threadSafeCopy  { *this };
    for (size_t i = threadSafeCopy.Find (string2SearchFor, 0, co); i != String::kBadIndex; i = threadSafeCopy.Find (string2SearchFor, i, co)) {
        result.push_back (i);
        i += string2SearchFor.length ();    // this cannot point past end of this string because we FOUND string2SearchFor
    }
    return result;
}

#if     !qCompilerAndStdLib_regex_Buggy
vector<pair<size_t, size_t>>  String::FindEach (const RegularExpression& regEx) const
{
    vector<pair<size_t, size_t>>  result;
#if     qCompilerAndStdLib_regex_Buggy
    AssertNotImplemented ();
#else
    //@TODO - FIX - IF we get back zero length match
    wstring         tmp { As<wstring> () };
    std::wsmatch    res;
    regex_search (tmp, res, regEx.GetCompiled ());
    size_t  nMatches = res.size ();
    result.reserve (nMatches);
    for (size_t mi = 0; mi < nMatches; ++mi) {
        size_t matchLen = res.length (mi);  // avoid populating with lots of empty matches - specail case of empty search
        if (matchLen != 0) {
            result.push_back (pair<size_t, size_t> (res.position (mi), matchLen));
        }
    }
#endif
    return result;
}
#endif

#if     !qCompilerAndStdLib_regex_Buggy
vector<RegularExpressionMatch>  String::FindEachMatch (const RegularExpression& regEx) const
{
    vector<RegularExpressionMatch>  result;
    wstring         tmp         { As<wstring> () };
    for (std::wsregex_iterator i = wsregex_iterator (tmp.begin (), tmp.end (), regEx.GetCompiled ()); i != std::wsregex_iterator (); ++i) {
        std::wsmatch    match { *i };
        Assert (match.size () != 0);
        size_t n = match.size ();
        Containers::Sequence<String>    s;
        for (size_t i = 1; i < n; ++i) {
            s.Append (match.str (i));
        }
        result.push_back (RegularExpressionMatch (match.str (0), s));
    }
    return result;
}

vector<String>  String::FindEachString (const RegularExpression& regEx) const
{
    vector<String>  result;
    wstring         tmp         { As<wstring> () };
#if 1
    for (std::wsregex_iterator i = wsregex_iterator (tmp.begin (), tmp.end (), regEx.GetCompiled ()); i != std::wsregex_iterator (); ++i) {
        result.push_back (String { i->str () });
    }
#else
    std::wsmatch    res;
    if (regex_search (tmp, res, regEx.GetCompiled ())) {
        result.reserve (res.size ());
        for (auto i : res) {
            result.push_back (String { i });
        }
    }
#endif
    return result;
}
#endif

#if 0
vector<String>  String::Find (const String& string2SearchFor, CompareOptions co) const
{
    AssertNotReached ();
    vector<String>  result;
    wstring tmp     =   As<wstring> ();
    wregex  regExp  =   wregex (string2SearchFor.As<wstring> ());
    std::wsmatch res;
    regex_search (tmp, res, regExp);
    result.reserve (res.size ());
    for (auto i = res.begin (); i != res.end (); ++i) {
        result.push_back (String (*i));
    }
    return result;
}
#endif
size_t  String::RFind (Character c) const
{
    //@todo: FIX HORRIBLE PERFORMANCE!!!
    _SafeReadRepAccessor accessor { this };
    const _IRep&    useRep = accessor._ConstGetRep ();
    size_t length = useRep._GetLength ();
    for (size_t i = length; i > 0; --i) {
        if (useRep.GetAt (i - 1) == c) {
            return i - 1;
        }
    }
    return kBadIndex;
}

size_t  String::RFind (const String& subString) const
{
    const String  threadSafeCopy  { *this };
    //@todo: FIX HORRIBLE PERFORMANCE!!!
    /*
     * Do quickie implementation, and dont worry about efficiency...
     */
    size_t  subStrLen   =   subString.GetLength ();
    if (subStrLen == 0) {
        return ((threadSafeCopy.GetLength () == 0) ? kBadIndex : threadSafeCopy.GetLength () - 1);
    }

    size_t  limit       =   threadSafeCopy.GetLength () - subStrLen + 1;
    for (size_t i = limit; i > 0; --i) {
        if (threadSafeCopy.SubString (i - 1, i - 1 + subStrLen) == subString) {
            return i - 1;
        }
    }
    return kBadIndex;
}

bool    String::StartsWith (const Character& c, CompareOptions co) const
{
    _SafeReadRepAccessor    accessor { this };
    if (accessor._ConstGetRep ()._GetLength () == 0) {
        return false;
    }
    return accessor._ConstGetRep ().GetAt (0).Compare (c, co) == 0;
}

bool    String::StartsWith (const String& subString, CompareOptions co) const
{
    _SafeReadRepAccessor    accessor { this };
    size_t  subStrLen = subString.GetLength ();
    if (subStrLen >  accessor._ConstGetRep ()._GetLength ()) {
        return false;
    }
#if     qDebug
    bool    referenceResult = (SubString (0, subString.GetLength ()).Compare (subString, co) == 0); // this check isnt threadsafe - redo
#endif
    const Character*    subStrStart = reinterpret_cast<const Character*> (subString.c_str ());
    pair<const Character*, const Character*> thisData = accessor._ConstGetRep ().GetData ();
    bool    result = (Character::Compare (thisData.first, thisData.first + subStrLen, subStrStart, subStrStart + subStrLen, co) == 0);
    Assert (result == referenceResult);
    return result;
}

bool    String::EndsWith (const Character& c, CompareOptions co) const
{
    _SafeReadRepAccessor    accessor { this };
    const _IRep&    useRep = accessor._ConstGetRep ();
    size_t  thisStrLen = useRep._GetLength ();
    if (thisStrLen == 0) {
        return false;
    }
    return useRep.GetAt (thisStrLen - 1).Compare (c, co) == 0;
}

bool    String::EndsWith (const String& subString, CompareOptions co) const
{
    _SafeReadRepAccessor    accessor { this };
    size_t      thisStrLen = accessor._ConstGetRep ()._GetLength ();
    size_t      subStrLen = subString.GetLength ();
    if (subStrLen >  thisStrLen) {
        return false;
    }
#if     qDebug
    bool    referenceResult = (SubString (thisStrLen - subStrLen, thisStrLen).Compare (subString, co) == 0);    // this check isnt threadsafe - redo
#endif
    const Character*    subStrStart = reinterpret_cast<const Character*> (subString.c_str ());
    pair<const Character*, const Character*> thisData = accessor._ConstGetRep ().GetData ();
    bool    result = (Character::Compare (thisData.first + thisStrLen - subStrLen, thisData.first + thisStrLen, subStrStart, subStrStart + subStrLen, co) == 0);
    Assert (result == referenceResult);
    return result;
}

#if     !qCompilerAndStdLib_regex_Buggy
bool    String::Match (const RegularExpression& regEx) const
{
    wstring tmp  { As<wstring> () };
    return regex_match (tmp.begin(), tmp.end(), regEx.GetCompiled ());
}
#endif

#if     !qCompilerAndStdLib_regex_Buggy
String  String::ReplaceAll (const RegularExpression& regEx, const String& with, CompareOptions co) const
{
    return String (regex_replace (As<wstring> (), regEx.GetCompiled (), with.As<wstring> ()));
}
#endif

String  String::ReplaceAll (const String& string2SearchFor, const String& with, CompareOptions co) const
{
    Require (not string2SearchFor.empty ());
    // simplistic quickie impl...
    String  result { *this };
    size_t  i   =   0;
    while ((i = result.Find (string2SearchFor, i, co)) != String::npos) {
        result = result.SubString (0, i) + with + result.SubString (i + string2SearchFor.length ());
        i += with.length ();
    }
    return result;
}

Containers::Sequence<String>  String::Tokenize (const function<bool(Character)>& isTokenSeperator, bool trim) const
{
    String  tmp { *this };    // quickie thread-safety - do diffenrtly soon with new thread safety model... -- LGP 2014-10-31
    Containers::Sequence<String>    r;
    bool    inToken = false;
    StringBuilder curToken;
    size_t  len = tmp.size ();
    for (size_t  i = 0; i != len; ++i) {
        Character c = tmp[i];
        bool    newInToken  =   not isTokenSeperator (c);
        if (inToken != newInToken) {
            if (inToken) {
                String  s { curToken.str () };
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
        String  s { curToken.str () };
        if (trim) {
            s = s.Trim ();
        }
        r.Append (s);
    }
    return r;
}

Containers::Sequence<String>  String::Tokenize (const Containers::Set<Character>& delimiters, bool trim) const
{
    /*
     *  @todo Inefficient impl, to encourage code saving. Do more efficently.
     */
    return Tokenize (
    [delimiters] (Character c) -> bool {
        return delimiters.Contains (c);
    },
    trim
           );
}

String  String::SubString_ (const _SafeReadRepAccessor& thisAccessor, size_t thisLen, size_t from, size_t to)
{
    Require (from <= to);
    Require (to <= thisLen);
    const wchar_t*  start   =   reinterpret_cast<const wchar_t*> (thisAccessor._ConstGetRep ().Peek ()) + from;
    size_t          len     =   to - from;
    const wchar_t*  end     =   start + len;
    Assert (start <= end);
    // Slightly faster to re-use pre-built empty string
    if (start == end) {
        return mkEmpty_ ();
    }
    if (len == thisLen) {
        Assert (from == 0);     // because we require from/to subrange of thisLen, so if equal, must be full range
#if     0
        // @todo SEE TODO in String.h
        // @todo - FIX - MUST RETURN thisAccessor AS STRING
#endif
    }
#if     0
    // If we are using kOptimizedForStringsLenLessOrEqualTo - so the string data gets stored in the rep anyhow, thats probably
    // faster than adding a ref to the original string object? Not too sure of this optimization
    if (len <= String_BufferedArray_Rep_::kOptimizedForStringsLenLessOrEqualTo) {
        return mk_ (start, end);
    }
#endif
#if     qStroika_Foundation_Traveral_Iterator_SafeRepAccessorIsSafe_
    // This is mostly a win if it saves a malloc() (and the copy of the start...end char data)
    return String (_SharedPtrIRep (String::MakeSharedPtr<String_Substring_::MyRep_> (thisAccessor, start, end)));
#else
    // really just to test - if qStroika_Foundation_Traveral_Iterator_SafeRepAccessorIsSafe_ not true, it doesnt bump refcount so really unsafe todo this way
    return mk_ (start, end);
#endif
}

String  String::Repeat (unsigned int count) const
{
    switch (count) {
        case 0:
            return String ();
        case 1:
            return *this;
        case 2:
            return *this + *this;
        default: {
                StringBuilder   result;
                for (unsigned int i = 0; i < count; ++i) {
                    result += *this;
                }
                return result.str ();
            }
            break;
    }
}

String  String::LTrim (bool (*shouldBeTrimmmed) (Character)) const
{
    RequireNotNull (shouldBeTrimmmed);
    // @todo - NOT ENVELOPE THREADSAFE (BUT MOSTLY OK)
    _SafeReadRepAccessor    accessor { this };
    size_t length = accessor._ConstGetRep ()._GetLength ();
    for (size_t i = 0; i < length; ++i) {
        if (not (*shouldBeTrimmmed) (accessor._ConstGetRep ().GetAt (i))) {
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

String  String::RTrim (bool (*shouldBeTrimmmed) (Character)) const
{
    // @todo - NOT THREADAFE - BUGGY - MUST USE ACCESSOR TO RECONSTRUCT WHAT WE STRING WE RETURN!!! - EVEN FOR SUBSTR
    // @todo - NOT ENVELOPE THREADSAFE (BUT MOSTLY OK)
    RequireNotNull (shouldBeTrimmmed);
    _SafeReadRepAccessor    accessor { this };
    ptrdiff_t length = accessor._ConstGetRep ()._GetLength ();
    ptrdiff_t endOfFirstTrim = length;
    for (; endOfFirstTrim != 0; --endOfFirstTrim) {
        if ((*shouldBeTrimmmed) (accessor._ConstGetRep ().GetAt (endOfFirstTrim - 1))) {
            // keep going backwards
        }
        else {
            break;
        }
    }
    if (endOfFirstTrim == 0) {
        return String ();       // all trimmed
    }
    else if (endOfFirstTrim == length) {
        return *this;           // nothing trimmed
    }
    else {
        return SubString (0, endOfFirstTrim);
    }
}

String  String::Trim (bool (*shouldBeTrimmmed) (Character)) const
{
    RequireNotNull (shouldBeTrimmmed);
    /*
     * This could be implemented more efficient, but this is simpler for now..
     */
    return LTrim (shouldBeTrimmmed).RTrim (shouldBeTrimmmed);
}

String  String::StripAll (bool (*removeCharIf) (Character)) const
{
    //@todo - fix - horribly impl..
    RequireNotNull (removeCharIf);

    // TODO: optimize special case where removeCharIf is always false
    //
    // Walk string and find first character we need to remove
    String  result  { *this };
    size_t  n   =   result.GetLength ();
    for (size_t i = 0; i < n; ++i) {
        Character   c   =   result[i];
        if ((removeCharIf) (c)) {
            // on first removal, clone part of string done so far, and start appending
            String  tmp =   result.SubString (0, i);
            // Now keep iterating IN THIS LOOP appending characters and return at the end of this loop
            i++;
            for (; i < n; ++i) {
                c   =   result[i];
                if (not (removeCharIf) (c)) {
                    tmp += c;
                }
            }
            return tmp;
        }
    }
    return *this;   // if we NEVER get removeCharIf return false, just clone this
}

String  String::ToLowerCase () const
{
    StringBuilder           result;
    _SafeReadRepAccessor    accessor { this };
    size_t                  n   =   accessor._ConstGetRep ()._GetLength ();
    bool    anyChange   =   false;
    for (size_t i = 0; i < n; ++i) {
        Character   c   =   accessor._ConstGetRep ().GetAt (i);
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

String  String::ToUpperCase () const
{
    StringBuilder           result;
    _SafeReadRepAccessor    accessor { this };
    size_t                  n   =   accessor._ConstGetRep ()._GetLength ();
    bool    anyChange   =   false;
    for (size_t i = 0; i < n; ++i) {
        Character   c   =   accessor._ConstGetRep ().GetAt (i);
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
    return not FindFirstThat ([] (Character c) -> bool { return not c.IsWhitespace (); });
}

String  String::LimitLength (size_t maxLen, bool keepLeft) const
{
#if 1
    static  const String kELIPSIS_  { String_Constant (L"\u2026") };
#else
    static  const String kELIPSIS_  { String_Constant (L"...") };
#endif
    return LimitLength (maxLen, keepLeft, kELIPSIS_);
}

String  String::LimitLength (size_t maxLen, bool keepLeft, const String& ellipsis) const
{
    String  tmp =   Trim ();
    if (tmp.length () <= maxLen) {
        return tmp;
    }
    size_t  useLen  =   maxLen;
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

void    String::AsNarrowString (const locale& l, string* into) const
{
    // See http://en.cppreference.com/w/cpp/locale/codecvt/~codecvt
    using Destructible_codecvt_byname = deletable_facet_<codecvt_byname<wchar_t, char, std::mbstate_t>>;
#if     qCompilerAndStdLib_codecvtbyname_mising_string_ctor_Buggy
    Destructible_codecvt_byname cvt {l.name ().c_str () };
#else
    Destructible_codecvt_byname cvt {l.name () };
#endif
    wstring wstr = As<wstring> ();
    // http://en.cppreference.com/w/cpp/locale/codecvt/out
    mbstate_t mbstate {};
    into->resize (wstr.size () * cvt.max_length (), '\0');
    const wchar_t* from_next;
    char* to_next;
    codecvt_base::result result = cvt.out (mbstate, &wstr[0], &wstr[wstr.size()], from_next, &(*into)[0], &(*into)[into->size()], to_next);
    if (result != codecvt_base::ok) {
        Execution::DoThrow (Execution::StringException (L"Error converting locale multibyte string to UNICODE"));
    }
    into->resize (to_next - & (*into)[0]);
}

template    <>
void    String::AsUTF8 (string* into) const
{
    RequireNotNull (into);
    _SafeReadRepAccessor    accessor { this };
    pair<const Character*, const Character*> lhsD   =   accessor._ConstGetRep ().GetData ();
    WideStringToNarrow (reinterpret_cast<const wchar_t*> (lhsD.first), reinterpret_cast<const wchar_t*> (lhsD.second), kCodePage_UTF8, into);
}

void    String::AsSDKString (SDKString* into) const
{
    RequireNotNull (into);
    _SafeReadRepAccessor    accessor { this };
    pair<const Character*, const Character*> lhsD   =   accessor._ConstGetRep ().GetData ();
#if     qTargetPlatformSDKUseswchar_t
    into->assign (reinterpret_cast<const wchar_t*> (lhsD.first), reinterpret_cast<const wchar_t*> (lhsD.second));
#else
    WideStringToNarrow (reinterpret_cast<const wchar_t*> (lhsD.first), reinterpret_cast<const wchar_t*> (lhsD.second), GetDefaultSDKCodePage (), into);
#endif
}

void    String::AsNarrowSDKString (string* into) const
{
    RequireNotNull (into);
    _SafeReadRepAccessor    accessor { this };
    pair<const Character*, const Character*> lhsD   =   accessor._ConstGetRep ().GetData ();
    WideStringToNarrow (reinterpret_cast<const wchar_t*> (lhsD.first), reinterpret_cast<const wchar_t*> (lhsD.second), GetDefaultSDKCodePage (), into);
}

template    <>
void    String::AsASCII (string* into) const
{
    RequireNotNull (into);
    into->clear ();
    _SafeReadRepAccessor    accessor { this };
    size_t                  len =   accessor._ConstGetRep ().GetLength ();
    into->reserve (len);
    for (size_t i = 0; i < len; ++i) {
        Assert (accessor._ConstGetRep ().GetAt (i).IsASCII ());
        into->push_back (static_cast<char> (accessor._ConstGetRep ().GetAt (i).GetCharacterCode ()));
    }
}

const wchar_t*  String::c_str_ () const
{
    /*
     *  NOTE: This function is INTRINSICALLY un-threadsafe, so don't even bother to try with threadsafety.
     *  Access to this envelope MUST be externally synchronized or the returned bare pointer is doo-doo.
     */
    const   wchar_t*    result = ConstGetRep_ ().c_str_peek ();
    if (result == nullptr) {
        pair<const Character*, const Character*> d = ConstGetRep_ ().GetData ();
        _SharedPtrIRep tmp = mk_(reinterpret_cast<const wchar_t*> (d.first), reinterpret_cast<const wchar_t*> (d.second), d.second - d.first + 1);
        String* REALTHIS    =   const_cast<String*> (this);
        *REALTHIS = String (move (tmp));
        result = ConstGetRep_ ().c_str_peek ();
    }
    Ensure (result != nullptr);
    return result;
}

void    String::erase (size_t from, size_t count)
{
    // @todo - NOT ENVELOPE THREADSAFE
    // TODO: Double check STL definition - but I think they allow for count to be 'too much' - and silently trim to end...
    size_t  max2Erase    =   static_cast<size_t> (max (static_cast<ptrdiff_t> (0), static_cast<ptrdiff_t> (GetLength ()) - static_cast<ptrdiff_t> (from)));
    if (count == kBadIndex) {
        *this = RemoveAt (from, GetLength ());
    }
    else {
        *this = RemoveAt (from,  from + min (count, max2Erase));
    }
}





/*
 ********************************************************************************
 *********************************** operator+ **********************************
 ********************************************************************************
 */
String  Characters::operator+ (const wchar_t* lhs, const String& rhs)
{
    RequireNotNull (lhs);
    String::_SafeReadRepAccessor    rhsAccessor { &rhs };
    pair<const Character*, const Character*> rhsD   =   rhsAccessor._ConstGetRep ().GetData ();
    size_t  lhsLen      =   ::wcslen (lhs);
    size_t  totalLen    =   lhsLen + (rhsD.second - rhsD.first);
    auto    sRep =  String::MakeSharedPtr<String_BufferedArray_Rep_> (reinterpret_cast<const wchar_t*> (lhs), reinterpret_cast<const wchar_t*> (lhs + lhsLen), totalLen);
    sRep->InsertAt (rhsD.first, rhsD.second, lhsLen);
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
