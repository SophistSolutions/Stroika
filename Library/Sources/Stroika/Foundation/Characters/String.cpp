/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
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

#include    "../Characters/CString/Utilities.h"
#include    "../Containers/Common.h"
#include    "../Execution/Exceptions.h"
#include    "../Execution/StringException.h"
#include    "../Memory/Common.h"
#include    "../Memory/BlockAllocated.h"

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
        String_BufferedArray_Rep_ (const wchar_t* start, const wchar_t* end)
            : inherited (start, end)
        {
        }
        String_BufferedArray_Rep_ (const wchar_t* start, const wchar_t* end, size_t reserveExtraCharacters)
            : inherited (start, end, reserveExtraCharacters)
        {
        }
        virtual  _IterableSharedPtrIRep   Clone (IteratorOwnerID forIterableEnvelope) const override
        {
            AssertNotReached ();    // Since Strings now immutable, this should never be called
            // Because of 'Design Choice - Iterable<T> / Iterator<T> behavior' in String class docs - we
            // ignore suggested IteratorOwnerID
            return String::MakeSharedPtr<String_BufferedArray_Rep_> (_fStart, _fEnd);
        }
#if     qStroika_Foundation_Traveral_IterableUsesStroikaSharedPtr
    public:
        DECLARE_USE_BLOCK_ALLOCATION(String_BufferedArray_Rep_);
#endif
    };
}












#if 0
//cannot do (except for special case at end) as of Stroika v2.0a148 - because we enforce that strings pre-nul-terminated for thread safety reasons (so cannot
// do in const c_str() method - and have unsynchonzied).
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
#endif





namespace {
    template    <typename FACET>
    struct  deletable_facet_ : FACET {
        template    <typename ...Args>
        deletable_facet_ (Args&& ...args) : FACET (std::forward<Args> (args)...) {}
        ~deletable_facet_() {}
    };
}







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






#if     qDebug
namespace {
    struct ModuleAssert_ {
        inline  ModuleAssert_ ()
        {
            Assert (String::kBadIndex == wstring::npos);
        }
    };
    const   ModuleAssert_   kModuleAssert_;
}
#endif








/*
 ********************************************************************************
 ************************************* String ***********************************
 ********************************************************************************
 */
static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
constexpr   size_t    String::kBadIndex;

String::String (const char16_t* cString)
    : String (cString, cString + Characters::CString::Length (cString))
{
    RequireNotNull (cString);
}

String::String (const char32_t* cString)
    : String (cString, cString + CString::Length (cString))
{
}

String::String (const char16_t* from, const char16_t* to)
    : inherited (mkEmpty_ ())
{
    Require ((from == nullptr) == (to == nullptr));
    Require (from <= to);
    // @todo fix for if char16_t != wchar_t
    // Horrible, but temporarily OK impl
    for (const char16_t* i = from; i != to; ++i) {
        Append (*i);
    }
    _AssertRepValidType (); // just make sure non-null and right type
}

String::String (const char32_t* from, const char32_t* to)
    : inherited (mkEmpty_ ())
{
    Require ((from == nullptr) == (to == nullptr));
    Require (from <= to);
    // @todo fix for if char16_t != wchar_t
    // Horrible, but temporarily OK impl
    for (const char32_t* i = from; i != to; ++i) {
        Append (*i);
    }
    _AssertRepValidType (); // just make sure non-null and right type
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
    Destructible_codecvt_byname cvt { l.name ().c_str () };
#else
    Destructible_codecvt_byname cvt { l.name () };
#endif

    // http://en.cppreference.com/w/cpp/locale/codecvt/in
    mbstate_t mbstate {};
    size_t externalSize = to - from;
    std::wstring resultWStr(externalSize, '\0');
    const char* from_next;
    wchar_t* to_next;
    codecvt_base::result result = cvt.in (mbstate, from, to, from_next, &resultWStr[0], &resultWStr[resultWStr.size()], to_next);
    if (result != codecvt_base::ok) {
        Execution::Throw (Execution::StringException (L"Error converting locale multibyte string to UNICODE"));
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
    static  constexpr   wchar_t         kEmptyStr_[1] = {};
    static  const       _SharedPtrIRep  s_   =   mk_ (std::begin (kEmptyStr_), std::begin (kEmptyStr_));
    return s_;
}

String::_SharedPtrIRep  String::mk_ (const wchar_t* start, const wchar_t* end)
{
    RequireNotNull (start);
    RequireNotNull (end);
    Require (start <= end);
    return MakeSharedPtr<String_BufferedArray_Rep_> (start, end);
}

String::_SharedPtrIRep  String::mk_ (const wchar_t* start1, const wchar_t* end1, const wchar_t* start2, const wchar_t* end2)
{
    size_t  len1        =   end1 - start1;
    auto sRep = MakeSharedPtr<String_BufferedArray_Rep_> (start1, end1, (end2 - start2));
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
    auto sRep = MakeSharedPtr<String_BufferedArray_Rep_> (reinterpret_cast<const wchar_t*> (d.first), reinterpret_cast<const wchar_t*> (d.second), (to - from));
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
    return result;
}
#endif

#if     !qCompilerAndStdLib_regex_Buggy
vector<String>  String::FindEachString (const RegularExpression& regEx) const
{
    vector<String>  result;
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
    return false;
}
#endif

#if     !qCompilerAndStdLib_regex_Buggy
String  String::ReplaceAll (const RegularExpression& regEx, const String& with, CompareOptions co) const
{
    return String{};
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

String  String::FilteredString (const Iterable<Character>& badCharacters, Memory::Optional<Character> replacement) const
{
    StringBuilder   sb;
    for (Character i : *this) {
        // @todo Slow impl - generate set from 'badCharacters'
        if (badCharacters.Contains (i)) {
            if (replacement) {
                sb += *replacement;
            }
        }
        else {
            sb += i;
        }
    }
    return sb.str ();;
}

String  String::FilteredString (const RegularExpression& badCharacters, Memory::Optional<Character> replacement) const
{
    AssertNotImplemented ();
    return String {};
}

String  String::FilteredString (const function<bool(Character)>& badCharacterP, Memory::Optional<Character> replacement) const
{
    StringBuilder   sb;
    for (Character i : *this) {
        if (badCharacterP (i)) {
            if (replacement) {
                sb += *replacement;
            }
        }
        else {
            sb += i;
        }
    }
    return sb.str ();;
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
    return mk_ (start, end);
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
        Execution::Throw (Execution::StringException (L"Error converting locale multibyte string to UNICODE"));
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

void    String::erase (size_t from, size_t count)
{
    // https://stroika.atlassian.net/browse/STK-445
    // @todo - NOT ENVELOPE THREADSAFE
    // MUST ACQUIRE ACCESSOR HERE - not just that RemoteAt threadsafe - but must SYNC at this point - need AssureExternallySycnonized stuff here!!!
    //
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



/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Containers/Common.h"

#include    "Character.h"


using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;



namespace {
    inline  int Compare_CS_ (const Character* lhsStart, const Character* lhsEnd, const Character* rhsStart, const Character* rhsEnd)
    {
        // TODO: Need a more efficient implementation - but this should do for starters...
        Assert (lhsStart <= lhsEnd);
        size_t lLen = (lhsEnd - lhsStart);
        size_t rLen = (rhsEnd - rhsStart);
        size_t length = min (lLen, rLen);
        for (size_t i = 0; i < length; i++) {
            if (lhsStart[i].GetCharacterCode () != rhsStart[i].GetCharacterCode ()) {
                return (lhsStart[i].GetCharacterCode () - rhsStart[i].GetCharacterCode ());
            }
        }
        return Containers::CompareResultNormalizeHelper<ptrdiff_t, int> (static_cast<ptrdiff_t> (lLen) - static_cast<ptrdiff_t> (rLen));
    }

    inline  int Compare_CI_ (const Character* lhsStart, const Character* lhsEnd, const Character* rhsStart, const Character* rhsEnd)
    {
        // TODO: Need a more efficient implementation - but this should do for starters...
        Assert (lhsStart <= lhsEnd);
        // Not sure wcscasecmp even helps because of convert to c-str
        //return ::wcscasecmp (l.c_str (), r.c_str ());;
        size_t lLen = (lhsEnd - lhsStart);
        size_t rLen = (rhsEnd - rhsStart);
        size_t length = min (lLen, rLen);
        for (size_t i = 0; i < length; i++) {
            Character   lc = lhsStart[i].ToLowerCase ();
            Character   rc = rhsStart[i].ToLowerCase ();
            if (lc.GetCharacterCode () != rc.GetCharacterCode ()) {
                return (lc.GetCharacterCode () - rc.GetCharacterCode ());
            }
        }
        return Containers::CompareResultNormalizeHelper<ptrdiff_t, int> (static_cast<ptrdiff_t> (lLen) - static_cast<ptrdiff_t> (rLen));
    }
}

int     Character::Compare (const Character* lhsStart, const Character* lhsEnd, const Character* rhsStart, const Character* rhsEnd, CompareOptions co)
{
    Require (co == CompareOptions::eWithCase or co == CompareOptions::eCaseInsensitive);
    Require (lhsStart <= lhsEnd);
    Require (rhsStart <= rhsEnd);
    switch (co) {
        case    CompareOptions::eWithCase:
            return Compare_CS_ (lhsStart, lhsEnd, rhsStart, rhsEnd);
        case    CompareOptions::eCaseInsensitive:
            return Compare_CI_ (lhsStart, lhsEnd, rhsStart, rhsEnd);
        default:
            AssertNotReached ();
            return 0;
    }
}
/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Execution/StringException.h"

#include    "String_Constant.h"

#include    "ToString.h"

using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;




/*
 ********************************************************************************
 *********************** Private_::ToString_exception_ptr ***********************
 ********************************************************************************
 */
String  Private_::ToString_exception_ptr (const exception_ptr& e)
{
    static  const   String_Constant kExceptPefix_   {  L"Exception: " };
    static  const   String_Constant kUnknown_       {  L"Unknown Exception" };
    try {
        std::rethrow_exception (e);
    }
    catch (const Execution::StringException& e) {
        return kExceptPefix_ + e.As<String> ();
    }
    catch (const std::exception& e) {
        return kExceptPefix_ + String::FromNarrowSDKString (e.what ());
    }
    catch (...) {
        // fall through
    }
    return kUnknown_;
}
/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <algorithm>
#include    <mutex>
#include    <set>

#include    "../Characters/CString/Utilities.h"
#include    "../Configuration/Common.h"
#include    "../Containers/Common.h"
#include    "../Execution/Common.h"
#include    "../Execution/Exceptions.h"
#include    "../Memory/SmallStackBuffer.h"

#include    "CodePage.h"


using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Memory;

using   Execution::make_unique_lock;





#ifndef qBuildInTableDrivenCodePageBuilderProc
#define qBuildInTableDrivenCodePageBuilderProc      0
#endif


/*
 *  Hack to build 'myiswalpha' and 'myiswpunct' for SPR#1220 (revised and moved here for SPR#1306 and class
 *  'CharacterProperties').
 */
#ifndef qBuildMemoizedISXXXBuilderProc
#define qBuildMemoizedISXXXBuilderProc              0
#endif

/*
 *  Use this to test my IsWXXX functions produce the right results. Only test under WinXP,
 *  since that is the reference they are copying (SPR#1229).
 */
#ifndef qTestMyISWXXXFunctions
#define qTestMyISWXXXFunctions                      0
#endif









#if     qBuildInTableDrivenCodePageBuilderProc || qBuildMemoizedISXXXBuilderProc
#include    <fstream>
#endif













using   namespace   Characters;



#if     qPlatform_Windows
namespace   {
    inline  const wchar_t*  SAFE_WIN_WCHART_CAST_ (const char16_t* t)       { return reinterpret_cast<const wchar_t*> (t); }
    inline  wchar_t*        SAFE_WIN_WCHART_CAST_ (char16_t* t)             { return reinterpret_cast<wchar_t*> (t); }
    inline  const char16_t* SAFE_WIN_WCHART_CAST_ (const wchar_t* t)        { return reinterpret_cast<const char16_t*> (t); }
    inline  char16_t*       SAFE_WIN_WCHART_CAST_ (wchar_t* t)              { return reinterpret_cast<char16_t*> (t); }
}
#endif







/*
 ********************************************************************************
 **************************** Characters::GetCharsetString **********************
 ********************************************************************************
 */
wstring Characters::GetCharsetString (CodePage cp)
{
    switch (cp) {
        case        kCodePage_UTF7:
            return L"UTF-7";
        case        kCodePage_UTF8:
            return L"UTF-8";
        default:
            return Characters::CString::Format (L"CodePage %d", cp);
    }
}










namespace   {
    template    <CodePage CODEPAGE>
    class   TableDrivenCodePageConverter_ {
    public:
        static  void    MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt);
        static  void    MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt);
    };


    template<>
    class   TableDrivenCodePageConverter_<kCodePage_ANSI> {
    public:
        static  void    MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt);
        static  void    MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt);
    private:
        static  const   char16_t    kMap[256];
    };
    template<>
    class   TableDrivenCodePageConverter_<kCodePage_MAC> {
    public:
        static  void    MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt);
        static  void    MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt);
    private:
        static  const   char16_t    kMap[256];
    };
    template<>
    class   TableDrivenCodePageConverter_<kCodePage_PC> {
    public:
        static  void    MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt);
        static  void    MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt);
    private:
        static  const   char16_t    kMap[256];
    };
    template<>
    class   TableDrivenCodePageConverter_<kCodePage_PCA> {
    public:
        static  void    MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt);
        static  void    MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt);
    private:
        static  const   char16_t    kMap[256];
    };
    template<>
    class   TableDrivenCodePageConverter_<kCodePage_GREEK> {
    public:
        static  void    MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt);
        static  void    MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt);
    private:
        static  const   char16_t    kMap[256];
    };
    template<>
    class   TableDrivenCodePageConverter_<kCodePage_Turkish> {
    public:
        static  void    MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt);
        static  void    MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt);
    private:
        static  const   char16_t    kMap[256];
    };
    template<>
    class   TableDrivenCodePageConverter_<kCodePage_HEBREW> {
    public:
        static  void    MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt);
        static  void    MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt);
    private:
        static  const   char16_t    kMap[256];
    };
    template<>
    class   TableDrivenCodePageConverter_<kCodePage_ARABIC> {
    public:
        static  void    MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt);
        static  void    MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt);
    private:
        static  const   char16_t    kMap[256];
    };
}




namespace {

#if     qBuildInTableDrivenCodePageBuilderProc
    static  void    WriteCodePageTable (CodePage codePage);

    struct  DoRunIt {
        DoRunIt ()
        {
            //WriteCodePageTable (kCodePage_ANSI);
            //WriteCodePageTable (kCodePage_MAC);
            //WriteCodePageTable (kCodePage_PC);
            //WriteCodePageTable (kCodePage_PCA);
            //WriteCodePageTable (kCodePage_GREEK);
            //WriteCodePageTable (kCodePage_Turkish);
            //WriteCodePageTable (kCodePage_HEBREW);
            //WriteCodePageTable (kCodePage_ARABIC);
        }
    }   gRunIt;
#endif


#if     qBuildMemoizedISXXXBuilderProc
    template    <class FUNCTION>
    void    WriteMemoizedIsXXXProc (FUNCTION function, const string& origFunctionName, const string& functionName)
    {
        ofstream    outStream ("IsXXXProc.txt");

        outStream << "bool	" << functionName << " (wchar_t c)\n";
        outStream << "{\n";

        outStream << "\t// ********** " << functionName << " (AUTOGENERATED memoize of " << origFunctionName << " - " << __DATE__ << ") ***********\n";
        outStream << "\t// Hack for SPR#1220 and SPR#1306\n";

        // SPR#1308 - the generated if/then skipchain can be somewhat long, and therefore inefficient.
        // This is needlessly so. In principle - we could break up the long skipchain into many smaller ones
        // nested hierarchically. This would involve first pre-computing the entire list of regions, and
        // then generating the if/then code recursively since before you can generate INNER code you need
        // to know the full range of codepoints contained in all contained ifs.
        //
        // Anyhow - there is a trivial implementation that gets us most of the speedup we seek - since most
        // characters looked up fall in the 0..256 range. So - just handle that specially.
        //

        const   wchar_t kBoundaryForSpecailLoop1    =   255;
        const   wchar_t kBoundaryForSpecailLoop2    =   5000;

        for (int j = 0; j <= 2; ++j) {

            if (j == 0) {
                outStream << "\tif (c < " << int (kBoundaryForSpecailLoop1) << ") {\n";
            }
            else if (j == 1) {
                outStream << "\telse if (c < " << int (kBoundaryForSpecailLoop2) << ") {\n";
            }
            else {
                outStream << "\telse {\n";
            }

            outStream << "\t\tif (";
            bool    firstTime   =   true;
            bool    hasLastTrue =   false;
            size_t  firstRangeIdxTrue   =   0;
            size_t  startLoop           =   0;
            size_t  endLoop             =   0;
            if (j == 0) {
                endLoop = kBoundaryForSpecailLoop1;
            }
            else if (j == 1) {
                startLoop = kBoundaryForSpecailLoop1;
                endLoop = kBoundaryForSpecailLoop2;
            }
            else {
                startLoop = kBoundaryForSpecailLoop2;
                endLoop = 256 * 256;
            }
            for (size_t i = startLoop; i < endLoop; ++i) {
                bool    isT     =   function (static_cast<wchar_t> (i));

                if (((not isT) or (i + 1 == endLoop))  and hasLastTrue) {
                    // then emit the range...
                    if (not firstTime) {
                        outStream << "\t\t\t||";
                    }
                    size_t  rangeEnd    =   isT ? i : i - 1;
                    outStream << "(" << firstRangeIdxTrue << " <= c && c <= " << rangeEnd << ")";
                    firstTime = false;
                    outStream << "\n";
                    hasLastTrue = false;
                }
                if (isT and not hasLastTrue) {
                    firstRangeIdxTrue = i;
                    hasLastTrue = true;
                }
            }
            if (firstTime) {
                outStream << "false";
            }

            outStream << "\t\t\t) {\n";
            outStream << "\t\t\t#if		qTestMyISWXXXFunctions\n";
            outStream << "\t\t\tAssert (" << origFunctionName << "(c));\n";
            outStream << "\t\t\t#endif\n";
            outStream << "\t\t\treturn true;\n";
            outStream << "\t\t}\n";

            outStream << "\t}\n";

        }
        outStream << "\t#if		qTestMyISWXXXFunctions\n";
        outStream << "\tAssert (!" << origFunctionName << "(c));\n";
        outStream << "\t#endif\n";
        outStream << "\treturn false;\n";
        outStream << "}\n";
        outStream << "\n";
    }

    struct  DoRunIt {
        DoRunIt ()
        {
            WriteMemoizedIsXXXProc (iswalpha, "iswalpha", "CharacterProperties::IsAlpha_M");
            //WriteMemoizedIsXXXProc (iswalnum, "iswalnum", "CharacterProperties::Ialnum_M");
            //WriteMemoizedIsXXXProc (iswpunct, "iswpunct", "CharacterProperties::IsPunct_M");
            //WriteMemoizedIsXXXProc (iswspace, "iswspace", "CharacterProperties::IsSpace_M");
            //WriteMemoizedIsXXXProc (iswcntrl, "iswcntrl", "CharacterProperties::IsCntrl_M");
            //WriteMemoizedIsXXXProc (iswdigit, "iswdigit", "CharacterProperties::IsDigit_M");
        }
    }   gRunIt;
#endif


#if     qTestMyISWXXXFunctions
    class   MyIsWXXXTesterFunctions {
    public:
        MyIsWXXXTesterFunctions ()
        {
            for (wchar_t c = 0; c < 0xffff; ++c) {
                Assert (CharacterProperties::IsAlpha_M (c) == !!iswalpha (c));
                Assert (CharacterProperties::IsAlnum_M (c) == !!iswalnum (c));
                Assert (CharacterProperties::IsPunct_M (c) == !!iswpunct (c));
                Assert (CharacterProperties::IsSpace_M (c) == !!iswspace (c));
                Assert (CharacterProperties::IsCntrl_M (c) == !!iswcntrl (c));
                Assert (CharacterProperties::IsDigit_M (c) == !!iswdigit (c));
            }
        }
    }   sMyIsWXXXTesterFunctions;
#endif

}




/*
 ********************************************************************************
 ************* Characters::MapSBUnicodeTextWithMaybeBOMToUNICODE ****************
 ********************************************************************************
 */
void    Characters::MapSBUnicodeTextWithMaybeBOMToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt)
{
    RequireNotNull (outChars);
    RequireNotNull (outCharCnt);
    size_t                          outBufSize  =   *outCharCnt;
    CodePagesGuesser::Confidence    confidence  =   CodePagesGuesser::Confidence::eLow;
    CodePage                        cp          =   CodePagesGuesser ().Guess (inMBChars, inMBCharCnt, &confidence, nullptr);
    if (confidence <= CodePagesGuesser::Confidence::eLow) {
        cp = kCodePage_UTF8;
    }
    CodePageConverter cpCvt (cp, CodePageConverter::eHandleBOM);
    cpCvt.MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt);
    Ensure (*outCharCnt <= outBufSize);
}







