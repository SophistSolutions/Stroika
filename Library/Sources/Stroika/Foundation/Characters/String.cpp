/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <algorithm>
#include    <cstdarg>
#include    <climits>
#include    <istream>
#include    <string>
#include    <regex>

#include    "../Containers/Common.h"
#include    "../Execution/Exceptions.h"
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
            return _SharedPtrIRep (DEBUG_NEW String_BufferedArray_Rep_ (_fStart, _fEnd));
        }
    public:
        DECLARE_USE_BLOCK_ALLOCATION(String_BufferedArray_Rep_);
    };
}













namespace   {
    struct String_Substring_ : public String {
        class   MyRep_ : public _IRep {
            using inherited = _IRep;
        public:
            MyRep_ (const _SafeRepAccessor& savedSP, const wchar_t* start, const wchar_t* end)
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
                return _IterableSharedPtrIRep (DEBUG_NEW MyRep_ (*this));
            }
        private:
            _SafeRepAccessor  fSaved_;
        public:
            DECLARE_USE_BLOCK_ALLOCATION(MyRep_);
        };
    };
}











namespace   {
    inline  constexpr   regex_constants::syntax_option_type mkOption_ (RegularExpression::SyntaxType st)
    {
        return (st == RegularExpression::SyntaxType::eECMAScript ? regex_constants::ECMAScript : regex_constants::basic);
    }
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
            return SharedIRepPtr (new MyIterRep_ (fStr, fCurIdx));
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
        virtual bool    Equals (const IRep* rhs) const
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
    return Iterator<Character> (Iterator<Character>::SharedIRepPtr (new MyIterRep_ (dynamic_pointer_cast<_SharedPtrIRep::element_type> (const_cast<String::_IRep*> (this)->shared_from_this ()))));
#else
    return Iterator<Character> (Iterator<Character>::SharedIRepPtr (new MyIterRep_ (const_cast<String::_IRep*> (this)->shared_from_this ())));
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

String  String::FromNarrowSDKString (const string& from)
{
    // @todo FIX PERFORMANCE
    return NarrowSDKStringToWide (from);
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
    static  _SharedPtrIRep s_   =   mk_ (nullptr, nullptr, 1);  /// so room for NUL-char
    return s_;
}

String::_SharedPtrIRep  String::mk_ (const wchar_t* start, const wchar_t* end)
{
    return _SharedPtrIRep (DEBUG_NEW String_BufferedArray_Rep_ (start, end));
}

String::_SharedPtrIRep  String::mk_ (const wchar_t* start, const wchar_t* end, size_t reserveLen)
{
    return _SharedPtrIRep (DEBUG_NEW String_BufferedArray_Rep_ (start, end, reserveLen));
}

String::_SharedPtrIRep  String::mk_ (const wchar_t* start1, const wchar_t* end1, const wchar_t* start2, const wchar_t* end2)
{
    size_t  len1        =   end1 - start1;
    size_t  totalLen    =   len1 + (end2 - start2);
    Traversal::IterableBase::_USING_SHARED_IMPL_<String_BufferedArray_Rep_> sRep { DEBUG_NEW String_BufferedArray_Rep_ (start1, end1, totalLen) };
    sRep->InsertAt (reinterpret_cast<const Character*> (start2), reinterpret_cast<const Character*> (end2), len1);
    return sRep;
}

String  String::operator+ (const String& rhs) const
{
    _SafeRepAccessor    thisAccessor (*this);
    pair<const Character*, const Character*> lhsD   =   thisAccessor._ConstGetRep ().GetData ();
    size_t  lhsLen  =   lhsD.second - lhsD.first;
    if (lhsLen == 0) {
        return rhs;
    }
    _SafeRepAccessor    rhsAccessor (rhs);
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

String  String::operator+ (const wchar_t* appendageCStr) const
{
    RequireNotNull (appendageCStr);
    _SafeRepAccessor    thisAccessor (*this);
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

void    String::InsertAt (const Character* from, const Character* to, size_t at)
{
    // DEPRECATED
    Require (at >= 0);
    Require (at <= GetLength ());
    Require (from <= to);
    Require (from != nullptr or from == to);
    Require (to != nullptr or from == to);
    if (from == to) {
        return;
    }
    _SafeRepAccessor copyAccessor { *this };
    pair<const Character*, const Character*> d = copyAccessor._ConstGetRep ().GetData ();
    Traversal::IterableBase::_USING_SHARED_IMPL_<String_BufferedArray_Rep_> sRep { DEBUG_NEW String_BufferedArray_Rep_ (reinterpret_cast<const wchar_t*> (d.first), reinterpret_cast<const wchar_t*> (d.second), (d.second - d.first) + (to - from)) };
    sRep->InsertAt (from, to, at);
    *this = String (sRep);
}

void    String::InsertAt (Character c, size_t at)
{
    // DEPRECATED
    DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
    DISABLE_COMPILER_MSC_WARNING_START(4996)
    InsertAt (&c, &c + 1, at);
    DISABLE_COMPILER_MSC_WARNING_END(4996)
    DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
}
void    String::InsertAt (const String& s, size_t at)
{
    // DEPRECATED
    DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
    DISABLE_COMPILER_MSC_WARNING_START(4996)
    /// @TODO - REDO / RETHINK THIS COMMENT - OBSOLETE
    // NB: I don't THINK we need be careful if s.fRep == this->fRep because when we first derefence this->fRep it will force a CLONE, so OUR fRep will be unique
    // And no need to worry about lifetime of 'p' because we don't allow changes to 's' from two different threads at a time, and the rep would rep if accessed from
    // another thread could only change that other envelopes copy
    _SafeRepAccessor rhsCopyAccessor (s);
    pair<const Character*, const Character*> d = rhsCopyAccessor._ConstGetRep ().GetData ();
    String  thisCopy =  *this;
    thisCopy.InsertAt (d.first, d.second, at);
    *this = thisCopy;
    DISABLE_COMPILER_MSC_WARNING_END(4996)
    DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
}
void    String::InsertAt (const wchar_t* from, const wchar_t* to, size_t at)
{
    // DEPRECATED
    DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
    DISABLE_COMPILER_MSC_WARNING_START(4996)
    InsertAt (reinterpret_cast<const Character*> (from), reinterpret_cast<const Character*> (to), at);
    DISABLE_COMPILER_MSC_WARNING_END(4996)
    DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
}

String    String::InsertAt_nu (const Character* from, const Character* to, size_t at) const
{
    Require (at >= 0);
    Require (at <= GetLength ());
    Require (from <= to);
    Require (from != nullptr or from == to);
    Require (to != nullptr or from == to);
    if (from == to) {
        return *this;
    }
    _SafeRepAccessor copyAccessor { *this };
    pair<const Character*, const Character*> d = copyAccessor._ConstGetRep ().GetData ();
    Traversal::IterableBase::_USING_SHARED_IMPL_<String_BufferedArray_Rep_> sRep { DEBUG_NEW String_BufferedArray_Rep_ (reinterpret_cast<const wchar_t*> (d.first), reinterpret_cast<const wchar_t*> (d.second), (d.second - d.first) + (to - from)) };
    sRep->InsertAt (from, to, at);
    return String (sRep);
}

String        String::RemoveAt (size_t from, size_t to) const
{
    Require (from <= to);
    Require (to <= GetLength ());
    _SafeRepAccessor accessor { *this };
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
    _SafeRepAccessor    accessor { *this };
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
    _SafeRepAccessor    accessor { *this };
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

vector<String>  String::FindEachString (const RegularExpression& regEx) const
{
    vector<String>  result;
    wstring tmp    { As<wstring> () };
    std::wsmatch res;
    regex_search (tmp, res, regEx.GetCompiled ());
    result.reserve (res.size ());
    for (auto i = res.begin (); i != res.end (); ++i) {
        result.push_back (String (*i));
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
    _SafeRepAccessor accessor { *this };
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
    _SafeRepAccessor    accessor { *this };
    if (accessor._ConstGetRep ()._GetLength () == 0) {
        return false;
    }
    return accessor._ConstGetRep ().GetAt (0).Compare (c, co) == 0;
}

bool    String::StartsWith (const String& subString, CompareOptions co) const
{
    _SafeRepAccessor    accessor { *this };
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
    _SafeRepAccessor    accessor { *this };
    const _IRep&    useRep = accessor._ConstGetRep ();
    size_t  thisStrLen = useRep._GetLength ();
    if (thisStrLen == 0) {
        return false;
    }
    return useRep.GetAt (thisStrLen - 1).Compare (c, co) == 0;
}

bool    String::EndsWith (const String& subString, CompareOptions co) const
{
    _SafeRepAccessor    accessor { *this };
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

bool    String::Match (const RegularExpression& regEx) const
{
    wstring tmp  { As<wstring> () };
    return regex_match (tmp.begin(), tmp.end(), regEx.GetCompiled ());
}

String  String::ReplaceAll (const RegularExpression& regEx, const String& with, CompareOptions co) const
{
#if     !qCompilerAndStdLib_regex_Buggy
    return String (regex_replace (As<wstring> (), regEx.GetCompiled (), with.As<wstring> ()));
#else
    AssertNotImplemented ();
    return String ();
#endif
}

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

String  String::SubString_ (const _SafeRepAccessor& thisAccessor, size_t thisLen, size_t from, size_t to)
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
    // This is mostly a win if it saves a malloc() (and the copy of the start...end char data)
    return String (_SharedPtrIRep (DEBUG_NEW String_Substring_::MyRep_ (thisAccessor, start, end)));
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
                StringBuilder   result
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
    // @todo - NOT ENVELOPE THREADSAFE
    _SafeRepAccessor    accessor { *this };
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
    // @todo - NOT ENVELOPE THREADSAFE
    RequireNotNull (shouldBeTrimmmed);
    _SafeRepAccessor    accessor { *this };
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
    StringBuilder       result;
    _SafeRepAccessor    accessor { *this };
    size_t              n   =   accessor._ConstGetRep ()._GetLength ();
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
    StringBuilder       result;
    _SafeRepAccessor    accessor { *this };
    size_t              n   =   accessor._ConstGetRep ()._GetLength ();
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
    String  tmp =   Trim ();
    if (tmp.length () <= maxLen) {
        return tmp;
    }
    size_t  useLen  =   maxLen;
    if (useLen > kELIPSIS_.length ()) {
        useLen -= kELIPSIS_.length ();
    }
    else {
        useLen = 0;
    }
    if (keepLeft) {
        return tmp.substr (0, useLen) + kELIPSIS_;
    }
    else {
        return kELIPSIS_ + tmp.substr (tmp.length () - useLen);
    }
}

template    <>
void    String::AsUTF8 (string* into) const
{
    RequireNotNull (into);
    _SafeRepAccessor    accessor { *this };
    pair<const Character*, const Character*> lhsD   =   accessor._ConstGetRep ().GetData ();
    WideStringToNarrow (reinterpret_cast<const wchar_t*> (lhsD.first), reinterpret_cast<const wchar_t*> (lhsD.second), kCodePage_UTF8, into);
}

void    String::AsSDKString (SDKString* into) const
{
    RequireNotNull (into);
    _SafeRepAccessor    accessor { *this };
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
    _SafeRepAccessor    accessor { *this };
    pair<const Character*, const Character*> lhsD   =   accessor._ConstGetRep ().GetData ();
    WideStringToNarrow (reinterpret_cast<const wchar_t*> (lhsD.first), reinterpret_cast<const wchar_t*> (lhsD.second), GetDefaultSDKCodePage (), into);
}

template    <>
void    String::AsASCII (string* into) const
{
    RequireNotNull (into);
    into->clear ();
    _SafeRepAccessor    accessor { *this };
    size_t  len =   accessor._ConstGetRep ().GetLength ();
    into->reserve (len);
    for (size_t i = 0; i < len; ++i) {
        Assert (accessor._ConstGetRep ().GetAt (i).IsAscii ());
        into->push_back (static_cast<char> (accessor._ConstGetRep ().GetAt (i).GetCharacterCode ()));
    }
}

const wchar_t*  String::c_str_ () const
{
    /*
     *  NOTE: This function is INTRINSICALLY un-threadsafe, so don't even bother to try with threadsafety.
     *  Access to this envelope MUST be externally synchonized or the returned bare pointer is doo-doo.
     */
    const   wchar_t*    result = _ConstGetRep ().c_str_peek ();
    if (result == nullptr) {
        pair<const Character*, const Character*> d = _ConstGetRep ().GetData ();
        _SharedPtrIRep tmp = mk_(reinterpret_cast<const wchar_t*> (d.first), reinterpret_cast<const wchar_t*> (d.second), d.second - d.first + 1);
        String* REALTHIS    =   const_cast<String*> (this);
        *REALTHIS = String (move (tmp));
        result = _ConstGetRep ().c_str_peek ();
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
    String::_SafeRepAccessor    rhsAccessor { rhs };
    pair<const Character*, const Character*> rhsD   =   rhsAccessor._ConstGetRep ().GetData ();
    size_t  lhsLen      =   ::wcslen (lhs);
    size_t  totalLen    =   lhsLen + (rhsD.second - rhsD.first);
    String::_USING_SHARED_IMPL_<String_BufferedArray_Rep_> sRep { DEBUG_NEW String_BufferedArray_Rep_ (reinterpret_cast<const wchar_t*> (lhs), reinterpret_cast<const wchar_t*> (lhs + lhsLen), totalLen) };
    sRep->InsertAt (rhsD.first, rhsD.second, lhsLen);
    return String (sRep);
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