/*
 ********************************************************************************
 ******************************** CodePageConverter *****************************
 ********************************************************************************
 */
size_t  CodePageConverter::MapFromUNICODE_QuickComputeOutBufSize (const wchar_t* inChars, size_t inCharCnt) const
{
    size_t  resultSize;
    switch (fCodePage) {
        case    kCodePage_ANSI:
            resultSize = inCharCnt * 1;
            break;
        case    kCodePage_MAC:
            resultSize = inCharCnt * 1;
            break;
        case    kCodePage_PC:
            resultSize = inCharCnt * 1;
            break;
        case    kCodePage_PCA:
            resultSize = inCharCnt * 1;
            break;
        case    kCodePage_SJIS:
            resultSize = inCharCnt * 2;
            break;
        case    kCodePage_UTF7:
            resultSize = inCharCnt * 6;
            break;  // ITHINK thats right... BOM appears to be 5 chars long? LGP 2001-09-11
        case    kCodePage_UTF8:
            resultSize = UTF8Converter ().MapFromUNICODE_QuickComputeOutBufSize (inChars, inCharCnt);
        default:
            resultSize = inCharCnt * 8;
            break;  // I THINK that should always be enough - but who knows...
    }
    if (GetHandleBOM ()) {
        switch (fCodePage) {
            case    kCodePage_UNICODE_WIDE:
            case    kCodePage_UNICODE_WIDE_BIGENDIAN: {
                    // BOM (byte order mark)
                    resultSize += 2;
                }
                break;
            case    kCodePage_UTF7: {
                    resultSize += 5;    // for BOM
                }
                break;
            case    kCodePage_UTF8: {
                    resultSize += 3;    // BOM (byte order mark)
                }
        }
    }
    return resultSize;
}

void    CodePageConverter::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt) const
{
    Require (inMBCharCnt == 0 or inMBChars != nullptr);
    RequireNotNull (outCharCnt);
    Require (*outCharCnt == 0 or outChars != nullptr);

    if (GetHandleBOM ()) {
        size_t          bytesToStrip    =   0;
        if (CodePagesGuesser ().Guess (inMBChars, inMBCharCnt, nullptr, &bytesToStrip) == fCodePage) {
            Assert (inMBCharCnt >= bytesToStrip);
            inMBChars += bytesToStrip;
            inMBCharCnt -= bytesToStrip;
        }
    }
    switch (fCodePage) {
        case    kCodePage_ANSI:
            TableDrivenCodePageConverter_<kCodePage_ANSI>::MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt);
            break;
        case    kCodePage_MAC:
            TableDrivenCodePageConverter_<kCodePage_MAC>::MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt);
            break;
        case    kCodePage_PC:
            TableDrivenCodePageConverter_<kCodePage_PC>::MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt);
            break;
        case    kCodePage_PCA:
            TableDrivenCodePageConverter_<kCodePage_PCA>::MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt);
            break;
        case    kCodePage_GREEK:
            TableDrivenCodePageConverter_<kCodePage_GREEK>::MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt);
            break;
        case    kCodePage_Turkish:
            TableDrivenCodePageConverter_<kCodePage_Turkish>::MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt);
            break;
        case    kCodePage_HEBREW:
            TableDrivenCodePageConverter_<kCodePage_HEBREW>::MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt);
            break;
        case    kCodePage_ARABIC:
            TableDrivenCodePageConverter_<kCodePage_ARABIC>::MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt);
            break;
        case    kCodePage_UNICODE_WIDE: {
                const wchar_t*  inWChars    =   reinterpret_cast<const wchar_t*> (inMBChars);
                size_t          inWCharCnt  =   (inMBCharCnt / sizeof (wchar_t));
                *outCharCnt = inWCharCnt;
                (void)::memcpy (outChars, inWChars, inWCharCnt * sizeof (wchar_t));
            }
            break;
        case    kCodePage_UNICODE_WIDE_BIGENDIAN: {
                const wchar_t*  inWChars    =   reinterpret_cast<const wchar_t*> (inMBChars);
                size_t          inWCharCnt  =   (inMBCharCnt / sizeof (wchar_t));
                *outCharCnt = inWCharCnt;
                for (size_t i = 0; i < inWCharCnt; ++i) {
                    wchar_t c   =   inWChars[i];
                    // byteswap
                    c = ((c & 0xff) << 8) + (c >> 8);
                    outChars[i] = c;
                }
            }
            break;
        case    kCodePage_UTF8:     {
                UTF8Converter ().MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt);
            }
            break;
        default: {
#if     qPlatform_Windows
                Characters::Platform::Windows::PlatformCodePageConverter (fCodePage).MapToUNICODE (inMBChars, inMBCharCnt, SAFE_WIN_WCHART_CAST_ (outChars), outCharCnt);
#else
                Execution::Throw (CodePageNotSupportedException (fCodePage));
#endif
            }
            break;
    }

#if     qDebug && qPlatform_Windows && 0
    // Assure my baked tables (and UTF8 converters) perform the same as the builtin Win32 API
    {
        size_t  tstCharCnt  =   *outCharCnt;
        SmallStackBuffer<wchar_t>   tstBuf (*outCharCnt);
        Characters::Platform::Windows::PlatformCodePageConverter (fCodePage).MapToUNICODE (inMBChars, inMBCharCnt, tstBuf, &tstCharCnt);
        Assert (tstCharCnt == *outCharCnt);
        Assert (memcmp (tstBuf, outChars, sizeof (wchar_t)*tstCharCnt) == 0);
    }
#endif
}

void    CodePageConverter::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char32_t* outChars, size_t* outCharCnt) const
{
// Not really right - but hopefully adquate for starters -- LGP 2011-09-06
    SmallStackBuffer<char16_t>  tmpBuf (*outCharCnt);
    MapToUNICODE (inMBChars, inMBCharCnt, tmpBuf, outCharCnt);
    for (size_t i = 0; i < *outCharCnt; ++i) {
        outChars[i] = tmpBuf[i];
    }
}

void    CodePageConverter::MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt) const
{
    Require (inCharCnt == 0 or inChars != nullptr);
    RequireNotNull (outCharCnt);
    Require (*outCharCnt == 0 or outChars != nullptr);

    size_t  outBufferSize   =   *outCharCnt;
#if     qDebug && qPlatform_Windows
    size_t  countOfBOMCharsAdded    =   0;      // just for the Windows debug check at the end
#endif

    switch (fCodePage) {
        case    kCodePage_ANSI:
            TableDrivenCodePageConverter_<kCodePage_ANSI>::MapFromUNICODE (inChars, inCharCnt, outChars, outCharCnt);
            break;
        case    kCodePage_MAC:
            TableDrivenCodePageConverter_<kCodePage_MAC>::MapFromUNICODE (inChars, inCharCnt, outChars, outCharCnt);
            break;
        case    kCodePage_PC:
            TableDrivenCodePageConverter_<kCodePage_PC>::MapFromUNICODE (inChars, inCharCnt, outChars, outCharCnt);
            break;
        case    kCodePage_PCA:
            TableDrivenCodePageConverter_<kCodePage_PCA>::MapFromUNICODE (inChars, inCharCnt, outChars, outCharCnt);
            break;
        case    kCodePage_UNICODE_WIDE: {
                if (*outCharCnt >= 2) {
                    wchar_t*    outWBytes       =   reinterpret_cast<wchar_t*> (outChars);
                    size_t      outByteCount    =   (inCharCnt * sizeof (wchar_t));
                    if (GetHandleBOM ()) {
                        outWBytes++;        // skip BOM
                        outByteCount -= 2;  // subtract for BOM
                    }
                    *outCharCnt = outByteCount;
                    if (GetHandleBOM ()) {
                        outChars[0] = '\xff';
                        outChars[1] = '\xfe';
#if     qDebug && qPlatform_Windows
                        countOfBOMCharsAdded = 2;
#endif
                    }
                    (void)::memcpy (outWBytes, inChars, inCharCnt * sizeof (wchar_t));
                }
                else {
                    *outCharCnt = 0;
                }
            }
            break;
        case    kCodePage_UNICODE_WIDE_BIGENDIAN: {
                if (*outCharCnt >= 2) {
                    wchar_t*    outWBytes       =   reinterpret_cast<wchar_t*> (outChars);
                    size_t      outByteCount    =   (inCharCnt * sizeof (wchar_t));
                    if (GetHandleBOM ()) {
                        outWBytes++;        // skip BOM
                        outByteCount -= 2;  // subtract for BOM
                    }
                    *outCharCnt = outByteCount;
                    if (GetHandleBOM ()) {
                        outChars[0] =  '\xfe';
                        outChars[1] =  '\xff';
#if     qDebug && qPlatform_Windows
                        countOfBOMCharsAdded = 2;
#endif
                    }
                    for (size_t i = 0; i < inCharCnt; ++i) {
                        wchar_t c   =   inChars[i];
                        // byteswap
                        c = ((c & 0xff) << 8) + (c >> 8);
                        outWBytes[i] = c;
                    }
                }
                else {
                    *outCharCnt = 0;
                }
            }
            break;
#if     qPlatform_Windows
        case    kCodePage_UTF7: {
                char*   useOutChars     =   outChars;
                size_t  useOutCharCount =   *outCharCnt;
                if (GetHandleBOM ()) {
                    if (*outCharCnt >= 5) {
                        useOutChars += 5;   // skip BOM
                        useOutCharCount -= 5;
                        outChars[0] = 0x2b;
                        outChars[1] = 0x2f;
                        outChars[2] = 0x76;
                        outChars[3] = 0x38;
                        outChars[4] = 0x2d;
#if     qDebug && qPlatform_Windows
                        countOfBOMCharsAdded = 5;
#endif
                    }
                    else {
                        useOutCharCount = 0;
                    }
                }
                Characters::Platform::Windows::PlatformCodePageConverter (kCodePage_UTF7).MapFromUNICODE (SAFE_WIN_WCHART_CAST_ (inChars), inCharCnt, useOutChars, &useOutCharCount);
                if (GetHandleBOM ()) {
                    if (*outCharCnt >= 5) {
                        useOutCharCount += 5;
                    }
                }
                *outCharCnt = useOutCharCount;
            }
            break;
#endif
        case    kCodePage_UTF8: {
                char*   useOutChars     =   outChars;
                size_t  useOutCharCount =   *outCharCnt;
                if (GetHandleBOM ()) {
                    if (*outCharCnt >= 3) {
                        useOutChars += 3;   // skip BOM
                        useOutCharCount -= 3;
                        reinterpret_cast<unsigned char*> (outChars)[0] = 0xef;
                        reinterpret_cast<unsigned char*> (outChars)[1] = 0xbb;
                        reinterpret_cast<unsigned char*> (outChars)[2] = 0xbf;
#if     qDebug && qPlatform_Windows
                        countOfBOMCharsAdded = 3;
#endif
                    }
                    else {
                        useOutCharCount = 0;
                    }
                }
                UTF8Converter ().MapFromUNICODE (inChars, inCharCnt, useOutChars, &useOutCharCount);
                if (GetHandleBOM ()) {
                    if (*outCharCnt >= 3) {
                        useOutCharCount += 3;
                    }
                }
                *outCharCnt = useOutCharCount;
            }
            break;
        default: {
#if     qPlatform_Windows
                Characters::Platform::Windows::PlatformCodePageConverter (fCodePage).MapFromUNICODE (SAFE_WIN_WCHART_CAST_ (inChars), inCharCnt, outChars, outCharCnt);
#else
                Execution::Throw (CodePageNotSupportedException (fCodePage));
#endif
            }
    }

#if     qDebug && qPlatform_Windows
    // Assure my baked tables perform the same as the builtin Win32 API
    {
        size_t                      win32TstCharCnt =   outBufferSize;
        SmallStackBuffer<char>  win32TstBuf (win32TstCharCnt);

        Characters::Platform::Windows::PlatformCodePageConverter (fCodePage).MapFromUNICODE (SAFE_WIN_WCHART_CAST_ (inChars), inCharCnt, win32TstBuf, &win32TstCharCnt);

        // SPR#0813 (and SPR#1277) - assert this produces the right result OR a '?' character -
        // used for bad conversions. Reason is cuz for characters that don't map - our table and
        // the system table can differ in how they map depending on current OS code page.
        Assert ((win32TstCharCnt + countOfBOMCharsAdded) == *outCharCnt or outChars[0] == '?');
        Assert (memcmp (win32TstBuf, outChars + countOfBOMCharsAdded, win32TstCharCnt) == 0 or outChars[0] == '?');
    }
#endif
}

void    CodePageConverter::MapFromUNICODE (const char32_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt) const
{
// Not really right - but hopefully adquate for starters -- LGP 2011-09-06
    SmallStackBuffer<char16_t>  tmpBuf (*outCharCnt);
    for (size_t i = 0; i < inCharCnt; ++i) {
        tmpBuf[i] = inChars[i];
    }
    MapFromUNICODE (tmpBuf, inCharCnt, outChars, outCharCnt);
}














/*
 ********************************************************************************
 ***************** TableDrivenCodePageConverter_<kCodePage_ANSI> *****************
 ********************************************************************************
 */
const   char16_t        TableDrivenCodePageConverter_<kCodePage_ANSI>::kMap[256] = {
    0x0,        0x1,        0x2,        0x3,        0x4,        0x5,        0x6,        0x7,
    0x8,        0x9,        0xa,        0xb,        0xc,        0xd,        0xe,        0xf,
    0x10,       0x11,       0x12,       0x13,       0x14,       0x15,       0x16,       0x17,
    0x18,       0x19,       0x1a,       0x1b,       0x1c,       0x1d,       0x1e,       0x1f,
    0x20,       0x21,       0x22,       0x23,       0x24,       0x25,       0x26,       0x27,
    0x28,       0x29,       0x2a,       0x2b,       0x2c,       0x2d,       0x2e,       0x2f,
    0x30,       0x31,       0x32,       0x33,       0x34,       0x35,       0x36,       0x37,
    0x38,       0x39,       0x3a,       0x3b,       0x3c,       0x3d,       0x3e,       0x3f,
    0x40,       0x41,       0x42,       0x43,       0x44,       0x45,       0x46,       0x47,
    0x48,       0x49,       0x4a,       0x4b,       0x4c,       0x4d,       0x4e,       0x4f,
    0x50,       0x51,       0x52,       0x53,       0x54,       0x55,       0x56,       0x57,
    0x58,       0x59,       0x5a,       0x5b,       0x5c,       0x5d,       0x5e,       0x5f,
    0x60,       0x61,       0x62,       0x63,       0x64,       0x65,       0x66,       0x67,
    0x68,       0x69,       0x6a,       0x6b,       0x6c,       0x6d,       0x6e,       0x6f,
    0x70,       0x71,       0x72,       0x73,       0x74,       0x75,       0x76,       0x77,
    0x78,       0x79,       0x7a,       0x7b,       0x7c,       0x7d,       0x7e,       0x7f,
    0x20ac,     0x81,       0x201a,     0x192,      0x201e,     0x2026,     0x2020,     0x2021,
    0x2c6,      0x2030,     0x160,      0x2039,     0x152,      0x8d,       0x17d,      0x8f,
    0x90,       0x2018,     0x2019,     0x201c,     0x201d,     0x2022,     0x2013,     0x2014,
    0x2dc,      0x2122,     0x161,      0x203a,     0x153,      0x9d,       0x17e,      0x178,
    0xa0,       0xa1,       0xa2,       0xa3,       0xa4,       0xa5,       0xa6,       0xa7,
    0xa8,       0xa9,       0xaa,       0xab,       0xac,       0xad,       0xae,       0xaf,
    0xb0,       0xb1,       0xb2,       0xb3,       0xb4,       0xb5,       0xb6,       0xb7,
    0xb8,       0xb9,       0xba,       0xbb,       0xbc,       0xbd,       0xbe,       0xbf,
    0xc0,       0xc1,       0xc2,       0xc3,       0xc4,       0xc5,       0xc6,       0xc7,
    0xc8,       0xc9,       0xca,       0xcb,       0xcc,       0xcd,       0xce,       0xcf,
    0xd0,       0xd1,       0xd2,       0xd3,       0xd4,       0xd5,       0xd6,       0xd7,
    0xd8,       0xd9,       0xda,       0xdb,       0xdc,       0xdd,       0xde,       0xdf,
    0xe0,       0xe1,       0xe2,       0xe3,       0xe4,       0xe5,       0xe6,       0xe7,
    0xe8,       0xe9,       0xea,       0xeb,       0xec,       0xed,       0xee,       0xef,
    0xf0,       0xf1,       0xf2,       0xf3,       0xf4,       0xf5,       0xf6,       0xf7,
    0xf8,       0xf9,       0xfa,       0xfb,       0xfc,       0xfd,       0xfe,       0xff,
};

void    TableDrivenCodePageConverter_<kCodePage_ANSI>::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt)
{
    size_t  nCharsToCopy    =   min (inMBCharCnt, *outCharCnt);
    for (size_t i = 0; i < nCharsToCopy; ++i) {
        outChars[i] = kMap[(unsigned char)inMBChars[i]];
    }
    *outCharCnt = nCharsToCopy;
}

void    TableDrivenCodePageConverter_<kCodePage_ANSI>::MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt)
{
    size_t  nCharsToCopy    =   min (inCharCnt, *outCharCnt);
    for (size_t i = 0; i < nCharsToCopy; ++i) {
        size_t j = 0;
        for (; j < 256; ++j) {
            if (kMap[j] == inChars[i]) {
                outChars[i] = static_cast<char> (j);
                break;
            }
        }
        if (j == 256) {
            /*Perhaps should add extra args to this function - as in Win32 API - for handling this missing-char case*/
            outChars[i] = '?';
        }
    }
    *outCharCnt = nCharsToCopy;
}










/*
 ********************************************************************************
 ******************** TableDrivenCodePageConverter_<kCodePage_MAC> **************
 ********************************************************************************
 */
const   char16_t        TableDrivenCodePageConverter_<kCodePage_MAC>::kMap[256] = {
    0x0,        0x1,        0x2,        0x3,        0x4,        0x5,        0x6,        0x7,
    0x8,        0x9,        0xa,        0xb,        0xc,        0xd,        0xe,        0xf,
    0x10,       0x11,       0x12,       0x13,       0x14,       0x15,       0x16,       0x17,
    0x18,       0x19,       0x1a,       0x1b,       0x1c,       0x1d,       0x1e,       0x1f,
    0x20,       0x21,       0x22,       0x23,       0x24,       0x25,       0x26,       0x27,
    0x28,       0x29,       0x2a,       0x2b,       0x2c,       0x2d,       0x2e,       0x2f,
    0x30,       0x31,       0x32,       0x33,       0x34,       0x35,       0x36,       0x37,
    0x38,       0x39,       0x3a,       0x3b,       0x3c,       0x3d,       0x3e,       0x3f,
    0x40,       0x41,       0x42,       0x43,       0x44,       0x45,       0x46,       0x47,
    0x48,       0x49,       0x4a,       0x4b,       0x4c,       0x4d,       0x4e,       0x4f,
    0x50,       0x51,       0x52,       0x53,       0x54,       0x55,       0x56,       0x57,
    0x58,       0x59,       0x5a,       0x5b,       0x5c,       0x5d,       0x5e,       0x5f,
    0x60,       0x61,       0x62,       0x63,       0x64,       0x65,       0x66,       0x67,
    0x68,       0x69,       0x6a,       0x6b,       0x6c,       0x6d,       0x6e,       0x6f,
    0x70,       0x71,       0x72,       0x73,       0x74,       0x75,       0x76,       0x77,
    0x78,       0x79,       0x7a,       0x7b,       0x7c,       0x7d,       0x7e,       0x7f,
    0xc4,       0xc5,       0xc7,       0xc9,       0xd1,       0xd6,       0xdc,       0xe1,
    0xe0,       0xe2,       0xe4,       0xe3,       0xe5,       0xe7,       0xe9,       0xe8,
    0xea,       0xeb,       0xed,       0xec,       0xee,       0xef,       0xf1,       0xf3,
    0xf2,       0xf4,       0xf6,       0xf5,       0xfa,       0xf9,       0xfb,       0xfc,
    0x2020,     0xb0,       0xa2,       0xa3,       0xa7,       0x2022,     0xb6,       0xdf,
    0xae,       0xa9,       0x2122,     0xb4,       0xa8,       0x2260,     0xc6,       0xd8,
    0x221e,     0xb1,       0x2264,     0x2265,     0xa5,       0xb5,       0x2202,     0x2211,
    0x220f,     0x3c0,      0x222b,     0xaa,       0xba,       0x2126,     0xe6,       0xf8,
    0xbf,       0xa1,       0xac,       0x221a,     0x192,      0x2248,     0x2206,     0xab,
    0xbb,       0x2026,     0xa0,       0xc0,       0xc3,       0xd5,       0x152,      0x153,
    0x2013,     0x2014,     0x201c,     0x201d,     0x2018,     0x2019,     0xf7,       0x25ca,
    0xff,       0x178,      0x2044,     0x20ac,     0x2039,     0x203a,     0xfb01,     0xfb02,
    0x2021,     0xb7,       0x201a,     0x201e,     0x2030,     0xc2,       0xca,       0xc1,
    0xcb,       0xc8,       0xcd,       0xce,       0xcf,       0xcc,       0xd3,       0xd4,
    0xf8ff,     0xd2,       0xda,       0xdb,       0xd9,       0x131,      0x2c6,      0x2dc,
    0xaf,       0x2d8,      0x2d9,      0x2da,      0xb8,       0x2dd,      0x2db,      0x2c7,
};

void    TableDrivenCodePageConverter_<kCodePage_MAC>::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt)
{
    size_t  nCharsToCopy    =   min (inMBCharCnt, *outCharCnt);
    for (size_t i = 0; i < nCharsToCopy; ++i) {
        outChars[i] = kMap[(unsigned char)inMBChars[i]];
    }
    *outCharCnt = nCharsToCopy;
}

void    TableDrivenCodePageConverter_<kCodePage_MAC>::MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt)
{
    size_t  nCharsToCopy    =   min (inCharCnt, *outCharCnt);
    for (size_t i = 0; i < nCharsToCopy; ++i) {
        size_t j = 0;
        for (; j < 256; ++j) {
            if (kMap[j] == inChars[i]) {
                outChars[i] = static_cast<char> (j);
                break;
            }
        }
        if (j == 256) {
            /*Perhaps should add extra args to this function - as in Win32 API - for handling this missing-char case*/
            outChars[i] = '?';
        }
    }
    *outCharCnt = nCharsToCopy;
}







/*
 ********************************************************************************
 ********************* TableDrivenCodePageConverter_<kCodePage_PC> ***************
 ********************************************************************************
 */
const   char16_t        TableDrivenCodePageConverter_<kCodePage_PC>::kMap[256] = {
    0x0,        0x1,        0x2,        0x3,        0x4,        0x5,        0x6,        0x7,
    0x8,        0x9,        0xa,        0xb,        0xc,        0xd,        0xe,        0xf,
    0x10,       0x11,       0x12,       0x13,       0x14,       0x15,       0x16,       0x17,
    0x18,       0x19,       0x1a,       0x1b,       0x1c,       0x1d,       0x1e,       0x1f,
    0x20,       0x21,       0x22,       0x23,       0x24,       0x25,       0x26,       0x27,
    0x28,       0x29,       0x2a,       0x2b,       0x2c,       0x2d,       0x2e,       0x2f,
    0x30,       0x31,       0x32,       0x33,       0x34,       0x35,       0x36,       0x37,
    0x38,       0x39,       0x3a,       0x3b,       0x3c,       0x3d,       0x3e,       0x3f,
    0x40,       0x41,       0x42,       0x43,       0x44,       0x45,       0x46,       0x47,
    0x48,       0x49,       0x4a,       0x4b,       0x4c,       0x4d,       0x4e,       0x4f,
    0x50,       0x51,       0x52,       0x53,       0x54,       0x55,       0x56,       0x57,
    0x58,       0x59,       0x5a,       0x5b,       0x5c,       0x5d,       0x5e,       0x5f,
    0x60,       0x61,       0x62,       0x63,       0x64,       0x65,       0x66,       0x67,
    0x68,       0x69,       0x6a,       0x6b,       0x6c,       0x6d,       0x6e,       0x6f,
    0x70,       0x71,       0x72,       0x73,       0x74,       0x75,       0x76,       0x77,
    0x78,       0x79,       0x7a,       0x7b,       0x7c,       0x7d,       0x7e,       0x7f,
    0xc7,       0xfc,       0xe9,       0xe2,       0xe4,       0xe0,       0xe5,       0xe7,
    0xea,       0xeb,       0xe8,       0xef,       0xee,       0xec,       0xc4,       0xc5,
    0xc9,       0xe6,       0xc6,       0xf4,       0xf6,       0xf2,       0xfb,       0xf9,
    0xff,       0xd6,       0xdc,       0xa2,       0xa3,       0xa5,       0x20a7,     0x192,
    0xe1,       0xed,       0xf3,       0xfa,       0xf1,       0xd1,       0xaa,       0xba,
    0xbf,       0x2310,     0xac,       0xbd,       0xbc,       0xa1,       0xab,       0xbb,
    0x2591,     0x2592,     0x2593,     0x2502,     0x2524,     0x2561,     0x2562,     0x2556,
    0x2555,     0x2563,     0x2551,     0x2557,     0x255d,     0x255c,     0x255b,     0x2510,
    0x2514,     0x2534,     0x252c,     0x251c,     0x2500,     0x253c,     0x255e,     0x255f,
    0x255a,     0x2554,     0x2569,     0x2566,     0x2560,     0x2550,     0x256c,     0x2567,
    0x2568,     0x2564,     0x2565,     0x2559,     0x2558,     0x2552,     0x2553,     0x256b,
    0x256a,     0x2518,     0x250c,     0x2588,     0x2584,     0x258c,     0x2590,     0x2580,
    0x3b1,      0xdf,       0x393,      0x3c0,      0x3a3,      0x3c3,      0xb5,       0x3c4,
    0x3a6,      0x398,      0x3a9,      0x3b4,      0x221e,     0x3c6,      0x3b5,      0x2229,
    0x2261,     0xb1,       0x2265,     0x2264,     0x2320,     0x2321,     0xf7,       0x2248,
    0xb0,       0x2219,     0xb7,       0x221a,     0x207f,     0xb2,       0x25a0,     0xa0,
};

void    TableDrivenCodePageConverter_<kCodePage_PC>::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt)
{
    size_t  nCharsToCopy    =   min (inMBCharCnt, *outCharCnt);
    for (size_t i = 0; i < nCharsToCopy; ++i) {
        outChars[i] = kMap[(unsigned char)inMBChars[i]];
    }
    *outCharCnt = nCharsToCopy;
}

void    TableDrivenCodePageConverter_<kCodePage_PC>::MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt)
{
    size_t  nCharsToCopy    =   min (inCharCnt, *outCharCnt);
    for (size_t i = 0; i < nCharsToCopy; ++i) {
        size_t j = 0;
        for (; j < 256; ++j) {
            if (kMap[j] == inChars[i]) {
                outChars[i] = static_cast<char> (j);
                break;
            }
        }
        if (j == 256) {
            /*Perhaps should add extra args to this function - as in Win32 API - for handling this missing-char case*/
            outChars[i] = '?';
        }
    }
    *outCharCnt = nCharsToCopy;
}









/*
 ********************************************************************************
 ******************** TableDrivenCodePageConverter_<kCodePage_PCA> **************
 ********************************************************************************
 */
const   char16_t        TableDrivenCodePageConverter_<kCodePage_PCA>::kMap[256] = {
    0x0,        0x1,        0x2,        0x3,        0x4,        0x5,        0x6,        0x7,
    0x8,        0x9,        0xa,        0xb,        0xc,        0xd,        0xe,        0xf,
    0x10,       0x11,       0x12,       0x13,       0x14,       0x15,       0x16,       0x17,
    0x18,       0x19,       0x1a,       0x1b,       0x1c,       0x1d,       0x1e,       0x1f,
    0x20,       0x21,       0x22,       0x23,       0x24,       0x25,       0x26,       0x27,
    0x28,       0x29,       0x2a,       0x2b,       0x2c,       0x2d,       0x2e,       0x2f,
    0x30,       0x31,       0x32,       0x33,       0x34,       0x35,       0x36,       0x37,
    0x38,       0x39,       0x3a,       0x3b,       0x3c,       0x3d,       0x3e,       0x3f,
    0x40,       0x41,       0x42,       0x43,       0x44,       0x45,       0x46,       0x47,
    0x48,       0x49,       0x4a,       0x4b,       0x4c,       0x4d,       0x4e,       0x4f,
    0x50,       0x51,       0x52,       0x53,       0x54,       0x55,       0x56,       0x57,
    0x58,       0x59,       0x5a,       0x5b,       0x5c,       0x5d,       0x5e,       0x5f,
    0x60,       0x61,       0x62,       0x63,       0x64,       0x65,       0x66,       0x67,
    0x68,       0x69,       0x6a,       0x6b,       0x6c,       0x6d,       0x6e,       0x6f,
    0x70,       0x71,       0x72,       0x73,       0x74,       0x75,       0x76,       0x77,
    0x78,       0x79,       0x7a,       0x7b,       0x7c,       0x7d,       0x7e,       0x7f,
    0xc7,       0xfc,       0xe9,       0xe2,       0xe4,       0xe0,       0xe5,       0xe7,
    0xea,       0xeb,       0xe8,       0xef,       0xee,       0xec,       0xc4,       0xc5,
    0xc9,       0xe6,       0xc6,       0xf4,       0xf6,       0xf2,       0xfb,       0xf9,
    0xff,       0xd6,       0xdc,       0xf8,       0xa3,       0xd8,       0xd7,       0x192,
    0xe1,       0xed,       0xf3,       0xfa,       0xf1,       0xd1,       0xaa,       0xba,
    0xbf,       0xae,       0xac,       0xbd,       0xbc,       0xa1,       0xab,       0xbb,
    0x2591,     0x2592,     0x2593,     0x2502,     0x2524,     0xc1,       0xc2,       0xc0,
    0xa9,       0x2563,     0x2551,     0x2557,     0x255d,     0xa2,       0xa5,       0x2510,
    0x2514,     0x2534,     0x252c,     0x251c,     0x2500,     0x253c,     0xe3,       0xc3,
    0x255a,     0x2554,     0x2569,     0x2566,     0x2560,     0x2550,     0x256c,     0xa4,
    0xf0,       0xd0,       0xca,       0xcb,       0xc8,       0x131,      0xcd,       0xce,
    0xcf,       0x2518,     0x250c,     0x2588,     0x2584,     0xa6,       0xcc,       0x2580,
    0xd3,       0xdf,       0xd4,       0xd2,       0xf5,       0xd5,       0xb5,       0xfe,
    0xde,       0xda,       0xdb,       0xd9,       0xfd,       0xdd,       0xaf,       0xb4,
    0xad,       0xb1,       0x2017,     0xbe,       0xb6,       0xa7,       0xf7,       0xb8,
    0xb0,       0xa8,       0xb7,       0xb9,       0xb3,       0xb2,       0x25a0,     0xa0,
};

void    TableDrivenCodePageConverter_<kCodePage_PCA>::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt)
{
    size_t  nCharsToCopy    =   min (inMBCharCnt, *outCharCnt);
    for (size_t i = 0; i < nCharsToCopy; ++i) {
        outChars[i] = kMap[(unsigned char)inMBChars[i]];
    }
    *outCharCnt = nCharsToCopy;
}

void    TableDrivenCodePageConverter_<kCodePage_PCA>::MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt)
{
    size_t  nCharsToCopy    =   min (inCharCnt, *outCharCnt);
    for (size_t i = 0; i < nCharsToCopy; ++i) {
        size_t j = 0;
        for (; j < 256; ++j) {
            if (kMap[j] == inChars[i]) {
                outChars[i] = static_cast<char> (j);
                break;
            }
        }
        if (j == 256) {
            /*Perhaps should add extra args to this function - as in Win32 API - for handling this missing-char case*/
            outChars[i] = '?';
        }
    }
    *outCharCnt = nCharsToCopy;
}








/*
 ********************************************************************************
 ******************** TableDrivenCodePageConverter_<kCodePage_GREEK> *************
 ********************************************************************************
 */
const   char16_t        TableDrivenCodePageConverter_<kCodePage_GREEK>::kMap[256] = {
    0x0,        0x1,        0x2,        0x3,        0x4,        0x5,        0x6,        0x7,
    0x8,        0x9,        0xa,        0xb,        0xc,        0xd,        0xe,        0xf,
    0x10,       0x11,       0x12,       0x13,       0x14,       0x15,       0x16,       0x17,
    0x18,       0x19,       0x1a,       0x1b,       0x1c,       0x1d,       0x1e,       0x1f,
    0x20,       0x21,       0x22,       0x23,       0x24,       0x25,       0x26,       0x27,
    0x28,       0x29,       0x2a,       0x2b,       0x2c,       0x2d,       0x2e,       0x2f,
    0x30,       0x31,       0x32,       0x33,       0x34,       0x35,       0x36,       0x37,
    0x38,       0x39,       0x3a,       0x3b,       0x3c,       0x3d,       0x3e,       0x3f,
    0x40,       0x41,       0x42,       0x43,       0x44,       0x45,       0x46,       0x47,
    0x48,       0x49,       0x4a,       0x4b,       0x4c,       0x4d,       0x4e,       0x4f,
    0x50,       0x51,       0x52,       0x53,       0x54,       0x55,       0x56,       0x57,
    0x58,       0x59,       0x5a,       0x5b,       0x5c,       0x5d,       0x5e,       0x5f,
    0x60,       0x61,       0x62,       0x63,       0x64,       0x65,       0x66,       0x67,
    0x68,       0x69,       0x6a,       0x6b,       0x6c,       0x6d,       0x6e,       0x6f,
    0x70,       0x71,       0x72,       0x73,       0x74,       0x75,       0x76,       0x77,
    0x78,       0x79,       0x7a,       0x7b,       0x7c,       0x7d,       0x7e,       0x7f,
    0x20ac,     0x81,       0x201a,     0x192,      0x201e,     0x2026,     0x2020,     0x2021,
    0x88,       0x2030,     0x8a,       0x2039,     0x8c,       0x8d,       0x8e,       0x8f,
    0x90,       0x2018,     0x2019,     0x201c,     0x201d,     0x2022,     0x2013,     0x2014,
    0x98,       0x2122,     0x9a,       0x203a,     0x9c,       0x9d,       0x9e,       0x9f,
    0xa0,       0x385,      0x386,      0xa3,       0xa4,       0xa5,       0xa6,       0xa7,
    0xa8,       0xa9,       0xf8f9,     0xab,       0xac,       0xad,       0xae,       0x2015,
    0xb0,       0xb1,       0xb2,       0xb3,       0x384,      0xb5,       0xb6,       0xb7,
    0x388,      0x389,      0x38a,      0xbb,       0x38c,      0xbd,       0x38e,      0x38f,
    0x390,      0x391,      0x392,      0x393,      0x394,      0x395,      0x396,      0x397,
    0x398,      0x399,      0x39a,      0x39b,      0x39c,      0x39d,      0x39e,      0x39f,
    0x3a0,      0x3a1,      0xf8fa,     0x3a3,      0x3a4,      0x3a5,      0x3a6,      0x3a7,
    0x3a8,      0x3a9,      0x3aa,      0x3ab,      0x3ac,      0x3ad,      0x3ae,      0x3af,
    0x3b0,      0x3b1,      0x3b2,      0x3b3,      0x3b4,      0x3b5,      0x3b6,      0x3b7,
    0x3b8,      0x3b9,      0x3ba,      0x3bb,      0x3bc,      0x3bd,      0x3be,      0x3bf,
    0x3c0,      0x3c1,      0x3c2,      0x3c3,      0x3c4,      0x3c5,      0x3c6,      0x3c7,
    0x3c8,      0x3c9,      0x3ca,      0x3cb,      0x3cc,      0x3cd,      0x3ce,      0xf8fb,
};

void    TableDrivenCodePageConverter_<kCodePage_GREEK>::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt)
{
    size_t  nCharsToCopy    =   min (inMBCharCnt, *outCharCnt);
    for (size_t i = 0; i < nCharsToCopy; ++i) {
        outChars[i] = kMap[(unsigned char)inMBChars[i]];
    }
    *outCharCnt = nCharsToCopy;
}

void    TableDrivenCodePageConverter_<kCodePage_GREEK>::MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt)
{
    size_t  nCharsToCopy    =   min (inCharCnt, *outCharCnt);
    for (size_t i = 0; i < nCharsToCopy; ++i) {
        size_t j = 0;
        for (; j < 256; ++j) {
            if (kMap[j] == inChars[i]) {
                outChars[i] = static_cast<char> (j);
                break;
            }
        }
        if (j == 256) {
            /*Perhaps should add extra args to this function - as in Win32 API - for handling this missing-char case*/
            outChars[i] = '?';
        }
    }
    *outCharCnt = nCharsToCopy;
}








/*
 ********************************************************************************
 **************** TableDrivenCodePageConverter_<kCodePage_Turkish> ***************
 ********************************************************************************
 */
const   char16_t        TableDrivenCodePageConverter_<kCodePage_Turkish>::kMap[256] = {
    0x0,        0x1,        0x2,        0x3,        0x4,        0x5,        0x6,        0x7,
    0x8,        0x9,        0xa,        0xb,        0xc,        0xd,        0xe,        0xf,
    0x10,       0x11,       0x12,       0x13,       0x14,       0x15,       0x16,       0x17,
    0x18,       0x19,       0x1a,       0x1b,       0x1c,       0x1d,       0x1e,       0x1f,
    0x20,       0x21,       0x22,       0x23,       0x24,       0x25,       0x26,       0x27,
    0x28,       0x29,       0x2a,       0x2b,       0x2c,       0x2d,       0x2e,       0x2f,
    0x30,       0x31,       0x32,       0x33,       0x34,       0x35,       0x36,       0x37,
    0x38,       0x39,       0x3a,       0x3b,       0x3c,       0x3d,       0x3e,       0x3f,
    0x40,       0x41,       0x42,       0x43,       0x44,       0x45,       0x46,       0x47,
    0x48,       0x49,       0x4a,       0x4b,       0x4c,       0x4d,       0x4e,       0x4f,
    0x50,       0x51,       0x52,       0x53,       0x54,       0x55,       0x56,       0x57,
    0x58,       0x59,       0x5a,       0x5b,       0x5c,       0x5d,       0x5e,       0x5f,
    0x60,       0x61,       0x62,       0x63,       0x64,       0x65,       0x66,       0x67,
    0x68,       0x69,       0x6a,       0x6b,       0x6c,       0x6d,       0x6e,       0x6f,
    0x70,       0x71,       0x72,       0x73,       0x74,       0x75,       0x76,       0x77,
    0x78,       0x79,       0x7a,       0x7b,       0x7c,       0x7d,       0x7e,       0x7f,
    0x20ac,     0x81,       0x201a,     0x192,      0x201e,     0x2026,     0x2020,     0x2021,
    0x2c6,      0x2030,     0x160,      0x2039,     0x152,      0x8d,       0x8e,       0x8f,
    0x90,       0x2018,     0x2019,     0x201c,     0x201d,     0x2022,     0x2013,     0x2014,
    0x2dc,      0x2122,     0x161,      0x203a,     0x153,      0x9d,       0x9e,       0x178,
    0xa0,       0xa1,       0xa2,       0xa3,       0xa4,       0xa5,       0xa6,       0xa7,
    0xa8,       0xa9,       0xaa,       0xab,       0xac,       0xad,       0xae,       0xaf,
    0xb0,       0xb1,       0xb2,       0xb3,       0xb4,       0xb5,       0xb6,       0xb7,
    0xb8,       0xb9,       0xba,       0xbb,       0xbc,       0xbd,       0xbe,       0xbf,
    0xc0,       0xc1,       0xc2,       0xc3,       0xc4,       0xc5,       0xc6,       0xc7,
    0xc8,       0xc9,       0xca,       0xcb,       0xcc,       0xcd,       0xce,       0xcf,
    0x11e,      0xd1,       0xd2,       0xd3,       0xd4,       0xd5,       0xd6,       0xd7,
    0xd8,       0xd9,       0xda,       0xdb,       0xdc,       0x130,      0x15e,      0xdf,
    0xe0,       0xe1,       0xe2,       0xe3,       0xe4,       0xe5,       0xe6,       0xe7,
    0xe8,       0xe9,       0xea,       0xeb,       0xec,       0xed,       0xee,       0xef,
    0x11f,      0xf1,       0xf2,       0xf3,       0xf4,       0xf5,       0xf6,       0xf7,
    0xf8,       0xf9,       0xfa,       0xfb,       0xfc,       0x131,      0x15f,      0xff,
};

void    TableDrivenCodePageConverter_<kCodePage_Turkish>::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt)
{
    size_t  nCharsToCopy    =   min (inMBCharCnt, *outCharCnt);
    for (size_t i = 0; i < nCharsToCopy; ++i) {
        outChars[i] = kMap[(unsigned char)inMBChars[i]];
    }
    *outCharCnt = nCharsToCopy;
}

void    TableDrivenCodePageConverter_<kCodePage_Turkish>::MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt)
{
    size_t  nCharsToCopy    =   min (inCharCnt, *outCharCnt);
    for (size_t i = 0; i < nCharsToCopy; ++i) {
        size_t j = 0;
        for (; j < 256; ++j) {
            if (kMap[j] == inChars[i]) {
                outChars[i] = static_cast<char> (j);
                break;
            }
        }
        if (j == 256) {
            /*Perhaps should add extra args to this function - as in Win32 API - for handling this missing-char case*/
            outChars[i] = '?';
        }
    }
    *outCharCnt = nCharsToCopy;
}









/*
 ********************************************************************************
 **************** TableDrivenCodePageConverter_<kCodePage_HEBREW> ***************
 ********************************************************************************
 */
const   char16_t        TableDrivenCodePageConverter_<kCodePage_HEBREW>::kMap[256] = {
    0x0,        0x1,        0x2,        0x3,        0x4,        0x5,        0x6,        0x7,
    0x8,        0x9,        0xa,        0xb,        0xc,        0xd,        0xe,        0xf,
    0x10,       0x11,       0x12,       0x13,       0x14,       0x15,       0x16,       0x17,
    0x18,       0x19,       0x1a,       0x1b,       0x1c,       0x1d,       0x1e,       0x1f,
    0x20,       0x21,       0x22,       0x23,       0x24,       0x25,       0x26,       0x27,
    0x28,       0x29,       0x2a,       0x2b,       0x2c,       0x2d,       0x2e,       0x2f,
    0x30,       0x31,       0x32,       0x33,       0x34,       0x35,       0x36,       0x37,
    0x38,       0x39,       0x3a,       0x3b,       0x3c,       0x3d,       0x3e,       0x3f,
    0x40,       0x41,       0x42,       0x43,       0x44,       0x45,       0x46,       0x47,
    0x48,       0x49,       0x4a,       0x4b,       0x4c,       0x4d,       0x4e,       0x4f,
    0x50,       0x51,       0x52,       0x53,       0x54,       0x55,       0x56,       0x57,
    0x58,       0x59,       0x5a,       0x5b,       0x5c,       0x5d,       0x5e,       0x5f,
    0x60,       0x61,       0x62,       0x63,       0x64,       0x65,       0x66,       0x67,
    0x68,       0x69,       0x6a,       0x6b,       0x6c,       0x6d,       0x6e,       0x6f,
    0x70,       0x71,       0x72,       0x73,       0x74,       0x75,       0x76,       0x77,
    0x78,       0x79,       0x7a,       0x7b,       0x7c,       0x7d,       0x7e,       0x7f,
    0x20ac,     0x81,       0x201a,     0x192,      0x201e,     0x2026,     0x2020,     0x2021,
    0x2c6,      0x2030,     0x8a,       0x2039,     0x8c,       0x8d,       0x8e,       0x8f,
    0x90,       0x2018,     0x2019,     0x201c,     0x201d,     0x2022,     0x2013,     0x2014,
    0x2dc,      0x2122,     0x9a,       0x203a,     0x9c,       0x9d,       0x9e,       0x9f,
    0xa0,       0xa1,       0xa2,       0xa3,       0x20aa,     0xa5,       0xa6,       0xa7,
    0xa8,       0xa9,       0xd7,       0xab,       0xac,       0xad,       0xae,       0xaf,
    0xb0,       0xb1,       0xb2,       0xb3,       0xb4,       0xb5,       0xb6,       0xb7,
    0xb8,       0xb9,       0xf7,       0xbb,       0xbc,       0xbd,       0xbe,       0xbf,
    0x5b0,      0x5b1,      0x5b2,      0x5b3,      0x5b4,      0x5b5,      0x5b6,      0x5b7,
    0x5b8,      0x5b9,      0x5ba,      0x5bb,      0x5bc,      0x5bd,      0x5be,      0x5bf,
    0x5c0,      0x5c1,      0x5c2,      0x5c3,      0x5f0,      0x5f1,      0x5f2,      0x5f3,
    0x5f4,      0xf88d,     0xf88e,     0xf88f,     0xf890,     0xf891,     0xf892,     0xf893,
    0x5d0,      0x5d1,      0x5d2,      0x5d3,      0x5d4,      0x5d5,      0x5d6,      0x5d7,
    0x5d8,      0x5d9,      0x5da,      0x5db,      0x5dc,      0x5dd,      0x5de,      0x5df,
    0x5e0,      0x5e1,      0x5e2,      0x5e3,      0x5e4,      0x5e5,      0x5e6,      0x5e7,
    0x5e8,      0x5e9,      0x5ea,      0xf894,     0xf895,     0x200e,     0x200f,     0xf896,
};

void    TableDrivenCodePageConverter_<kCodePage_HEBREW>::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt)
{
    size_t  nCharsToCopy    =   min (inMBCharCnt, *outCharCnt);
    for (size_t i = 0; i < nCharsToCopy; ++i) {
        outChars[i] = kMap[(unsigned char)inMBChars[i]];
    }
    *outCharCnt = nCharsToCopy;
}

void    TableDrivenCodePageConverter_<kCodePage_HEBREW>::MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt)
{
    size_t  nCharsToCopy    =   min (inCharCnt, *outCharCnt);
    for (size_t i = 0; i < nCharsToCopy; ++i) {
        size_t j = 0;
        for (; j < 256; ++j) {
            if (kMap[j] == inChars[i]) {
                outChars[i] = static_cast<char> (j);
                break;
            }
        }
        if (j == 256) {
            /*Perhaps should add extra args to this function - as in Win32 API - for handling this missing-char case*/
            outChars[i] = '?';
        }
    }
    *outCharCnt = nCharsToCopy;
}








/*
 ********************************************************************************
 ******************* TableDrivenCodePageConverter_<kCodePage_ARABIC> ************
 ********************************************************************************
 */
const   char16_t        TableDrivenCodePageConverter_<kCodePage_ARABIC>::kMap[256] = {
    0x0,        0x1,        0x2,        0x3,        0x4,        0x5,        0x6,        0x7,
    0x8,        0x9,        0xa,        0xb,        0xc,        0xd,        0xe,        0xf,
    0x10,       0x11,       0x12,       0x13,       0x14,       0x15,       0x16,       0x17,
    0x18,       0x19,       0x1a,       0x1b,       0x1c,       0x1d,       0x1e,       0x1f,
    0x20,       0x21,       0x22,       0x23,       0x24,       0x25,       0x26,       0x27,
    0x28,       0x29,       0x2a,       0x2b,       0x2c,       0x2d,       0x2e,       0x2f,
    0x30,       0x31,       0x32,       0x33,       0x34,       0x35,       0x36,       0x37,
    0x38,       0x39,       0x3a,       0x3b,       0x3c,       0x3d,       0x3e,       0x3f,
    0x40,       0x41,       0x42,       0x43,       0x44,       0x45,       0x46,       0x47,
    0x48,       0x49,       0x4a,       0x4b,       0x4c,       0x4d,       0x4e,       0x4f,
    0x50,       0x51,       0x52,       0x53,       0x54,       0x55,       0x56,       0x57,
    0x58,       0x59,       0x5a,       0x5b,       0x5c,       0x5d,       0x5e,       0x5f,
    0x60,       0x61,       0x62,       0x63,       0x64,       0x65,       0x66,       0x67,
    0x68,       0x69,       0x6a,       0x6b,       0x6c,       0x6d,       0x6e,       0x6f,
    0x70,       0x71,       0x72,       0x73,       0x74,       0x75,       0x76,       0x77,
    0x78,       0x79,       0x7a,       0x7b,       0x7c,       0x7d,       0x7e,       0x7f,
    0x20ac,     0x67e,      0x201a,     0x192,      0x201e,     0x2026,     0x2020,     0x2021,
    0x2c6,      0x2030,     0x679,      0x2039,     0x152,      0x686,      0x698,      0x688,
    0x6af,      0x2018,     0x2019,     0x201c,     0x201d,     0x2022,     0x2013,     0x2014,
    0x6a9,      0x2122,     0x691,      0x203a,     0x153,      0x200c,     0x200d,     0x6ba,
    0xa0,       0x60c,      0xa2,       0xa3,       0xa4,       0xa5,       0xa6,       0xa7,
    0xa8,       0xa9,       0x6be,      0xab,       0xac,       0xad,       0xae,       0xaf,
    0xb0,       0xb1,       0xb2,       0xb3,       0xb4,       0xb5,       0xb6,       0xb7,
    0xb8,       0xb9,       0x61b,      0xbb,       0xbc,       0xbd,       0xbe,       0x61f,
    0x6c1,      0x621,      0x622,      0x623,      0x624,      0x625,      0x626,      0x627,
    0x628,      0x629,      0x62a,      0x62b,      0x62c,      0x62d,      0x62e,      0x62f,
    0x630,      0x631,      0x632,      0x633,      0x634,      0x635,      0x636,      0xd7,
    0x637,      0x638,      0x639,      0x63a,      0x640,      0x641,      0x642,      0x643,
    0xe0,       0x644,      0xe2,       0x645,      0x646,      0x647,      0x648,      0xe7,
    0xe8,       0xe9,       0xea,       0xeb,       0x649,      0x64a,      0xee,       0xef,
    0x64b,      0x64c,      0x64d,      0x64e,      0xf4,       0x64f,      0x650,      0xf7,
    0x651,      0xf9,       0x652,      0xfb,       0xfc,       0x200e,     0x200f,     0x6d2,
};

void    TableDrivenCodePageConverter_<kCodePage_ARABIC>::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt)
{
    size_t  nCharsToCopy    =   min (inMBCharCnt, *outCharCnt);
    for (size_t i = 0; i < nCharsToCopy; ++i) {
        outChars[i] = kMap[(unsigned char)inMBChars[i]];
    }
    *outCharCnt = nCharsToCopy;
}

void    TableDrivenCodePageConverter_<kCodePage_ARABIC>::MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt)
{
    size_t  nCharsToCopy    =   min (inCharCnt, *outCharCnt);
    for (size_t i = 0; i < nCharsToCopy; ++i) {
        size_t j = 0;
        for (; j < 256; ++j) {
            if (kMap[j] == inChars[i]) {
                outChars[i] = static_cast<char> (j);
                break;
            }
        }
        if (j == 256) {
            /*Perhaps should add extra args to this function - as in Win32 API - for handling this missing-char case*/
            outChars[i] = '?';
        }
    }
    *outCharCnt = nCharsToCopy;
}









/*
 ********************************************************************************
 ********************************** UTF8Converter *******************************
 ********************************************************************************
 */

void    UTF8Converter::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt) const
{
    Require (inMBCharCnt == 0 or inMBChars != nullptr);
    RequireNotNull (outCharCnt);
    Require (*outCharCnt == 0 or outChars != nullptr);

    /*
     *  NOTE - based on ConvertUTF8toUTF16 () code from ConvertUTF.C, written by Mark E. Davis (mark_davis@taligent.com),
     *   and owned by Taligtent. That code is copyrighted. It says it cannot be reproduced without the consent of Taligent,
     *  but the Taligent company doesn't seem to exist anymore (at least no web site). Also - technically,
     *  I'm not sure if this is a reproduction, since I've rewritten it (somewhat).
     *  I hope inclusion of this notice is sufficient. -- LGP 2001-09-15
     *
     *  Original code was found refered to on web page: http://www.czyborra.com/utf/
     *  and downloaded from URL:                        ftp://ftp.unicode.org/Public/PROGRAMS/CVTUTF/
     *
     *  NB: I COULD get portable UTF7 code from the same location, but it didn't seem worth the trouble.
     */
    {
        enum ConversionResult {
            ok,                 /* conversion successful */
            sourceExhausted,    /* partial character in source, but hit end */
            targetExhausted     /* insuff. room in target for conversion */
        };
        using   UCS4    =   uint32_t;
        using   UTF16   =   uint16_t;
        using   UTF8    =   uint8_t;
        const UCS4 kReplacementCharacter =  0x0000FFFDUL;
        const UCS4 kMaximumUCS2 =           0x0000FFFFUL;
        const UCS4 kMaximumUTF16 =          0x0010FFFFUL;
        const UCS4 kMaximumUCS4 =           0x7FFFFFFFUL;
        static  const   char bytesFromUTF8[256] = {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5
        };
        static  const   UCS4 offsetsFromUTF8[6] =   {0x00000000UL, 0x00003080UL, 0x000E2080UL,
                                                     0x03C82080UL, 0xFA082080UL, 0x82082080UL
                                                    };
        const int halfShift             = 10;
        const UCS4 halfBase             = 0x0010000UL;
        const UCS4 halfMask             = 0x3FFUL;
        const UCS4 kSurrogateHighStart  = 0xD800UL;
        const UCS4 kSurrogateHighEnd    = 0xDBFFUL;
        const UCS4 kSurrogateLowStart   = 0xDC00UL;
        const UCS4 kSurrogateLowEnd     = 0xDFFFUL;
        ConversionResult result = ok;
        const UTF8* source = reinterpret_cast<const UTF8*> (inMBChars);
        const UTF8* sourceEnd = source + inMBCharCnt;
        UTF16* target = reinterpret_cast<UTF16*> (outChars);
        UTF16* targetEnd = target + *outCharCnt;
        while (source < sourceEnd) {
            UCS4 ch = 0;
            unsigned short extraBytesToWrite = bytesFromUTF8[*source];
            if (source + extraBytesToWrite > sourceEnd) {
                result = sourceExhausted;
                break;
            };
            switch(extraBytesToWrite) { /* note: code falls through cases! */
                case 5:
                    ch += *source++;
                    ch <<= 6;
                case 4:
                    ch += *source++;
                    ch <<= 6;
                case 3:
                    ch += *source++;
                    ch <<= 6;
                case 2:
                    ch += *source++;
                    ch <<= 6;
                case 1:
                    ch += *source++;
                    ch <<= 6;
                case 0:
                    ch += *source++;
            };
            ch -= offsetsFromUTF8[extraBytesToWrite];

            if (target >= targetEnd) {
                result = targetExhausted;
                break;
            };
            if (ch <= kMaximumUCS2) {
                Assert (target < targetEnd);
                *target++ = static_cast<UTF16> (ch);
            }
            else if (ch > kMaximumUTF16) {
                Assert (target < targetEnd);
                *target++ = kReplacementCharacter;
            }
            else {
                if (target + 1 >= targetEnd) {
                    result = targetExhausted;
                    break;
                };
                ch -= halfBase;
                Assert (target < targetEnd);
                *target++ = static_cast<UTF16> ((ch >> halfShift) + kSurrogateHighStart);
                Assert (target < targetEnd);
                *target++ = static_cast<UTF16> ((ch & halfMask) + kSurrogateLowStart);
            };
        };

        // For now - we ignore ConversionResult flag - and just say how much output was generated...
        //      *sourceStart = source;
        //      *targetStart = target;
        //      return result;
        *outCharCnt = (reinterpret_cast<char16_t*> (target) - outChars);
    }
}

void    UTF8Converter::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char32_t* outChars, size_t* outCharCnt) const
{
// Not really right - but hopefully adquate for starters -- LGP 2011-09-06
    SmallStackBuffer<char16_t>  tmpBuf (*outCharCnt);
    MapToUNICODE (inMBChars, inMBCharCnt, tmpBuf, outCharCnt);
}

void    UTF8Converter::MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt) const
{
    /*
     *  NOTE - based on ConvertUTF16toUTF8 () code from ConvertUTF.C, written by Mark E. Davis (mark_davis@taligent.com),
     *   and owned by Taligtent. That code is copyrighted. It says it cannot be reproduced without the consent of Taligent,
     *  but the Taligent company doesn't seem to exist anymore (at least no web site). Also - technically,
     *  I'm not sure if this is a reproduction, since I've rewritten it (somewhat).
     *  I hope inclusion of this notice is sufficient. -- LGP 2001-09-15
     *
     *  Original code was found refered to on web page: http://www.czyborra.com/utf/
     *  and downloaded from URL:                        ftp://ftp.unicode.org/Public/PROGRAMS/CVTUTF/
     *
     *  NB: I COULD get portable UTF7 code from the same location, but it didn't seem worth the trouble.
     */
    {
        enum ConversionResult {
            ok,                 /* conversion successful */
            sourceExhausted,    /* partial character in source, but hit end */
            targetExhausted     /* insuff. room in target for conversion */
        };
        using   UCS4    =   uint32_t;
        using   UTF16   =   uint16_t;
        using   UTF8    =   uint8_t;
        const UCS4 kReplacementCharacter =  0x0000FFFDUL;
        const UCS4 kMaximumUCS2 =           0x0000FFFFUL;
        const UCS4 kMaximumUTF16 =          0x0010FFFFUL;
        const UCS4 kMaximumUCS4 =           0x7FFFFFFFUL;
        static  const   char bytesFromUTF8[256] = {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5
        };
        static  const   UCS4 offsetsFromUTF8[6] =   {0x00000000UL, 0x00003080UL, 0x000E2080UL,
                                                     0x03C82080UL, 0xFA082080UL, 0x82082080UL
                                                    };
        static  const   UTF8 firstByteMark[7] = {0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC};
        const int halfShift             = 10;
        const UCS4 halfBase             = 0x0010000UL;
        const UCS4 halfMask             = 0x3FFUL;
        const UCS4 kSurrogateHighStart  = 0xD800UL;
        const UCS4 kSurrogateHighEnd    = 0xDBFFUL;
        const UCS4 kSurrogateLowStart   = 0xDC00UL;
        const UCS4 kSurrogateLowEnd     = 0xDFFFUL;
        ConversionResult result = ok;
        const UTF16* source = reinterpret_cast <const UTF16*> (inChars);
        const UTF16* sourceEnd = source + inCharCnt;
        UTF8* target = reinterpret_cast<UTF8*> (outChars);
        const UTF8* targetEnd = target + *outCharCnt;
        while (source < sourceEnd) {
            UCS4 ch;
            unsigned short bytesToWrite = 0;
            const UCS4 byteMask = 0xBF;
            const UCS4 byteMark = 0x80;
            ch = *source++;
            if (ch >= kSurrogateHighStart && ch <= kSurrogateHighEnd
                    && source < sourceEnd) {
                UCS4 ch2 = *source;
                if (ch2 >= kSurrogateLowStart && ch2 <= kSurrogateLowEnd) {
                    ch = ((ch - kSurrogateHighStart) << halfShift)
                         + (ch2 - kSurrogateLowStart) + halfBase;
                    ++source;
                };
            };
            if (ch < 0x80) {
                bytesToWrite = 1;
            }
            else if (ch < 0x800) {
                bytesToWrite = 2;
            }
            else if (ch < 0x10000) {
                bytesToWrite = 3;
            }
            else if (ch < 0x200000) {
                bytesToWrite = 4;
            }
            else if (ch < 0x4000000) {
                bytesToWrite = 5;
            }
            else if (ch <= kMaximumUCS4) {
                bytesToWrite = 6;
            }
            else {
                bytesToWrite = 2;
                ch = kReplacementCharacter;
            }; /* I wish there were a smart way to avoid this conditional */

            target += bytesToWrite;
            if (target > targetEnd) {
                target -= bytesToWrite;
                result = targetExhausted;
                break;
            };
            switch (bytesToWrite) { /* note: code falls through cases! */
                case 6:
                    *--target = static_cast<UTF8> ((ch | byteMark) & byteMask);
                    ch >>= 6;
                case 5:
                    *--target = static_cast<UTF8> ((ch | byteMark) & byteMask);
                    ch >>= 6;
                case 4:
                    *--target = static_cast<UTF8> ((ch | byteMark) & byteMask);
                    ch >>= 6;
                case 3:
                    *--target = static_cast<UTF8> ((ch | byteMark) & byteMask);
                    ch >>= 6;
                case 2:
                    *--target = static_cast<UTF8> ((ch | byteMark) & byteMask);
                    ch >>= 6;
                case 1:
                    *--target = static_cast<UTF8> (ch | firstByteMark[bytesToWrite]);
            };
            target += bytesToWrite;
        };
        // For now - we ignore ConversionResult flag - and just say how much output was generated...
        //      *sourceStart = source;
        //      *targetStart = target;
        //      return result;
        *outCharCnt = (target - reinterpret_cast<UTF8*> (outChars));
    }
#if     0
    /*
     *  This code is based on comments in the document: http://www.czyborra.com/utf/
     *          -- LGP 2001-09-15
     */
    char*               outP        =   outChars;
    char*               outEOB      =   outChars + *outCharCnt;
    const   wchar_t*    inCharEnd   =   inChars + inCharCnt;
    for (const wchar_t* i = inChars; i < inCharEnd; ++i) {
        wchar_t c   =   *i;
        if (c < 0x80) {
            if (outP >= outEOB) {
                return; // outCharCnt already set. Unclear if/how to signal buffer too small?
            }
            *outP++ = c;
        }
        else if (c < 0x800) {
            if (outP + 1 >= outEOB) {
                return; // outCharCnt already set. Unclear if/how to signal buffer too small?
            }
            *outP++ = (0xC0 | c >> 6);
            *outP++ = (0x80 | c & 0x3F);
        }
        else if (c < 0x10000) {
            if (outP + 2 >= outEOB) {
                return; // outCharCnt already set. Unclear if/how to signal buffer too small?
            }
            *outP++ = (0xE0 | c >> 12);
            *outP++ = (0x80 | c >> 6 & 0x3F);
            *outP++ = (0x80 | c & 0x3F);
        }
        else if (c < 0x200000) {
            if (outP + 3 >= outEOB) {
                return; // outCharCnt already set. Unclear if/how to signal buffer too small?
            }
            *outP++ = (0xF0 | c >> 18);
            *outP++ = (0x80 | c >> 12 & 0x3F);
            *outP++ = (0x80 | c >> 6 & 0x3F);
            *outP++ = (0x80 | c & 0x3F);
        }
        else {
            // NOT SURE WHAT TODO HERE??? IGNORE FOR NOW...
        }
    }
    *outCharCnt = (outP - outChars);
#endif
}

void    UTF8Converter::MapFromUNICODE (const char32_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt) const
{
// Not really right - but hopefully adquate for starters -- LGP 2011-09-06
    SmallStackBuffer<char16_t>  tmpBuf (*outCharCnt);
    for (size_t i = 0; i < inCharCnt; ++i) {
        tmpBuf[i] = inChars[i];
    }
    MapFromUNICODE (tmpBuf, inCharCnt, outChars, outCharCnt);
}









/*
 ********************************************************************************
 ********************************** CodePagesInstalled **************************
 ********************************************************************************
 */
namespace   {
#if     qPlatform_Windows
    shared_ptr<set<CodePage>>   s_EnumCodePagesProc_Accumulator_;
    BOOL FAR    PASCAL EnumCodePagesProc_ (LPTSTR lpCodePageString)
    {
        s_EnumCodePagesProc_Accumulator_->insert (_ttoi (lpCodePageString));
        return (1);
    }
#endif
}

CodePagesInstalled::CodePagesInstalled ()
    : fCodePages_ ()
{
    Assert (fCodePages_.size () == 0);

    shared_ptr<set<CodePage>>   accum = make_shared<set<CodePage>> ();
#if     qPlatform_Windows
    static  mutex  sCritSec_;
    {
        auto    critSec { make_unique_lock (sCritSec_) };
        Assert (s_EnumCodePagesProc_Accumulator_.get () == nullptr);
        s_EnumCodePagesProc_Accumulator_ = accum;
        ::EnumSystemCodePages (EnumCodePagesProc_, CP_INSTALLED);
        s_EnumCodePagesProc_Accumulator_.reset ();
    }
#endif
    // Add these 'fake' code pages - which I believe are always available, but never listed by EnumSystemCodePages()
    accum->insert (kCodePage_UNICODE_WIDE);
    accum->insert (kCodePage_UNICODE_WIDE_BIGENDIAN);
    accum->insert (kCodePage_UTF8);
    fCodePages_ = vector<CodePage> (accum->begin (), accum->end ());
}







/*
 ********************************************************************************
 ********************************** CodePagesGuesser ****************************
 ********************************************************************************
 */
CodePage    CodePagesGuesser::Guess (const void* input, size_t nBytes, Confidence* confidence, size_t* bytesFromFrontToStrip)
{
    if (confidence != nullptr) {
        *confidence = Confidence::eLow;
    }
    if (bytesFromFrontToStrip != nullptr) {
        *bytesFromFrontToStrip = 0;
    }
    if (nBytes >= 2) {
        unsigned char   c0  =   reinterpret_cast<const unsigned char*> (input)[0];
        unsigned char   c1  =   reinterpret_cast<const unsigned char*> (input)[1];
        if (c0 == 0xff and c1 == 0xfe) {
            if (confidence != nullptr) {
                *confidence = Confidence::eHigh;
            }
            if (bytesFromFrontToStrip != nullptr) {
                *bytesFromFrontToStrip = 2;
            }
            return kCodePage_UNICODE_WIDE;
        }
        if (c0 == 0xfe and c1 == 0xff) {
            if (confidence != nullptr) {
                *confidence = Confidence::eHigh;
            }
            if (bytesFromFrontToStrip != nullptr) {
                *bytesFromFrontToStrip = 2;
            }
            return kCodePage_UNICODE_WIDE_BIGENDIAN;
        }
        if (nBytes >= 3) {
            unsigned char   c2  =   reinterpret_cast<const unsigned char*> (input)[2];
            if (c0 == 0xef and c1 == 0xbb and c2 == 0xbf) {
                if (confidence != nullptr) {
                    *confidence = Confidence::eHigh;
                }
                if (bytesFromFrontToStrip != nullptr) {
                    *bytesFromFrontToStrip = 3;
                }
                return kCodePage_UTF8;
            }
        }
        if (nBytes >= 5) {
            unsigned char   c2  =   reinterpret_cast<const unsigned char*> (input)[2];
            unsigned char   c3  =   reinterpret_cast<const unsigned char*> (input)[3];
            unsigned char   c4  =   reinterpret_cast<const unsigned char*> (input)[4];
            if (c0 == 0x2b and c1 == 0x2f and c2 == 0x76 and c3 == 0x38 and c4 == 0x2d) {
                if (confidence != nullptr) {
                    *confidence = Confidence::eHigh;
                }
                if (bytesFromFrontToStrip != nullptr) {
                    *bytesFromFrontToStrip = 5;
                }
                return kCodePage_UTF7;
            }
        }
    }


    /*
     * Final ditch efforts if we don't recognize any prefix.
     */
    if (confidence != nullptr) {
        *confidence = Confidence::eLow;
    }
    return Characters::GetDefaultSDKCodePage ();
}








/*
 ********************************************************************************
 ****************************** CodePagePrettyNameMapper ************************
 ********************************************************************************
 */
CodePagePrettyNameMapper::CodePageNames CodePagePrettyNameMapper::sCodePageNames_    =   CodePagePrettyNameMapper::MakeDefaultCodePageNames ();

CodePagePrettyNameMapper::CodePageNames CodePagePrettyNameMapper::MakeDefaultCodePageNames ()
{
    CodePageNames   codePageNames;
    codePageNames.fUNICODE_WIDE             =   L"UNICODE {wide characters}";
    codePageNames.fUNICODE_WIDE_BIGENDIAN   =   L"UNICODE {wide characters - big endian}";
    codePageNames.fANSI                     =   L"ANSI (1252)";
    codePageNames.fMAC                      =   L"MAC (2)";
    codePageNames.fPC                       =   L"IBM PC United States code page (437)";
    codePageNames.fSJIS                     =   L"Japanese SJIS {932}";
    codePageNames.fUTF7                     =   L"UNICODE {UTF-7}";
    codePageNames.fUTF8                     =   L"UNICODE {UTF-8}";
    codePageNames.f850                      =   L"Latin I - MS-DOS Multilingual (850)";
    codePageNames.f851                      =   L"Latin II - MS-DOS Slavic (850)";
    codePageNames.f866                      =   L"Russian - MS-DOS (866)";
    codePageNames.f936                      =   L"Chinese {Simplfied} (936)";
    codePageNames.f949                      =   L"Korean (949)";
    codePageNames.f950                      =   L"Chinese {Traditional} (950)";
    codePageNames.f1250                     =   L"Eastern European Windows (1250)";
    codePageNames.f1251                     =   L"Cyrilic (1251)";
    codePageNames.f10000                    =   L"Roman {Macintosh} (10000)";
    codePageNames.f10001                    =   L"Japanese {Macintosh} (10001)";
    codePageNames.f50220                    =   L"Japanese JIS (50220)";
    return codePageNames;
}

wstring  CodePagePrettyNameMapper::GetName (CodePage cp)
{
    switch (cp) {
        case    kCodePage_UNICODE_WIDE:
            return sCodePageNames_.fUNICODE_WIDE;
        case    kCodePage_UNICODE_WIDE_BIGENDIAN:
            return sCodePageNames_.fUNICODE_WIDE_BIGENDIAN;
        case    kCodePage_ANSI:
            return sCodePageNames_.fANSI;
        case    kCodePage_MAC:
            return sCodePageNames_.fMAC;
        case    kCodePage_PC:
            return sCodePageNames_.fPC;
        case    kCodePage_SJIS:
            return sCodePageNames_.fSJIS;
        case    kCodePage_UTF7:
            return sCodePageNames_.fUTF7;
        case    kCodePage_UTF8:
            return sCodePageNames_.fUTF8;
        case    850:
            return sCodePageNames_.f850;
        case    851:
            return sCodePageNames_.f851;
        case    866:
            return sCodePageNames_.f866;
        case    936:
            return sCodePageNames_.f936;
        case    949:
            return sCodePageNames_.f949;
        case    950:
            return sCodePageNames_.f950;
        case    1250:
            return sCodePageNames_.f1250;
        case    1251:
            return sCodePageNames_.f1251;
        case    10000:
            return sCodePageNames_.f10000;
        case    10001:
            return sCodePageNames_.f10001;
        case    50220:
            return sCodePageNames_.f50220;
        default: {
                return Characters::CString::Format (L"%d", cp);
            }
    }
}







/*
 ********************************************************************************
 *********************** Characters::WideStringToNarrow *************************
 ********************************************************************************
 */
namespace   {
    void    PortableWideStringToNarrow_ (const wchar_t* wsStart, const wchar_t* wsEnd, CodePage codePage, string* intoResult)
    {
        RequireNotNull (intoResult);
        Require (wsStart <= wsEnd);
        size_t  inSize  =   wsEnd - wsStart;
        CodePageConverter   cc (codePage);
// this grossly overestimates size - which is a problem for the RESIZE below!!! COULD pointlessly run out of memroy and intitialize data to good values...
        size_t outSizeBuf = cc.MapFromUNICODE_QuickComputeOutBufSize (wsStart, inSize);
        intoResult->resize (outSizeBuf);
        size_t  actualOutSize   =   0;
        if (inSize != 0) {
            actualOutSize = outSizeBuf;
            cc.MapFromUNICODE (wsStart, inSize, Containers::Start (*intoResult), &actualOutSize);
            if (intoResult->size () != actualOutSize) {
                // shrink
                Assert (intoResult->size () > actualOutSize);
                intoResult->resize (actualOutSize);
            }
        }
    }
}
void    Characters::WideStringToNarrow (const wchar_t* wsStart, const wchar_t* wsEnd, CodePage codePage, string* intoResult)
{
    RequireNotNull (intoResult);
    Require (wsStart <= wsEnd);

#if     qPlatform_Windows
    Platform::Windows::WideStringToNarrow (wsStart, wsEnd, codePage, intoResult);
#else
    PortableWideStringToNarrow_ (wsStart, wsEnd, codePage, intoResult);
#endif
}








/*
 ********************************************************************************
 *********************** Characters::NarrowStringToWide *************************
 ********************************************************************************
 */
namespace   {
    void    PortableNarrowStringToWide_ (const char* sStart, const char* sEnd, int codePage, wstring* intoResult)
    {
        RequireNotNull (intoResult);
        Require (sStart <= sEnd);
        size_t  inSize  =   sEnd - sStart;
        CodePageConverter   cc (codePage);
// this grossly overestimates size - which is a problem for the RESIZE below!!! COULD pointlessly run out of memroy and intitialize data to good values...
        size_t outSizeBuf = cc.MapToUNICODE_QuickComputeOutBufSize (sStart, inSize);
        intoResult->resize (outSizeBuf);
        size_t  actualOutSize   =   0;
        if (inSize != 0) {
            actualOutSize = outSizeBuf;
            cc.MapToUNICODE (sStart, inSize, Containers::Start (*intoResult), &actualOutSize);
            if (intoResult->size () != actualOutSize) {
                // shrink
                Assert (intoResult->size () > actualOutSize);
                intoResult->resize (actualOutSize);
            }
        }
    }
}
void    Characters::NarrowStringToWide (const char* sStart, const char* sEnd, int codePage, wstring* intoResult)
{
    RequireNotNull (intoResult);
    Require (sStart <= sEnd);
#if     qPlatform_Windows
    Platform::Windows::NarrowStringToWide (sStart, sEnd, codePage, intoResult);
#else
    PortableNarrowStringToWide_ (sStart, sEnd, codePage, intoResult);
#endif
}










/*
 ********************************************************************************
 *********************** MapUNICODETextWithMaybeBOMTowstring ********************
 ********************************************************************************
 */
wstring Characters::MapUNICODETextWithMaybeBOMTowstring (const char* start, const char* end)
{
    Require (start <= end);
    if (start == end) {
        return wstring ();
    }
    else {
        size_t  outBufSize      =   end - start;
        SmallStackBuffer<wchar_t>   wideBuf (outBufSize);
        size_t  outCharCount    =   outBufSize;
        MapSBUnicodeTextWithMaybeBOMToUNICODE (start, end - start, wideBuf, &outCharCount);
        Assert (outCharCount <= outBufSize);
        if (outCharCount == 0) {
            return wstring ();
        }

        // The wideBuf may be NUL-terminated or not (depending on whether the input was NUL-terminated or not).
        // Be sure to construct the resuting string with the right end-of-string pointer (the length doesn't include
        // the NUL-char)
        return wstring (wideBuf, wideBuf[outCharCount - 1] == '\0' ? (outCharCount - 1) : outCharCount);
    }
}





/*
 ********************************************************************************
 *********************** MapUNICODETextWithMaybeBOMTowstring ********************
 ********************************************************************************
 */
vector<Byte>    Characters::MapUNICODETextToSerializedFormat (const wchar_t* start, const wchar_t* end, CodePage useCP)
{
    CodePageConverter   cpc (useCP, CodePageConverter::eHandleBOM);
    size_t  outCharCount    =   cpc.MapFromUNICODE_QuickComputeOutBufSize (start, end - start);
    SmallStackBuffer<char>  buf (outCharCount);
    cpc.MapFromUNICODE (start, end - start, buf, &outCharCount);
    const Byte* bs = reinterpret_cast<const Byte*> (static_cast<const char*> (buf));
    return vector<Byte> (bs, bs + outCharCount);
}


/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */

#if     qPlatform_Windows
#include    <Windows.h>
#else
#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include    "../Debug/Trace.h"
#include    "..//Execution/Exceptions.h"

#include    "CodePage.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Characters::Platform::Windows;





/*
 ********************************************************************************
 *************** Characters::Platform::Windows::BSTRStringToUTF8 ****************
 ********************************************************************************
 */
string  Characters::Platform::Windows::BSTRStringToUTF8 (const BSTR bstr)
{
    if (bstr == nullptr) {
        return string ();
    }
    else {
        int srcStrLen   =   ::SysStringLen (bstr);
        int stringLength = ::WideCharToMultiByte (kCodePage_UTF8, 0, bstr, srcStrLen, nullptr, 0, nullptr, nullptr);
        string  result;
        result.resize (stringLength);
        Verify (::WideCharToMultiByte (kCodePage_UTF8, 0, bstr, srcStrLen, Containers::Start (result), stringLength, nullptr, nullptr) == stringLength);
        return result;
    }
}




/*
 ********************************************************************************
 *************** Characters::Platform::Windows::UTF8StringToBSTR ****************
 ********************************************************************************
 */
BSTR    Characters::Platform::Windows::UTF8StringToBSTR (const char* ws)
{
    RequireNotNull (ws);
    size_t  wsLen   =   ::strlen (ws);
    int stringLength = ::MultiByteToWideChar (CP_UTF8, 0, ws, static_cast<int> (wsLen), nullptr, 0);
    BSTR result = ::SysAllocStringLen (nullptr, stringLength);
    if (result == nullptr) {
        Execution::Throw (bad_alloc ());
    }
    Verify (::MultiByteToWideChar (kCodePage_UTF8, 0, ws, static_cast<int> (wsLen), result, stringLength) == stringLength);
    return result;
}



/*
 ********************************************************************************
 ******************* Characters::Platform::Windows::BSTR2wstring ****************
 ********************************************************************************
 */
wstring Characters::Platform::Windows::BSTR2wstring (VARIANT b)
{
    if (b.vt == VT_BSTR) {
        return BSTR2wstring (b.bstrVal);
    }
    else {
        return wstring ();
    }
}










/*
 ********************************************************************************
 **************************** PlatformCodePageConverter *************************
 ********************************************************************************
 */
void    PlatformCodePageConverter::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt) const
{
    Require (inMBCharCnt == 0 or inMBChars != nullptr);
    RequireNotNull (outCharCnt);
    Require (*outCharCnt == 0 or outChars != nullptr);
//  *outCharCnt = ::MultiByteToWideChar (fCodePage, MB_ERR_INVALID_CHARS, inMBChars, inMBCharCnt, outChars, *outCharCnt);
    *outCharCnt = ::MultiByteToWideChar (fCodePage, 0, inMBChars, static_cast<int> (inMBCharCnt), outChars, static_cast<int> (*outCharCnt));
#if 0
// enable to debug cases (e.g. caused when you read a CRLF file with fstream
// in text mode, and get - somehow - stuff that triggers this ??? - with convert to
// xml???). Anyhow - get error#102 - BUF_NOT_BIG-ENUF or osmeting like that...
//
// Debug when its happening again -- LGP 2008-09-02
    if (*outCharCnt == 0) {
        DWORD x = GetLastError ();
        int breaker = 1;
    }
#endif
}

void    PlatformCodePageConverter::MapFromUNICODE (const wchar_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt) const
{
    Require (inCharCnt == 0 or inChars != nullptr);
    RequireNotNull (outCharCnt);
    Require (*outCharCnt == 0 or outChars != nullptr);
    *outCharCnt = ::WideCharToMultiByte (fCodePage, 0, inChars, static_cast<int> (inCharCnt), outChars, static_cast<int> (*outCharCnt), nullptr, nullptr);
}









#include    "CString/Utilities.h"



using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Characters::CString;




/*
 ********************************************************************************
 ************************************* Format ***********************************
 ********************************************************************************
 */
string  Characters::CString::FormatV (const char* format, va_list argsList)
{
    RequireNotNull (format);
    Memory::SmallStackBuffer<char, 10 * 1024> msgBuf (10 * 1024);
#if     __STDC_WANT_SECURE_LIB__
    while (::vsnprintf_s (msgBuf, msgBuf.GetSize (), msgBuf.GetSize () - 1, format, argsList) < 0) {
        msgBuf.GrowToSize (msgBuf.GetSize () * 2);
    }
#else
    // SUBTLE: va_list looks like it is passed by value, but its not really,
    // and vswprintf, at least on GCC munges it. So we must use va_copy() to do this safely
    // @see http://en.cppreference.com/w/cpp/utility/variadic/va_copy
    va_list argListCopy ;
    va_copy(argListCopy, argsList);

    while (::vsnprintf (msgBuf, msgBuf.GetSize (), format, argListCopy) < 0) {
        msgBuf.GrowToSize (msgBuf.GetSize () * 2);
        va_end (argListCopy);
        va_copy (argListCopy, argsList);
    }

    va_end (argListCopy);
#endif
    Assert (::strlen (msgBuf) < msgBuf.GetSize ());
    return string (msgBuf);
}

string  Characters::CString::Format (const char* format, ...)
{
    va_list     argsList;
    va_start (argsList, format);
    string  tmp = FormatV (format, argsList);
    va_end (argsList);
    return tmp;
}

DISABLE_COMPILER_MSC_WARNING_START(6262)
wstring Characters::CString::FormatV (const wchar_t* format, va_list argsList)
{
    RequireNotNull (format);
    Memory::SmallStackBuffer<wchar_t, 10 * 1024>  msgBuf (10 * 1024);
    const   wchar_t*    useFormat   =   format;
#if     !qStdLibSprintfAssumesPctSIsWideInFormatIfWideFormat
    wchar_t     newFormat[5 * 1024];
    {
        size_t  origFormatLen   =   wcslen (format);
        Require (origFormatLen < NEltsOf (newFormat) / 2);  // just to be sure safe - this is already crazy-big for format string...
        // Could use Memory::SmallStackBuffer<> but I doubt this will ever get triggered
        bool lookingAtFmtCvt = false;
        size_t  newFormatIdx    =   0;
        for (size_t i = 0; i < origFormatLen; ++i) {
            if (lookingAtFmtCvt) {
                switch (format[i]) {
                    case    '%': {
                            lookingAtFmtCvt = false;
                        }
                        break;
                    case    's': {
                            newFormat[newFormatIdx] = 'l';
                            newFormatIdx++;
                        }
                        break;
                    case    '.': {
                            // could still be part for format string
                        }
                        break;
                    default: {
                            if (isdigit (format[i])) {
                                // could still be part for format string
                            }
                            else {
                                lookingAtFmtCvt = false;    // DONE
                            }
                        }
                        break;
                }
            }
            else {
                if (format[i] == '%') {
                    lookingAtFmtCvt = true;
                }
            }
            newFormat[newFormatIdx] = format[i];
            newFormatIdx++;
        }
        Assert (newFormatIdx >= origFormatLen);
        if (newFormatIdx > origFormatLen) {
            newFormat[newFormatIdx] = '\0';
            useFormat   =   newFormat;
        }
    }
#endif

    // SUBTLE: va_list looks like it is passed by value, but its not really,
    // and vswprintf, at least on GCC munges it. So we must use va_copy() to do this safely
    // @see http://en.cppreference.com/w/cpp/utility/variadic/va_copy
    va_list argListCopy ;
    va_copy (argListCopy, argsList);

    // Assume only reason for failure is not enuf bytes, so allocate more.
    // If I'm wrong, we'll just runout of memory and throw out...
    while (::vswprintf (msgBuf, msgBuf.GetSize (), useFormat, argListCopy) < 0) {
        msgBuf.GrowToSize (msgBuf.GetSize () * 2);
        va_end (argListCopy);
        va_copy (argListCopy, argsList);
    }
    va_end (argListCopy);
    Assert (::wcslen (msgBuf) < msgBuf.GetSize ());
    return wstring (msgBuf);
}
DISABLE_COMPILER_MSC_WARNING_END(6262)


wstring Characters::CString::Format (const wchar_t* format, ...)
{
    va_list     argsList;
    va_start (argsList, format);
    wstring tmp = FormatV (format, argsList);
    va_end (argsList);
    return tmp;
}








/*
 ********************************************************************************
 ********************************* LimitLength **********************************
 ********************************************************************************
 */
namespace   {
    template    <typename STRING>
    inline  STRING  LimitLength_HLPR (const STRING& str, size_t maxLen, bool keepLeft, const STRING& kELIPSIS)
    {
        if (str.length () <= maxLen) {
            return str;
        }
        size_t  useLen  =   maxLen;
        if (useLen > kELIPSIS.length ()) {
            useLen -= kELIPSIS.length ();
        }
        else {
            useLen = 0;
        }
        if (keepLeft) {
            return str.substr (0, useLen) + kELIPSIS;
        }
        else {
            return kELIPSIS + str.substr (str.length () - useLen);
        }
    }
}
string  Characters::CString::LimitLength (const string& str, size_t maxLen, bool keepLeft)
{
    return LimitLength_HLPR<string> (str, maxLen, keepLeft, "...");
}

wstring Characters::CString::LimitLength (const wstring& str, size_t maxLen, bool keepLeft)
{
    return LimitLength_HLPR<wstring> (str, maxLen, keepLeft, L"...");
}








/*
 ********************************************************************************
 *************************** StripTrailingCharIfAny *****************************
 ********************************************************************************
 */
namespace   {
    template    <typename STRING>
    inline  STRING  StripTrailingCharIfAny_HLPR (const STRING& str, typename STRING::value_type c)
    {
        if (str.size () > 0 and str[str.size () - 1] == c) {
            STRING  tmp =   str;
            tmp.erase (tmp.size () - 1);
            return tmp;
        }
        return str;
    }
}

string  Characters::CString::StripTrailingCharIfAny (const string& s, char c)
{
    return StripTrailingCharIfAny_HLPR (s, c);
}

wstring Characters::CString::StripTrailingCharIfAny (const wstring& s, wchar_t c)
{
    return StripTrailingCharIfAny_HLPR (s, c);
}






/*
 ********************************************************************************
 ********************************* Float2String *********************************
 ********************************************************************************
 */
wstring Characters::CString::Float2String (double f)
{
	return wstring {};
}









/*
 ********************************************************************************
 ****************************** HexString2Int ***********************************
 ********************************************************************************
 */
unsigned int Characters::CString::HexString2Int (const string& s)
{
    using   std::numeric_limits;
    unsigned    long    l   =   strtoul (s.c_str (), nullptr, 16);
    if (l >= numeric_limits<unsigned int>::max ()) {
        return numeric_limits<unsigned int>::max ();
    }
    return l;
}

unsigned int Characters::CString::HexString2Int (const wchar_t* s)
{
    using   std::numeric_limits;
    RequireNotNull (s);
    unsigned    long    l   =   wcstoul (s, nullptr, 16);
    if (l >= numeric_limits<unsigned int>::max ()) {
        return numeric_limits<unsigned int>::max ();
    }
    return l;
}

unsigned int Characters::CString::HexString2Int (const wstring& s)
{
    using   std::numeric_limits;
    unsigned    long    l   =   wcstoul (s.c_str (), nullptr, 16);
    if (l >= numeric_limits<unsigned int>::max ()) {
        return numeric_limits<unsigned int>::max ();
    }
    return l;
}






/*
 ********************************************************************************
 ********************************* String2Int ***********************************
 ********************************************************************************
 */
long long int Characters::CString::Private_::String2Int_ (const string& s)
{
    using   std::numeric_limits;
    // nothing needed todo to pin the value to min/max
    return strtoll (s.c_str (), nullptr, 10);
}

long long int Characters::CString::Private_::String2Int_ (const wstring& s)
{
    using   std::numeric_limits;
    unsigned    long long int    l   =   wcstoll (s.c_str (), nullptr, 10);
    return l;
}




/*
 ********************************************************************************
 ********************************* String2UInt ***********************************
 ********************************************************************************
 */
unsigned long long int Characters::CString::Private_::String2UInt_ (const string& s)
{
    using   std::numeric_limits;
    // nothing needed todo to pin the value to min/max
    unsigned    long long int    l = strtoull (s.c_str (), nullptr, 10);
    return l;
}

unsigned long long int Characters::CString::Private_::String2UInt_ (const wstring& s)
{
    using   std::numeric_limits;
    long long int    l   =   wcstoull (s.c_str (), nullptr, 10);
    return l;
}








/*
 ********************************************************************************
 ********************************* String2Float *********************************
 ********************************************************************************
 */
double  Characters::CString::String2Float (const string& s)
{
    char*   e   =   nullptr;
    double  d   =   strtod (s.c_str (), &e);
    return d;
}

double  Characters::CString::String2Float (const wchar_t* s)
{
    RequireNotNull (s);
    wchar_t*    e   =   nullptr;
    double  d   =   wcstod (s, &e);
    return d;
}

double  Characters::CString::String2Float (const wstring& s)
{
    wchar_t*    e   =   nullptr;
    double  d   =   wcstod (s.c_str (), &e);
    return d;
}




#include    <cmath>
#include    <cstdarg>
#include    <cstdio>
#include    <fstream>
#include    <map>
#include    <mutex>

#include    "../Characters/LineEndings.h"
#include    "../Execution/Common.h"
#include    "../Execution/Module.h"
#include    "../Execution/Thread.h"
#include    "../Memory/Common.h"
#include    "../Time/Realtime.h"

#if     qTraceToFile
#include    "../IO/FileSystem/WellKnownLocations.h"
#include    "../Time/DateTime.h"
#endif

#include    "../Debug/Trace.h"



using   namespace   Stroika::Foundation;

using   namespace   Characters;
using   namespace   Debug;
using   namespace   Execution;


using   Execution::make_unique_lock;



/*
 * TODO:
 *
 *      @todo   The buffering code here maybe now correct, but isn't simple/clear, so rewrite/improve...
 *              -- LGP 2011-10-03
 */



CompileTimeFlagChecker_SOURCE(Stroika::Foundation::Debug, qTraceToFile, qTraceToFile);
CompileTimeFlagChecker_SOURCE(Stroika::Foundation::Debug, qDefaultTracingOn, qDefaultTracingOn);



namespace   {
    // This is MOSTLY to remove NEWLINES from the MIDDLE of a message - replace with kBadChar.
    const   char    kBadChar_   =   ' ';
    void    SquishBadCharacters_ (string* s)
    {
        RequireNotNull (s);
        size_t  end =   s->length ();
        // ignore last 2 in case crlf
        if (end > 2) {
            end -= 2;
        }
        for (size_t i = 0; i < end; ++i) {
            if ((*s)[i] == '\n' or (*s)[i] == '\r') {
                (*s)[i] = kBadChar_;
            }
        }
    }
    void    SquishBadCharacters_ (wstring* s)
    {
        RequireNotNull (s);
        size_t  end =   s->length ();
        // ignore last 2 in case crlf
        if (end > 2) {
            end -= 2;
        }
        for (size_t i = 0; i < end; ++i) {
            if ((*s)[i] == '\n' or (*s)[i] == '\r') {
                (*s)[i] = kBadChar_;
            }
        }
    }
}







/*
 ********************************************************************************
 ************************ Private_::TraceModuleData_ ****************************
 ********************************************************************************
 */
namespace   {
    recursive_mutex*    sEmitTraceCritSec_   =   nullptr;
#if     qTraceToFile
    ofstream*   sTraceFile  =   nullptr;
#endif
#if     qDefaultTracingOn
    thread_local    unsigned int    sTraceContextDepth_ { 0 };  // no need for atomic access because thread_local
#endif

    // Declared HERE instead of the template so they get shared across TYPE values for CHARTYPE
    Thread::IDType  sMainThread_                =   Execution::GetCurrentThreadID ();

    string  mkPrintDashAdornment_ ()
    {
        size_t threadPrintWidth = FormatThreadID_A (sMainThread_).length () - 4;
        string result;
        result.reserve (threadPrintWidth / 2);
        for (size_t i = 0; i < threadPrintWidth / 2; ++i) {
            result.append ("-");
        }
        return result;
    }
    char        sThreadPrintDashAdornment_[32]; // use static array to avoid putting the string object into TraceModuleData_, and otherwise having to worry about use after main when calls DbgTrace()
    bool        sDidOneTimePrimaryThreadMessage_    =   false;
}





#if     qTraceToFile
namespace   {
    SDKString mkTraceFileName_ ()
    {
        // Use TempDir instead of EXEDir because on vista, installation permissions prevent us from (easily) writing in EXEDir.
        // (could fix of course, but I'm not sure desirable - reasonable defaults)
        //
        // Don't want to use TempFileLibrarian cuz we dont want these deleted on app exit
        SDKString mfname;
        {
            try {
                mfname = Execution::GetEXEPathT ();
            }
            catch (...) {
                mfname = SDKSTR ("{unknown}");
            }
            size_t i = mfname.rfind (IO::FileSystem::kPathComponentSeperator);
            if (i != SDKString::npos) {
                mfname = mfname.substr (i + 1);
            }
            i = mfname.rfind ('.');
            if (i != SDKString::npos) {
                mfname.erase (i);
            }
            for (auto i = mfname.begin (); i != mfname.end (); ++i) {
                if (*i == ' ') {
                    *i = '-';
                }
            }
        }
        SDKString nowstr  =   Time::DateTime::Now ().Format (Time::DateTime::PrintFormat::eXML).AsSDKString ();
        for (auto i = nowstr.begin (); i != nowstr.end (); ++i) {
            if (*i == ':') {
                *i = '-';
            }
        }
        return IO::FileSystem::WellKnownLocations::GetTemporaryT () + CString::Format (SDKSTR ("TraceLog_%s_PID#%d-%s.txt"), mfname.c_str (), (int)Execution::GetCurrentProcessID (), nowstr.c_str ());
    }
}
#endif

Debug::Private_::TraceModuleData_::TraceModuleData_ ()
    : fEmitter ()
    , fStringDependency (Characters::MakeModuleDependency_String ())
#if     qTraceToFile
    , fTraceFileName (mkTraceFileName_ ())
#endif
{
    CString::Copy (sThreadPrintDashAdornment_, NEltsOf (sThreadPrintDashAdornment_), mkPrintDashAdornment_ ().c_str ());
    Assert (sEmitTraceCritSec_ == nullptr);
    sEmitTraceCritSec_ = new recursive_mutex ();
#if     qTraceToFile
    Assert (sTraceFile == nullptr);
    sTraceFile = new ofstream ();
    sTraceFile->open (Emitter::Get ().GetTraceFileName ().c_str (), ios::out | ios::binary);
#endif
}

Debug::Private_::TraceModuleData_::~TraceModuleData_ ()
{
    delete sEmitTraceCritSec_;
    sEmitTraceCritSec_ = nullptr;
#if     qTraceToFile
    AssertNotNull (sTraceFile);
    sTraceFile->close ();
    delete sTraceFile;
    sTraceFile = nullptr;
#endif
}








namespace   {
    inline  recursive_mutex&    GetCritSection_ ()
    {
        // obsolete comment because as of 2014-01-31 (or earlier) we are acutallly deleting this. But we do have
        // some race where we sometimes - rarely - trigger this error. So we may need to re-instate that leak!!!
        // and this comment!!!
        //  --LGP 2014-02-01
#if 0
        // this is a 'false' or 'apparent' memory leak, but we allocate the object this way because in C++ things
        // can be destroyed in any order, (across OBJs), and though this gets destroyed late, its still possible
        // someone might do a trace message.
        //      -- LGP 2008-12-21
#endif
        EnsureNotNull (sEmitTraceCritSec_);
        return *sEmitTraceCritSec_;
    }
}



#if     qTraceToFile
SDKString Emitter::GetTraceFileName () const
{
    return Execution::ModuleInitializer<Private_::TraceModuleData_>::Actual ().fTraceFileName;
}
#endif


#if     qTraceToFile
namespace   {
    void    Emit2File_ (const char* text) noexcept
    {
        RequireNotNull (text);
        RequireNotNull (sTraceFile);
        try {
            if (sTraceFile->is_open ()) {
                (*sTraceFile) << text;
                sTraceFile->flush ();
            }
        }
        catch (...) {
            AssertNotReached ();
        }
    }
    void    Emit2File_ (const wchar_t* text) noexcept
    {
        RequireNotNull (text);
        try {
            Emit2File_ (WideStringToUTF8 (text).c_str ());
        }
        catch (...) {
            AssertNotReached ();
        }
    }
}
#endif







/*
 ********************************************************************************
 ******************** Debug::MakeModuleDependency_Trace *************************
 ********************************************************************************
 */
Execution::ModuleDependency Debug::MakeModuleDependency_Trace ()
{
    return Execution::ModuleInitializer<Debug::Private_::TraceModuleData_>::GetDependency ();
}






/*
 ********************************************************************************
 ************************************ Emitter ***********************************
 ********************************************************************************
 */
Emitter::Emitter ()
    : fLastNCharBufCharCount_ (0)
//  , fLastNCharBuf_CHAR_ ()
//  , fLastNCharBuf_WCHAR_ ()
    , fLastNCharBuf_WCHARFlag_ (false)
    , fLastNCharBuf_Token_ (0)
    , fLastNCharBuf_WriteTickcount_ (0.0f)
{
}

/*
@DESCRIPTION:   <p>This function takes a 'format' argument and then any number of additional arguments - exactly
            like std::printf (). It calls std::vsprintf () internally. This can be called directly - regardless of the
             @'qDefaultTracingOn' flag - but is typically just called indirectly by calling
             @'DbgTrace'.</p>
*/
void    Emitter::EmitTraceMessage (const char* format, ...) noexcept
{
    Thread::SuppressInterruptionInContext   suppressAborts;
    try {
        va_list     argsList;
        va_start (argsList, format);
        string  tmp =   Characters::CString::FormatV (format, argsList);
        va_end (argsList);
        SquishBadCharacters_ (&tmp);
        AssureHasLineTermination (&tmp);
        DoEmitMessage_ (0, Containers::Start (tmp), Containers::End (tmp));
    }
    catch (...) {
        Assert (false); // Should NEVER happen anymore becuase of new vsnprintf() stuff
        // Most likely indicates invalid format string for varargs parameters
        DoEmit_ (L"EmitTraceMessage FAILED internally (buffer overflow?)");
    }
}

void    Emitter::EmitTraceMessage (const wchar_t* format, ...) noexcept
{
    Thread::SuppressInterruptionInContext   suppressAborts;
    try {
        va_list     argsList;
        va_start (argsList, format);
        wstring tmp =   Characters::CString::FormatV (format, argsList);
        va_end (argsList);
        SquishBadCharacters_ (&tmp);
        AssureHasLineTermination (&tmp);
        DoEmitMessage_ (0, Containers::Start (tmp), Containers::End (tmp));
    }
    catch (...) {
        Assert (false); // Should NEVER happen anymore becuase of new vsnprintf() stuff
        // Most likely indicates invalid format string for varargs parameters
        DoEmit_ (L"EmitTraceMessage FAILED internally (buffer overflow?)");
    }
}

Emitter::TraceLastBufferedWriteTokenType    Emitter::EmitTraceMessage (size_t bufferLastNChars, const char* format, ...) noexcept
{
    Thread::SuppressInterruptionInContext   suppressAborts;
    try {
        va_list     argsList;
        va_start (argsList, format);
        string  tmp =   Characters::CString::FormatV (format, argsList);
        va_end (argsList);
        SquishBadCharacters_ (&tmp);
        AssureHasLineTermination (&tmp);
        return DoEmitMessage_ (bufferLastNChars, Containers::Start (tmp), Containers::End (tmp));
    }
    catch (...) {
        Assert (false); // Should NEVER happen anymore becuase of new vsnprintf() stuff
        // Most likely indicates invalid format string for varargs parameters
        DoEmit_ (L"EmitTraceMessage FAILED internally (buffer overflow?)");
        return 0;
    }
}

Emitter::TraceLastBufferedWriteTokenType    Emitter::EmitTraceMessage (size_t bufferLastNChars, const wchar_t* format, ...) noexcept
{
    Thread::SuppressInterruptionInContext   suppressAborts;
    try {
        va_list     argsList;
        va_start (argsList, format);
        wstring tmp =   Characters::CString::FormatV (format, argsList);
        va_end (argsList);
        SquishBadCharacters_ (&tmp);
        AssureHasLineTermination (&tmp);
        return DoEmitMessage_ (bufferLastNChars, Containers::Start (tmp), Containers::End (tmp));
    }
    catch (...) {
        Assert (false); // Should NEVER happen anymore becuase of new vsnprintf() stuff
        // Most likely indicates invalid format string for varargs parameters
        DoEmit_ (L"EmitTraceMessage FAILED internally (buffer overflow?)");
        return 0;
    }
}

namespace {
    inline  Time::DurationSecondsType   GetStartOfEpoch_ ()
    {
        // factored out of template Emitter::DoEmitMessage_ so we have ONE start of epoch - instead of one per type CHARTYPE!
        static  Time::DurationSecondsType   sStartOfTime_    =   Time::GetTickCount (); // always set just once, and threadsafe
        return sStartOfTime_;
    }
}

template    <typename   CHARTYPE>
Emitter::TraceLastBufferedWriteTokenType    Emitter::DoEmitMessage_ (size_t bufferLastNChars, const CHARTYPE* p, const CHARTYPE* e)
{
    auto    critSec { make_unique_lock (GetCritSection_ ()) };
    FlushBufferedCharacters_ ();
    Time::DurationSecondsType   curRelativeTime =   Time::GetTickCount () - GetStartOfEpoch_ ();
    {
        char    buf[1024];
        Thread::IDType  threadID    =   Execution::GetCurrentThreadID ();
        string  threadIDStr =   FormatThreadID_A (threadID);
        if (sMainThread_ == threadID) {
            Verify (::snprintf  (buf, NEltsOf (buf), "[%sMAIN%s][%08.3f]\t", sThreadPrintDashAdornment_, sThreadPrintDashAdornment_, static_cast<double> (curRelativeTime)) > 0);
            if (not sDidOneTimePrimaryThreadMessage_) {
                sDidOneTimePrimaryThreadMessage_ = true;
                char buf2[1024];
                Verify ( ::snprintf  (buf2, NEltsOf (buf2), "(REAL THREADID=%s)\t", threadIDStr.c_str ()) > 0);
#if     __STDC_WANT_SECURE_LIB__
                strcat_s (buf, buf2);
#else
                strcat (buf, buf2);
#endif
#if     qPlatform_POSIX
                Verify (::snprintf  (buf2, NEltsOf (buf2), "(pthread_self=0x%lx)\t", (unsigned long)pthread_self ()) > 0);
#if     __STDC_WANT_SECURE_LIB__
                strcat_s (buf, buf2);
#else
                strcat (buf, buf2);
#endif
#endif
            }
        }
        else {
            (void)::snprintf  (buf, NEltsOf (buf), "[%s][%08.3f]\t", threadIDStr.c_str (), static_cast<double> (curRelativeTime));
        }
        DoEmit_ (buf);
    }
#if     qDefaultTracingOn
    unsigned int    contextDepth    =   TraceContextBumper::GetCount ();
    for (unsigned int i = 0; i < contextDepth; ++i) {
        DoEmit_ (L"\t");
    }
#endif
    if (bufferLastNChars == 0) {
        DoEmit_ (p, e);
        fLastNCharBuf_Token_++; // even if not buffering, increment, so other buffers known to be invalid
    }
    else {
        Assert ((e - p) > static_cast<ptrdiff_t> (bufferLastNChars));
        BufferNChars_ (bufferLastNChars, e - bufferLastNChars);
        DoEmit_ (p, e - bufferLastNChars);
        fLastNCharBuf_WriteTickcount_ = curRelativeTime + GetStartOfEpoch_ ();
        fLastNCharBuf_Token_++; // even if not buffering, increment, so other buffers known to be invalid
    }
    return fLastNCharBuf_Token_;
}

void    Emitter::BufferNChars_ (size_t bufferLastNChars, const char* p)
{
    Assert (bufferLastNChars < NEltsOf (fLastNCharBuf_CHAR_));
    fLastNCharBufCharCount_ = bufferLastNChars;
#if     __STDC_WANT_SECURE_LIB__
    strcpy_s (fLastNCharBuf_CHAR_, p);
#else
    strcpy (fLastNCharBuf_CHAR_, p);
#endif
    fLastNCharBuf_WCHARFlag_ = false;
}

void    Emitter::BufferNChars_ (size_t bufferLastNChars, const wchar_t* p)
{
    Assert (bufferLastNChars < NEltsOf (fLastNCharBuf_WCHAR_));
    fLastNCharBufCharCount_ = bufferLastNChars;
#if     __STDC_WANT_SECURE_LIB__
    ::wcscpy_s (fLastNCharBuf_WCHAR_, p);
#else
    ::wcscpy (fLastNCharBuf_WCHAR_, p);
#endif
    fLastNCharBuf_WCHARFlag_ = true;
}

void    Emitter::FlushBufferedCharacters_ ()
{
    if (fLastNCharBufCharCount_ != 0) {
        if (fLastNCharBuf_WCHARFlag_) {
            DoEmit_ (fLastNCharBuf_WCHAR_);
        }
        else {
            DoEmit_ (fLastNCharBuf_CHAR_);
        }
        fLastNCharBufCharCount_ = 0;
    }
}

bool    Emitter::UnputBufferedCharactersForMatchingToken (TraceLastBufferedWriteTokenType token)
{
    auto    critSec { make_unique_lock (GetCritSection_ ()) };
    // If the fLastNCharBuf_Token_ matches (no new tokens written since the saved one) and the time
    // hasn't been too long (we currently write 1/100th second timestamp resolution).
    // then blank unput (ignore) buffered characters, and return true so caller knows to write
    // funky replacement for those characters.
    if (fLastNCharBuf_Token_ == token and (Time::GetTickCount () - fLastNCharBuf_WriteTickcount_ < 0.02f)) {
        fLastNCharBufCharCount_ = 0;
        return true;
    }
    return false;   // assume old behavior for now
}

void    Emitter::DoEmit_ (const char* p) noexcept
{
#if     qPlatform_Windows
    constexpr   size_t  kMaxLen_    =   1023;   // no docs on limit, but various hints the limit is somewhere between 1k and 4k. Empirically - just chops off after a point...
    if (::strlen (p) < kMaxLen_) {
        ::OutputDebugStringA (p);
    }
    else {
        char    buf[1024];  // @todo if/when we always support constexpr can use that here!
        memcpy (buf, p, sizeof (buf));
        buf[NEltsOf(buf) - 1] = 0;
        ::OutputDebugStringA (buf);
        ::OutputDebugStringA ("...");
        ::OutputDebugStringA (GetEOL<char> ());
    }
#endif
#if     qTraceToFile
    Emit2File_ (p);
#endif
}

void    Emitter::DoEmit_ (const wchar_t* p) noexcept
{
#if     qPlatform_Windows
    constexpr   size_t  kMaxLen_    =   1023;   // no docs on limit, but various hints the limit is somewhere between 1k and 4k. Empirically - just chops off after a point...
    if (::wcslen (p) < kMaxLen_) {
        ::OutputDebugStringW (p);
    }
    else {
        wchar_t buf[1024];  // @todo if/when we always support constexpr can use that here!
        memcpy (buf, p, sizeof (buf));
        buf[NEltsOf(buf) - 1] = 0;
        ::OutputDebugStringW (buf);
        ::OutputDebugStringW (L"...");
        ::OutputDebugStringW (GetEOL<wchar_t> ());
    }
#endif
#if     qTraceToFile
    Emit2File_ (p);
#endif
}

void    Emitter::DoEmit_ (const char* p, const char* e) noexcept
{
    try {
        size_t  len =   e - p;
        Memory::SmallStackBuffer<char>  buf (len + 1);
        (void)::memcpy (buf.begin (), p, len);
        buf.begin () [len] = '\0';
        DoEmit_ (buf.begin ());
    }
    catch (...) {
        AssertNotReached ();
    }
}

void    Emitter::DoEmit_ (const wchar_t* p, const wchar_t* e) noexcept
{
    try {
        size_t  len =   e - p;
        Memory::SmallStackBuffer<wchar_t>   buf (len + 1);
        (void)::memcpy (buf.begin (), p, len * sizeof (wchar_t));
        buf.begin () [len] = '\0';
        DoEmit_ (buf.begin ());
    }
    catch (...) {
        AssertNotReached ();
    }
}









/*
 ********************************************************************************
 ****************************** TraceContextBumper ******************************
 ********************************************************************************
 */
#if     qDefaultTracingOn
TraceContextBumper::TraceContextBumper (const wchar_t* contextName) noexcept
    : fDoEndMarker (true)
      //,fSavedContextName_ ()
{
    fLastWriteToken_ = Emitter::Get ().EmitTraceMessage (3 + ::wcslen (GetEOL<wchar_t> ()), L"<%s> {", contextName);
    size_t  len =   min (NEltsOf (fSavedContextName_), char_traits<wchar_t>::length (contextName));
    char_traits<wchar_t>::copy (fSavedContextName_, contextName, len);
    *(std::end (fSavedContextName_) - 1) = '\0';
    fSavedContextName_[len] = '\0';
    IncCount_ ();
}

TraceContextBumper::TraceContextBumper (const char* contextName) noexcept
    : TraceContextBumper (mkwtrfromascii_ (contextName).data ())
{
}

unsigned int    TraceContextBumper::GetCount ()
{
    return sTraceContextDepth_;
}

void    TraceContextBumper::IncCount_ () noexcept
{
    sTraceContextDepth_++;
}

void    TraceContextBumper::DecrCount_ () noexcept
{
    --sTraceContextDepth_;
}

TraceContextBumper::~TraceContextBumper ()
{
    DecrCount_ ();
    if (fDoEndMarker) {
        auto    critSec { make_unique_lock (GetCritSection_ ()) };
        if (Emitter::Get ().UnputBufferedCharactersForMatchingToken (fLastWriteToken_)) {
            Emitter::Get ().EmitUnadornedText ("/>");
            Emitter::Get ().EmitUnadornedText (GetEOL<char> ());
        }
        else {
            Emitter::Get ().EmitTraceMessage (L"} </%s>", fSavedContextName_);
        }
    }
}

auto    TraceContextBumper::mkwtrfromascii_ (const char* contextName) -> array<wchar_t, kMaxContextNameLen_> {
    array<wchar_t, kMaxContextNameLen_>  r;
    auto ci = contextName;
    for (; *ci != '\0'; ++ci)
    {
        Require (isascii (*ci));
        size_t i = ci - contextName;
        if (i < kMaxContextNameLen_ - 1) {
            r[i] = *ci;
        }
        else {
            break;
        }
    }
    Assert (ci - contextName < kMaxContextNameLen_);
    r[ci - contextName] = '\0';
    return r;
}
#endif




#include    <cmath>

#include    "../Characters/CString/Utilities.h"
#include    "../Characters/String_Constant.h"
#include    "../Debug/Assertions.h"
#include    "../Debug/Trace.h"
#include    "../Linguistics/Words.h"

#include    "../Time/Duration.h"

using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Time;

using   Characters::String_Constant;
using   Debug::TraceContextBumper;

using   namespace   Time;







/*
 ********************************************************************************
 ********************** Duration::FormatException *******************************
 ********************************************************************************
 */
Duration::FormatException::FormatException ()
    : StringException (String_Constant (L"Invalid Duration Format"))
{
}

const   Duration::FormatException   Duration::FormatException::kThe;












/*
 ********************************************************************************
 ********************* Private_::Duration_ModuleData_ ***************************
 ********************************************************************************
 */

Time::Private_::Duration_ModuleData_::Duration_ModuleData_ ()
    : fMin (numeric_limits<Duration::InternalNumericFormatType_>::lowest ())
    , fMax (numeric_limits<Duration::InternalNumericFormatType_>::max ())
{
}



/*
 ********************************************************************************
 *********************************** Duration ***********************************
 ********************************************************************************
 */
const   Duration&    Duration::kMin = Execution::ModuleInitializer<Time::Private_::Duration_ModuleData_>::Actual ().fMin;
const   Duration&    Duration::kMax = Execution::ModuleInitializer<Time::Private_::Duration_ModuleData_>::Actual ().fMax;

const   Duration::PrettyPrintInfo   Duration::kDefaultPrettyPrintInfo = {
    {
        String_Constant (L"year"), String_Constant (L"years"),
        String_Constant (L"month"), String_Constant (L"months"),
        String_Constant (L"week"), String_Constant (L"weeks"),
        String_Constant (L"day"), String_Constant (L"days"),
        String_Constant (L"hour"), String_Constant (L"hours"),
        String_Constant (L"minute"), String_Constant (L"minutes"),
        String_Constant (L"second"), String_Constant (L"seconds"),
        String_Constant (L"ms"), String_Constant (L"ms"),
        String_Constant (L"µs"), String_Constant (L"µs"),
        String_Constant (L"ns"), String_Constant (L"ns")
    }
};

const   Duration::AgePrettyPrintInfo   Duration::kDefaultAgePrettyPrintInfo = {
    {
        String_Constant (L"now"),
        String_Constant (L"ago"),
        String_Constant (L"from now"),
    },
    12 * 60 /*fNowThreshold*/
};

Duration::Duration ()
    : fDurationRep_ ()
{
}

Duration::Duration (const string& durationStr)
    : fDurationRep_ (durationStr)
{
    (void)ParseTime_ (fDurationRep_);    // call for the side-effect of throw if bad format src string
}

Duration::Duration (const String& durationStr)
    : fDurationRep_ (durationStr.AsASCII ())
{
    (void)ParseTime_ (fDurationRep_);    // call for the side-effect of throw if bad format src string
}

Duration::Duration (int duration)
    : fDurationRep_ (UnParseTime_ (static_cast<InternalNumericFormatType_> (duration)))
{
}

Duration::Duration (long duration)
    : fDurationRep_ (UnParseTime_ (static_cast<InternalNumericFormatType_> (duration)))
{
}

Duration::Duration (long long duration)
    : fDurationRep_ (UnParseTime_ (static_cast<InternalNumericFormatType_> (duration)))
{
}

Duration::Duration (float duration)
    : fDurationRep_ (UnParseTime_ (static_cast<InternalNumericFormatType_> (duration)))
{
}

Duration::Duration (double duration)
    : fDurationRep_ (UnParseTime_ (static_cast<InternalNumericFormatType_> (duration)))
{
}

Duration::Duration (long double duration)
    : fDurationRep_ (UnParseTime_ (static_cast<InternalNumericFormatType_> (duration)))
{
}

Duration&   Duration::operator+= (const Duration& rhs)
{
    *this = *this + rhs;
    return *this;
}

void    Duration::clear ()
{
    fDurationRep_.clear ();
}

bool    Duration::empty () const
{
    return fDurationRep_.empty ();
}

template    <>
int  Duration::As () const
{
    return static_cast<int> (ParseTime_ (fDurationRep_));       // could cache value, but ... well - maybe not worth the effort/cost of extra data etc.
}

template    <>
long int  Duration::As () const
{
    return static_cast<long int> (ParseTime_ (fDurationRep_));      // could cache value, but ... well - maybe not worth the effort/cost of extra data etc.
}

template    <>
long long int  Duration::As () const
{
    return static_cast<long long int> (ParseTime_ (fDurationRep_));     // could cache value, but ... well - maybe not worth the effort/cost of extra data etc.
}

template    <>
float  Duration::As () const
{
    return static_cast<float> (ParseTime_ (fDurationRep_));     // could cache value, but ... well - maybe not worth the effort/cost of extra data etc.
}

template    <>
double  Duration::As () const
{
    return ParseTime_ (fDurationRep_);                           // could cache value, but ... well - maybe not worth the effort/cost of extra data etc.
}

template    <>
long double  Duration::As () const
{
    return ParseTime_ (fDurationRep_);                           // could cache value, but ... well - maybe not worth the effort/cost of extra data etc.
}

#if     qCompilerAndStdLib_TemplateSpecializationInAnyNS_Buggy
namespace   Stroika {
    namespace   Foundation {
        namespace   Time {
#endif

            template    <>
            std::chrono::duration<double>  Duration::As () const
            {
                return std::chrono::duration<double> (ParseTime_ (fDurationRep_));
            }
            template    <>
            std::chrono::seconds  Duration::As () const
            {
                return std::chrono::seconds (static_cast<std::chrono::seconds::rep> (ParseTime_ (fDurationRep_)));
            }
            template    <>
            std::chrono::milliseconds  Duration::As () const
            {
                return std::chrono::milliseconds (static_cast<std::chrono::milliseconds::rep> (ParseTime_ (fDurationRep_) * 1000));
            }
            template    <>
            std::chrono::microseconds  Duration::As () const
            {
                return std::chrono::microseconds (static_cast<std::chrono::microseconds::rep> (ParseTime_ (fDurationRep_) * 1000 * 1000));
            }
            template    <>
            std::chrono::nanoseconds  Duration::As () const
            {
                return std::chrono::nanoseconds (static_cast<std::chrono::nanoseconds::rep> (ParseTime_ (fDurationRep_) * 1000.0 * 1000.0 * 1000.0));
            }
            template    <>
            String  Duration::As () const
            {
                return ASCIIStringToWide (fDurationRep_);
            }

#if     qCompilerAndStdLib_TemplateSpecializationInAnyNS_Buggy
        }
    }
}
#endif

template    <>
wstring Duration::As () const
{
    return ASCIIStringToWide (fDurationRep_);
}

namespace   {
    string::const_iterator  SkipWhitespace_ (string::const_iterator i, string::const_iterator end)
    {
        // GNU LIBC code (header) says that whitespace is allowed (though I've found no external docs to support this).
        // Still - no harm in accepting this - so long as we don't ever generate it...
        while (i != end and isspace (*i)) {
            ++i;
        }
        Ensure (i <= end);
        return i;
    }
    string::const_iterator  FindFirstNonDigitOrDot_ (string::const_iterator i, string::const_iterator end)
    {
        while (i != end and (isdigit (*i) or * i == '.')) {
            ++i;
        }
        Ensure (i <= end);
        return i;
    }

    const   time_t  kSecondsPerMinute   =   60;
    const   time_t  kSecondsPerHour     =   kSecondsPerMinute * 60;
    const   time_t  kSecondsPerDay      =   kSecondsPerHour * 24;
    const   time_t  kSecondsPerWeek     =   kSecondsPerDay * 7;
    const   time_t  kSecondsPerMonth    =   kSecondsPerDay * 30;
    const   time_t  kSecondsPerYear     =   kSecondsPerDay * 365;
}

String Duration::PrettyPrint (const PrettyPrintInfo& prettyPrintInfo) const
{
    return String{};
}

Characters::String Duration::Format (const PrettyPrintInfo& prettyPrintInfo) const
{
    return PrettyPrint (prettyPrintInfo);
}

Characters::String  Duration::ToString () const
{
    return Format ();
}

Characters::String Duration::PrettyPrintAge (const AgePrettyPrintInfo& agePrettyPrintInfo, const PrettyPrintInfo& prettyPrintInfo) const
{
    return String{};
}

Duration    Duration::operator- () const
{
    wstring tmp =   As<wstring> ();
    if (tmp.empty ()) {
        return *this;
    }
    if (tmp[0] == '-') {
        return Duration (tmp.substr (1));
    }
    else {
        return Duration (L"-" + tmp);
    }
}

int Duration::Compare (const Duration& rhs) const
{
    Duration::InternalNumericFormatType_    n   =   As<Duration::InternalNumericFormatType_> () - rhs.As<Duration::InternalNumericFormatType_> ();
    if (n < 0) {
        return -1;
    }
    if (n > 0) {
        return 1;
    }
    return 0;
}

Duration::InternalNumericFormatType_    Duration::ParseTime_ (const string& s)
{
    //Debug::TraceContextBumper   ctx ("Duration::ParseTime_");
    //DbgTrace ("(s = %s)", s.c_str ());
    if (s.empty ()) {
        return 0;
    }
    InternalNumericFormatType_  curVal  =   0;
    bool    isNeg   =   false;
    // compute and throw if bad...
    string::const_iterator  i   =   SkipWhitespace_ (s.begin (), s.end ());
    if  (*i == '-') {
        isNeg = true;
        i = SkipWhitespace_ (i + 1, s.end ());
    }
    if (*i == 'P') {
        i = SkipWhitespace_ (i + 1, s.end ());
    }
    else {
        Execution::Throw (FormatException::kThe);
    }
    bool    timePart    =   false;
    while (i != s.end ()) {
        if (*i == 'T') {
            timePart = true;
            i = SkipWhitespace_ (i + 1, s.end ());
            continue;
        }
        string::const_iterator  firstDigitI =   i;
        string::const_iterator  lastDigitI  =   FindFirstNonDigitOrDot_ (i, s.end ());
        if (lastDigitI == s.end ()) {
            Execution::Throw (FormatException::kThe);
        }
        if (firstDigitI == lastDigitI) {
            Execution::Throw (FormatException::kThe);
        }
        /*
         *  According to http://en.wikipedia.org/wiki/ISO_8601
         *      "The smallest value used may also have a decimal fraction, as in "P0.5Y" to indicate
         *      half a year. This decimal fraction may be specified with either a comma or a full stop,
         *      as in "P0,5Y" or "P0.5Y"."
         *
         *  @todo   See todo in header: the first/last digit range could use '.' or ',' and I'm not sure atof is as flexible
         *  test/verify!!!
         */
        InternalNumericFormatType_  n   =   atof (string (firstDigitI, lastDigitI).c_str ());
        switch (*lastDigitI) {
            case    'Y':
                curVal += n * kSecondsPerYear;
                break;
            case    'M':
                curVal += n * (timePart ? kSecondsPerMinute : kSecondsPerMonth);
                break;
            case    'W':
                curVal += n * kSecondsPerWeek;
                break;
            case    'D':
                curVal += n * kSecondsPerDay;
                break;
            case    'H':
                curVal += n * kSecondsPerHour;
                break;
            case    'S':
                curVal += n;
                break;
        }
        i = SkipWhitespace_ (lastDigitI + 1, s.end ());
    }
    return isNeg ? -curVal : curVal;
}

namespace {
    // take 3.1340000 and return 3.13
    // take 300 and return 300
    // take 300.0 and return 300
    //
    void    TrimTrailingZerosInPlace_ (char* sWithMaybeTrailingZeros)
    {
        RequireNotNull (sWithMaybeTrailingZeros);
        char*   pDot = sWithMaybeTrailingZeros;
        for (; *pDot != '.' and * pDot != '\0'; ++pDot)
            ;
        Assert (*pDot == '\0' or * pDot == '.');
        if (*pDot != '\0') {
            char*   pPastDot = pDot + 1;
            char*   pPastLastZero = pPastDot + ::strlen (pPastDot);
            Assert (*pPastLastZero == '\0');
            for (; (pPastLastZero - 1) > pPastDot; --pPastLastZero) {
                Assert (sWithMaybeTrailingZeros + 1 <= pPastLastZero);  // so ptr ref always valid
                if (*(pPastLastZero - 1) == '0') {
                    *(pPastLastZero - 1) = '\0';
                }
                else {
                    break;
                }
            }
            if (strcmp (pDot, ".0") == 0) {
                *pDot = '\0';
            }
        }
    }
#if     qDebug
    struct Tester_ {
        Tester_ ()
        {
            {
                char buf[1024] = "3.1340000";
                TrimTrailingZerosInPlace_ (buf);
                Assert (string (buf) == "3.134");
            }
            {
                char buf[1024] = "300";
                TrimTrailingZerosInPlace_ (buf);
                Assert (string (buf) == "300");
            }
            {
                char buf[1024] = "300.0";
                TrimTrailingZerosInPlace_ (buf);
                Assert (string (buf) == "300");
            }
        }
    }   s_Tester_;
#endif
}

#if     qCompilerAndStdLib_GCC_48_OptimizerBug
// This code fails with -O2 or greater! Tried to see which particular optimization failed but not obvious...
#pragma GCC push_options
#pragma GCC optimize ("O0")
#endif
string  Duration::UnParseTime_ (InternalNumericFormatType_ t)
{
    //Debug::TraceContextBumper   ctx ("Duration::UnParseTime_");
    //DbgTrace ("(t = %f)", t);
    bool                        isNeg       =   (t < 0);
    InternalNumericFormatType_  timeLeft    =   t < 0 ? -t : t;
    string  result;
    result.reserve (50);
    if (isNeg) {
        result += "-";
    }
    result += "P";
    if (timeLeft >= kSecondsPerYear) {
        InternalNumericFormatType_    nYears = trunc (timeLeft / kSecondsPerYear);
        Assert (nYears > 0.0);
        if (nYears > 0.0) {
            char buf[10 * 1024];
            (void)snprintf (buf, sizeof (buf), "%.0LfY", static_cast<long double> (nYears));
            result += buf;
            timeLeft -= nYears * kSecondsPerYear;
            if (std::isinf (timeLeft) or timeLeft < 0) {
                // some date numbers are so large, we cannot compute a number of days, weeks etc
                // Also, for reasons which elude me (e.g. 32 bit gcc builds) this can go negative.
                // Not strictly a bug (I don't think). Just roundoff.
                timeLeft = 0.0;
            }
        }
    }
    Assert (0.0 <= timeLeft and timeLeft < kSecondsPerYear);
    if (timeLeft >= kSecondsPerMonth) {
        unsigned int    nMonths = static_cast<unsigned int> (timeLeft / kSecondsPerMonth);
        if (nMonths != 0) {
            char buf[1024];
            (void)snprintf (buf, sizeof (buf), "%dM", nMonths);
            result += buf;
            timeLeft -= nMonths * kSecondsPerMonth;
        }
    }
    Assert (0.0 <= timeLeft and timeLeft < kSecondsPerMonth);
    if (timeLeft >= kSecondsPerDay) {
        unsigned int    nDays = static_cast<unsigned int> (timeLeft / kSecondsPerDay);
        if (nDays != 0) {
            char buf[1024];
            (void)snprintf (buf, sizeof (buf), "%dD", nDays);
            result += buf;
            timeLeft -= nDays * kSecondsPerDay;
        }
    }
    Assert (0.0 <= timeLeft and timeLeft < kSecondsPerDay);
    if (timeLeft > 0) {
        result += "T";
        if (timeLeft >= kSecondsPerHour) {
            unsigned int    nHours = static_cast<unsigned int> (timeLeft / kSecondsPerHour);
            if (nHours != 0) {
                char buf[1024];
                (void)snprintf (buf, sizeof (buf), "%dH", nHours);
                result += buf;
                timeLeft -= nHours * kSecondsPerHour;
            }
        }
        Assert (0.0 <= timeLeft and timeLeft < kSecondsPerHour);
        if (timeLeft >= kSecondsPerMinute) {
            unsigned int    nMinutes = static_cast<unsigned int> (timeLeft / kSecondsPerMinute);
            if (nMinutes != 0) {
                char buf[1024];
                (void)snprintf (buf, sizeof (buf), "%dM", nMinutes);
                result += buf;
                timeLeft -= nMinutes * kSecondsPerMinute;
            }
        }
        Assert (0.0 <= timeLeft and timeLeft < kSecondsPerMinute);
        if (timeLeft > 0.0) {
            char buf[10 * 1024];
            buf[0] = '\0';
            // We used to use 1000, but that failed silently on AIX 7.1/pcc. And its a waste anyhow.
            // I'm pretty sure we never need more than 20 or so digits here. And it wastes time.
            // (100 works on AIX 7.1/gcc 4.9.2).
            //
            // Pick a slightly more aggressive number for now, to avoid the bugs/performance cost,
            // and eventually totally rewrite how we handle this.
            Verify (::snprintf (buf, sizeof (buf), "%.50f", static_cast<double> (timeLeft)) >= 52);
            TrimTrailingZerosInPlace_ (buf);
            result += buf;
            result += "S";
        }
    }
    if (result.length () == 1) {
        result += "T0S";
    }
    return result;
}
#if     qCompilerAndStdLib_GCC_48_OptimizerBug
#pragma GCC pop_options
#endif





/*
 ********************************************************************************
 *************************** Time operators *************************************
 ********************************************************************************
 */
Duration    Time::operator+ (const Duration& lhs, const Duration& rhs)
{
    // @todo - this convers to/from floats. This could be done more efficiently, and less lossily...
    return Duration (lhs.As<Time::DurationSecondsType> () + rhs.As<DurationSecondsType> ());
}







#include    <algorithm>

#if     qPlatform_Windows
#include    <Windows.h>
#elif   qPlatform_POSIX
#include    <time.h>
#endif

#include    "../Debug/Assertions.h"
#include    "../Memory/SmallStackBuffer.h"

#include    "../Time/Realtime.h"

using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Time;










/*
 ********************************************************************************
 ************************* Time::GetTickCount ***********************************
 ********************************************************************************
 */
namespace {
    inline  DurationSecondsType GetTickCount_ () noexcept
    {
#if     qPlatform_MacOS
        return (DurationSecondsType (::TickCount ()) / 60.0);
#elif   qPlatform_POSIX
        timespec ts;
        Verify (::clock_gettime (CLOCK_MONOTONIC, &ts) == 0);
        return ts.tv_sec + DurationSecondsType (ts.tv_nsec) / (1000.0 * 1000.0 * 1000.0);
#elif   qPlatform_Windows
        static  DurationSecondsType sPerformanceFrequencyBasis_ = [] () -> DurationSecondsType {
            LARGE_INTEGER   performanceFrequency;
            if (::QueryPerformanceFrequency (&performanceFrequency) == 0)
            {
                return 0.0;
            }
            else {
                return static_cast<DurationSecondsType> (performanceFrequency.QuadPart);
            }
        } ();
        if (sPerformanceFrequencyBasis_ == 0.0) {
#if     (_WIN32_WINNT >= 0x0600)
            return (DurationSecondsType (::GetTickCount64 ()) / 1000.0);
#else
            DISABLE_COMPILER_MSC_WARNING_START(28159)
            return (DurationSecondsType (::GetTickCount ()) / 1000.0);
            DISABLE_COMPILER_MSC_WARNING_END(28159)
#endif
        }
        LARGE_INTEGER   counter;
        Verify (::QueryPerformanceCounter (&counter));
        return static_cast<DurationSecondsType> (counter.QuadPart) / sPerformanceFrequencyBasis_;
#else
        return ::time (0);    //tmphack... not good but better than assert error
#endif
    }
}
DurationSecondsType Stroika::Foundation::Time::GetTickCount () noexcept
{
    static  const   DurationSecondsType kFirstTC_   =   GetTickCount_ ();
    return GetTickCount_ () - kFirstTC_;
}










/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <cassert>
#include    <cstdlib>

#include    "../Debug/Trace.h"

#include    "../Debug/Assertions.h"

#if     qPlatform_POSIX
#include    <cstdio>
#endif


using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Debug;




CompileTimeFlagChecker_SOURCE(Stroika::Foundation::Debug, qDebug, qDebug);




#if     qDebug

namespace   {
    void    DefaultAssertionHandler_ (const char* assertCategory, const char* assertionText, const char* fileName, int lineNum, const char* functionName)
    {
        DbgTrace ("%s (%s) failed in '%s'; %s:%d",
                  assertCategory == nullptr ? "Unknown assertion" : assertCategory,
                  assertionText == nullptr ? "" : assertionText,
                  functionName == nullptr ? "" : functionName,
                  fileName == nullptr ? "" : fileName,
                  lineNum
                 );
#if     qPlatform_POSIX
        fprintf (stderr, "%s (%s) failed in '%s'; %s:%d\n",
                 assertCategory == nullptr ? "Unknown assertion" : assertCategory,
                 assertionText == nullptr ? "" : assertionText,
                 functionName == nullptr ? "" : functionName,
                 fileName == nullptr ? "" : fileName,
                 lineNum
                );
#endif
        DbgTrace ("ABORTING...");
#if     qPlatform_POSIX
        fprintf (stderr, "ABORTING...\n");
#endif
        abort ();   // if we ever get that far...
    }
}

namespace {
    AssertionHandlerType    sAssertFailureHandler_      =   DefaultAssertionHandler_;
}


AssertionHandlerType    Stroika::Foundation::Debug::GetAssertionHandler ()
{
    return sAssertFailureHandler_;
}

AssertionHandlerType    Stroika::Foundation::Debug::GetDefaultAssertionHandler ()
{
    return DefaultAssertionHandler_;
}

void    Stroika::Foundation::Debug::SetAssertionHandler (AssertionHandlerType assertionHandler)
{
    sAssertFailureHandler_ = (assertionHandler == nullptr) ? DefaultAssertionHandler_ : assertionHandler;
}

DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Winvalid-noreturn\"");
// Cannot figure out how to disable this warning? -- LGP 2014-01-04
//DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Wenabled-by-default\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
[[noreturn]]    void    Stroika::Foundation::Debug::Private::Debug_Trap_ (const char* assertCategory, const char* assertionText, const char* fileName, int lineNum, const char* functionName)
{
    static  bool    s_InTrap    =   false;
    if (s_InTrap) {
        // prevent infinite looping if we get an assertion triggered while processing an assertion.
        // And ignore threading issues, because we are pragmatically aborting at this stage anyhow...
        abort ();
    }
    s_InTrap = true;
    try {
        (sAssertFailureHandler_) (assertCategory, assertionText, fileName, lineNum, functionName);
        s_InTrap = false;   // in case using some sort of assertion handler that allows for continuation
    }
    catch (...) {
        s_InTrap = false;   // in case using some sort of assertion handler that allows for continuation
        throw;
    }
}
//DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Wenabled-by-default\"");
DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Winvalid-noreturn\"");
#endif









#include    <algorithm>
#include    <climits>
#include    <string>

#include    "../Containers/Common.h"
#include    "../Execution/Exceptions.h"
#include    "../Memory/Common.h"
#include    "../Memory/BlockAllocated.h"

#include    "Concrete/Private/String_BufferedStringRep.h"

#include    "Concrete/String_ExternalMemoryOwnership_ApplicationLifetime.h"





using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Characters::Concrete;


using   Traversal::IteratorOwnerID;



#if 0
namespace   {
    class   String_BufferedArray_Rep_ : public Concrete::Private::BufferedStringRep::_Rep {
    private:
        using   inherited   =   Concrete::Private::BufferedStringRep::_Rep;
    public:
        String_BufferedArray_Rep_ (const wchar_t* start, const wchar_t* end)
            : inherited (start, end)
        {
        }
        virtual _IterableSharedPtrIRep   Clone (IteratorOwnerID forIterableEnvelope) const override
        {
            // Because of 'Design Choice - Iterable<T> / Iterator<T> behavior' in String class docs - we
            // ignore suggested IteratorOwnerID
            return Traversal::Iterable<Character>::MakeSharedPtr<String_BufferedArray_Rep_> (_fStart, _fEnd);
        }
    public:
        DECLARE_USE_BLOCK_ALLOCATION(String_BufferedArray_Rep_);
    };
}
#endif




class   String_ExternalMemoryOwnership_ApplicationLifetime::MyRep_ : public String::_IRep {
private:
    using   inherited   =   String::_IRep;
public:
    MyRep_ (const wchar_t* start, const wchar_t* end)
        : inherited (start, end)
    {
        Require (start + ::wcslen (start) == end);
    }
    virtual _IterableSharedPtrIRep   Clone (IteratorOwnerID forIterableEnvelope) const override
    {
        /*
         * Subtle point. If we are making a clone, its cuz caller wants to change the buffer, and they cannot cuz its readonly, so
         * make a rep that is modifyable
         */
        return Traversal::Iterable<Character>::MakeSharedPtr<String_BufferedArray_Rep_> (_fStart, _fEnd);
    }
    virtual const wchar_t*  c_str_peek () const  noexcept override
    {
        // This class ALWAYS constructed with String_ExternalMemoryOwnership_ApplicationLifetime and ALWAYS with NUL-terminated string
        Assert (_fStart + ::wcslen (_fStart) == _fEnd);
        return _fStart;
    }
public:
    DECLARE_USE_BLOCK_ALLOCATION(MyRep_);
};









/*
 ********************************************************************************
 ************** String_ExternalMemoryOwnership_ApplicationLifetime **************
 ********************************************************************************
 */
String_ExternalMemoryOwnership_ApplicationLifetime::String_ExternalMemoryOwnership_ApplicationLifetime (const wchar_t* start, const wchar_t* end)
    : inherited (_SharedPtrIRep (new MyRep_ (start, end)))
{
    Require (*end == '\0');
    Require (end == start + ::wcslen (start));  // require standard C-string
}









#include    "../Execution/Thread.h"

#include    "../Memory/BlockAllocator.h"






using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Memory;
using   namespace   Stroika::Foundation::Memory::Private_;
using   namespace   Stroika::Foundation::Execution;

using   namespace   Execution;



#if     !qStroika_Foundation_Memory_BlockAllocator_UseLockFree_
Memory::Private_::LockType_*    Memory::Private_::sLock_  =   nullptr;
#endif


#if     !qStroika_Foundation_Memory_BlockAllocator_UseLockFree_
/*
 ********************************************************************************
 *********************** BlockAllocator_ModuleInit_ *****************************
 ********************************************************************************
 */
BlockAllocator_ModuleInit_::BlockAllocator_ModuleInit_ ()
{
    Require (sLock_ == nullptr);
    sLock_ = new Private_::LockType_ ();
}

BlockAllocator_ModuleInit_::~BlockAllocator_ModuleInit_ ()
{
    RequireNotNull (sLock_);
    delete sLock_;
    sLock_ = nullptr;
}
#endif





/*
 ********************************************************************************
 ************** Memory::MakeModuleDependency_BlockAllocator *********************
 ********************************************************************************
 */
Execution::ModuleDependency Memory::MakeModuleDependency_BlockAllocator ()
{
#if     qStroika_Foundation_Memory_BlockAllocator_UseLockFree_
    return Execution::ModuleDependency ([] () {}, [] () {});
#else
    return Execution::ModuleInitializer<Private_::BlockAllocator_ModuleInit_>::GetDependency ();
#endif
}



/*
 ********************************************************************************
 *********** Memory::Private_::DoDeleteHandlingLocksExceptionsEtc_ **************
 ********************************************************************************
 */
#if     !qStroika_Foundation_Memory_BlockAllocator_UseLockFree_
void    Memory::Private_::DoDeleteHandlingLocksExceptionsEtc_ (void* p, void** staticNextLinkP) noexcept
{
    /*
     *  Logically this just does a lock acquire and assginemnt through pointers (swap). But
     *  it checks for thread abort exceptions, and supresses that if needed, since this is noexcept
     *  and can be used in DTOR. You can interrupt (abort) a thread while it deletes things.
     */
    try {
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
        MACRO_LOCK_GUARD_CONTEXT (Private_::GetLock_ ());
#else
        auto    critSec  { make_unique_lock (Private_::GetLock_ ()) };
#endif
        // push p onto the head of linked free list
        (*(void**)p) = *staticNextLinkP;
        * staticNextLinkP = p;
    }
    catch (const Execution::Thread::InterruptException&) {
        Execution::Thread::SuppressInterruptionInContext  suppressContext;
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
        MACRO_LOCK_GUARD_CONTEXT (Private_::GetLock_ ());
#else
        auto    critSec  { make_unique_lock (Private_::GetLock_ ()) };
#endif
        // push p onto the head of linked free list
        (*(void**)p) = *staticNextLinkP;
        *staticNextLinkP = p;
    }
}
#endif










#include    "../Characters/String_Constant.h"

#include    "../Execution/TimeOutException.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;

using   Characters::String_Constant;


/*
 ********************************************************************************
 ********************************* TimeOutException *****************************
 ********************************************************************************
 */
const   TimeOutException    TimeOutException::kThe;

TimeOutException::TimeOutException ()
    : StringException (String_Constant (L"Timeout Expired"))
{
}

TimeOutException::TimeOutException (const Characters::String& message)
    : StringException (message)
{
}








#include    "../Execution/Thread.h"

#include    "../Execution/SharedStaticData.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;


/*
 ********************************************************************************
 ****************** Execution::Private_::SharedStaticData_DTORHelper_ ***********
 ********************************************************************************
 */
bool    Execution::Private_::SharedStaticData_DTORHelper_ (
#if     qStroika_Foundation_Execution_SpinLock_IsFasterThan_mutex
    SpinLock* m,
#else
    mutex* m,
#endif
    unsigned int* cu
)
{
    Thread::SuppressInterruptionInContext   suppressAborts;
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
    MACRO_LOCK_GUARD_CONTEXT (*m);
#else
    auto    critSec { make_unique_lock (*m) };
#endif
    --(*cu);
    if (*cu == 0) {
        return true;
    }
    return false;

}








/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Time/Duration.h"

#include    "../Execution/Common.h"
#include    "../Execution/TimeOutException.h"

#include    "../Execution/WaitableEvent.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;


using   Stroika::Foundation::Time::Duration;


// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1



/*
 * Design notes:
 *
 *      o   The use of condition variables is non-obvious. I haven't found good documentation, but
 *          the best I've found would be
 *              https://computing.llnl.gov/tutorials/pthreads/#ConVarOverview
 *
 *          In particular, on the surface, it looks like the mutex locks in Wait() and signal should prevent things
 *          form working (deadlock). But they apparently do not cause a deadlock because
 *              "pthread_cond_wait() blocks the calling thread until the specified condition is signalled.
 *               This routine should be called while mutex is locked, and it will automatically release the
 *               mutex while it waits. After signal is received and thread is awakened, mutex will be
 *               automatically locked for use by the thread. The programmer is then responsible for
 *               unlocking mutex when the thread is finished with it."
 *
 */



/*
 ********************************************************************************
 ****************************** WaitableEvent::WE_ ******************************
 ********************************************************************************
 */
void    WaitableEvent::WE_::WaitUntil (Time::DurationSecondsType timeoutAt)
{
    if (WaitUntilQuietly (timeoutAt) == kTIMEOUTBoolResult) {
        // note - safe use of TimeOutException::kThe because you cannot really wait except when threads are running, so
        // inside 'main' lifetime
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
        // only thing Throw() helper does is DbgTrace ()- and that can make traces hard to read unless you are debugging a timeout /event issue
        Throw (TimeOutException::kThe);
#else
        throw (TimeOutException::kThe);
#endif
    }
}

bool    WaitableEvent::WE_::WaitUntilQuietly (Time::DurationSecondsType timeoutAt)
{
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx ("WaitableEvent::WE_::WaitUntil");
    DbgTrace ("(timeout = %.2f)", timeoutAt);
#endif
    CheckForThreadInterruption ();
    if (timeoutAt <= Time::GetTickCount ()) {
        return kTIMEOUTBoolResult;
    }

    /*
     *  Note - this unique_lock<> looks like a bug, but is not. Internally, fConditionVariable_.wait_for does an
     *  unlock.
     */
    std::unique_lock<mutex>     lock (fMutex);
    /*
     * The reason for the loop is that fConditionVariable_.wait_for() can return for things like errno==EINTR,
     * but must keep waiting. wait_for () returns no_timeout if for a real reason (notify called) OR spurious.
     */
    while (not fTriggered) {
        CheckForThreadInterruption ();
        Time::DurationSecondsType   remaining   =   timeoutAt - Time::GetTickCount ();
        if (remaining < 0) {
            return kTIMEOUTBoolResult;
        }

        /*
         *  See WaitableEvent::SetThreadAbortCheckFrequency ();
         */
        remaining = min (remaining, fThreadAbortCheckFrequency);

        if (fConditionVariable.wait_for (lock, Time::Duration (remaining).As<std::chrono::milliseconds> ()) == std::cv_status::timeout) {
            /*
             *  Cannot throw here because we trim time to wait so we can re-check for thread aborting. No need to pay attention to
             *  this timeout value (or any return code) - cuz we re-examine fTriggered and tickcount.
             *
             *      Throw (TimeOutException::kThe);
             */
        }
    }
    if (fResetType == eAutoReset) {
        // cannot call Reset () directly because we (may???) already have the lock mutex? Maybe not cuz of cond variable?
        fTriggered = false ;   // autoreset
    }
    return not kTIMEOUTBoolResult;
}







/*
 ********************************************************************************
 ********************************** WaitableEvent *******************************
 ********************************************************************************
 */
#if     qDebug || qStroika_FeatureSupported_Valgrind
WaitableEvent::~WaitableEvent ()
{
    Assert (fExtraWaitableEvents_.empty ());    // Cannot kill a waitable event while its being waited on by others
}
#endif

void    WaitableEvent::Set ()
{
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx ("WaitableEvent::Set");
#endif
    fWE_.Set ();
#if     qExecution_WaitableEvent_SupportWaitForMultipleObjects
    auto    critSec { make_unique_lock (_Stroika_Foundation_Execution_Private_WaitableEvent_ModuleInit_.Actual ().fExtraWaitableEventsMutex_) };
    for (auto i : fExtraWaitableEvents_) {
        i->Set ();
    }
#endif
}
