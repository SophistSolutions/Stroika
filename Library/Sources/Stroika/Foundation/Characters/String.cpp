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
                Assert (reinterpret_cast<const wchar_t*> (fSaved_._GetRep ().Peek ()) <= _fStart and _fStart <= _fEnd);
            }
            MyRep_ (const MyRep_& from)
                : inherited (from._fStart, from._fEnd)
                , fSaved_ (from.fSaved_)
            {
                Assert (reinterpret_cast<const wchar_t*> (fSaved_._GetRep ().Peek ()) <= _fStart and _fStart <= _fEnd);
            }
            virtual  _IterableSharedPtrIRep   Clone (IteratorOwnerID forIterableEnvelope) const override
            {
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
    inline  regex_constants::syntax_option_type mkOption_ (RegularExpression::SyntaxType st)
    {
        regex_constants::syntax_option_type f   =   (st == RegularExpression::SyntaxType::eECMAScript ? regex_constants::ECMAScript : regex_constants::basic);
        return f;
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
    // This is a kludge. We are CLONING the Iterator rep (as a way to get a shared_ptr) - but really we should
    // use 'shared_from_this'.
    //
    //
    // Because of 'Design Choice - Iterable<T> / Iterator<T> behavior' in String class docs - we
    // ignore suggested IteratorOwnerID - which explains the arg to Clone () below
    return Iterator<Character> (Iterator<Character>::SharedIRepPtr (new MyIterRep_ (dynamic_pointer_cast<String::_SharedPtrIRep::element_type> (Clone (nullptr)))));
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

#if 0




void    String::_IRep::InsertAt (const Character* srcStart, const Character* srcEnd, size_t index)
{
    Execution::DoThrow (UnsupportedFeatureException ());
}
#endif

#if 0
void    String::_IRep::SetAt (Character item, size_t index)
{
    Execution::DoThrow (UnsupportedFeatureException ());
}
#endif

const wchar_t*  String::_IRep::c_str_peek () const  noexcept
{
    return nullptr;
}

const wchar_t*      String::_IRep::c_str_change ()
{
    Execution::DoThrow (UnsupportedFeatureException ());
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
        InsertAt (*i, (i - cString));
    }
#if     qDebug
    _GetRep (); // just make sure non-null and right type
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
    return UTF8StringToWide (from);
}

String  String::FromUTF8 (const char* from, const char* to)
{
    return UTF8StringToWide (string (from, to));
}

String  String::FromUTF8 (const std::string& from)
{
    return UTF8StringToWide (from);
}

String  String::FromSDKString (const SDKChar* from)
{
    return SDKString2Wide (from);
}

String  String::FromSDKString (const SDKChar* from, const SDKChar* to)
{
    return SDKString2Wide (SDKString (from, to));
}

String  String::FromSDKString (const basic_string<SDKChar>& from)
{
    return SDKString2Wide (from);
}

String  String::FromNarrowSDKString (const char* from)
{
    RequireNotNull (from);
    return NarrowSDKStringToWide (from);
}

String  String::FromNarrowSDKString (const string& from)
{
    return NarrowSDKStringToWide (from);
}

String  String::FromAscii (const char* from)
{
#if     qDebug
    for (auto i = from; i != from; ++i) {
        Require (isascii (*i));
    }
#endif
    return ASCIIStringToWide (from);
}

String  String::FromAscii (const string& from)
{
#if     qDebug
    for (auto i = from.begin (); i != from.end (); ++i) {
        Require (isascii (*i));
    }
#endif
    return ASCIIStringToWide (from);
}

String::_SharedPtrIRep  String::mkEmpty_ ()
{
    static  _SharedPtrIRep s_   =   mk_ (nullptr, nullptr);
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
    pair<const Character*, const Character*> lhsD   =   thisAccessor._GetRep ().GetData ();
    size_t  lhsLen  =   lhsD.second - lhsD.first;
    if (lhsLen == 0) {
        return rhs;
    }
    _SafeRepAccessor    rhsAccessor (rhs);
    pair<const Character*, const Character*> rhsD   =   rhsAccessor._GetRep ().GetData ();
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
    pair<const Character*, const Character*> lhsD   =   thisAccessor._GetRep ().GetData ();
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
    Require (at >= 0);
    Require (at <= GetLength ());
    Require (from <= to);
    Require (from != nullptr or from == to);
    Require (to != nullptr or from == to);
    if (from == to) {
        return;
    }
#if 1
    _SafeRepAccessor copyAccessor { *this };
    pair<const Character*, const Character*> d = copyAccessor._GetRep ().GetData ();
    Traversal::IterableBase::_USING_SHARED_IMPL_<String_BufferedArray_Rep_> sRep { DEBUG_NEW String_BufferedArray_Rep_ (reinterpret_cast<const wchar_t*> (d.first), reinterpret_cast<const wchar_t*> (d.second), (d.second - d.first) + (to - from)) };
    sRep->InsertAt (from, to, at);
    *this = String (sRep);
#else
    try {
        _GetRep ().InsertAt (from, to, at);
    }
    catch (const _IRep::UnsupportedFeatureException&) {
        _SafeRepAccessor copyAccessor { *this };
        pair<const Character*, const Character*> d = copyAccessor._GetRep ().GetData ();
        _SharedPtrIRep  tmp = mk_ (reinterpret_cast<const wchar_t*> (d.first), reinterpret_cast<const wchar_t*> (d.second), (d.second - d.first) + (to - from));
        tmp->InsertAt (from, to, at);
        *this = tmp;
    }
#endif
}

void    String::InsertAt (Character c, size_t at)
{
    InsertAt (&c, &c + 1, at);
}
void    String::InsertAt (const String& s, size_t at)
{
    /// @TODO - REDO / RETHINK THIS COMMENT - OBSOLETE
    // NB: I don't THINK we need be careful if s.fRep == this->fRep because when we first derefence this->fRep it will force a CLONE, so OUR fRep will be unique
    // And no need to worry about lifetime of 'p' because we don't allow changes to 's' from two different threads at a time, and the rep would rep if accessed from
    // another thread could only change that other envelopes copy

    _SafeRepAccessor rhsCopyAccessor (s);
    pair<const Character*, const Character*> d = rhsCopyAccessor._GetRep ().GetData ();
    String  thisCopy =  *this;
    thisCopy.InsertAt (d.first, d.second, at);
    *this = thisCopy;
}
void    String::InsertAt (const wchar_t* from, const wchar_t* to, size_t at)
{
    InsertAt (reinterpret_cast<const Character*> (from), reinterpret_cast<const Character*> (to), at);
}

void    String::Append (Character c)
{
    String  tmp =   *this;
    tmp.InsertAt (c, tmp.GetLength ());
    *this = tmp;
}
void    String::Append (const String& s)
{
    String  tmp =   *this;
    tmp.InsertAt (s, tmp.GetLength ());
    *this = tmp;
}
void    String::Append (const wchar_t* from, const wchar_t* to)
{
    String  tmp =   *this;
    tmp.InsertAt (from, to, tmp.GetLength ());
    *this = tmp;
}
void    String::Append (const Character* from, const Character* to)
{
    // @todo - FIX - NOT ENVELOPE THREADSAFE
    InsertAt (from, to, GetLength ());
}

String        String::RemoveAt (size_t from, size_t to) const
{
    Require (from <= to);
    Require (to <= GetLength ());
    _SafeRepAccessor accessor { *this };
    size_t length = accessor._GetRep ()._GetLength ();
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
        pair<const Character*, const Character*> d = accessor._GetRep ().GetData ();
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
    Require (startAt <= accessor._GetRep ()._GetLength ());
    size_t length = accessor._GetRep ()._GetLength ();
    switch (co) {
        case CompareOptions::eCaseInsensitive: {
                for (size_t i = startAt; i < length; i++) {
                    if (accessor._GetRep ().GetAt (i).ToLowerCase () == c.ToLowerCase ()) {
                        return i;
                    }
                }
            }
            break;
        case CompareOptions::eWithCase: {
                for (size_t i = startAt; i < length; i++) {
                    if (accessor._GetRep ().GetAt (i) == c) {
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
    Require (startAt <= accessor._GetRep ()._GetLength ());

    size_t  subStrLen   =   subString.GetLength ();
    if (subStrLen == 0) {
        return (accessor._GetRep ()._GetLength () == 0) ? kBadIndex : 0;
    }
    if (accessor._GetRep ()._GetLength () < subStrLen) {
        return kBadIndex;   // important test cuz size_t is unsigned
    }

    size_t  limit       =   accessor._GetRep ()._GetLength () - subStrLen;
    switch (co) {
        case CompareOptions::eCaseInsensitive: {
                for (size_t i = startAt; i <= limit; i++) {
                    for (size_t j = 0; j < subStrLen; j++) {
                        if (accessor._GetRep ().GetAt (i + j).ToLowerCase () != subString[j].ToLowerCase ()) {
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
                        if (accessor._GetRep ().GetAt (i + j) != subString[j]) {
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
    const _IRep&    useRep = accessor._GetRep ();
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
    if (accessor._GetRep ()._GetLength () == 0) {
        return false;
    }
    return accessor._GetRep ().GetAt (0).Compare (c, co) == 0;
}

bool    String::StartsWith (const String& subString, CompareOptions co) const
{
    _SafeRepAccessor    accessor { *this };
    size_t  subStrLen = subString.GetLength ();
    if (subStrLen >  accessor._GetRep ()._GetLength ()) {
        return false;
    }
#if     qDebug
    bool    referenceResult = (SubString (0, subString.GetLength ()).Compare (subString, co) == 0); // this check isnt threadsafe - redo
#endif
    const Character*    subStrStart = reinterpret_cast<const Character*> (subString.c_str ());
    pair<const Character*, const Character*> thisData = accessor._GetRep ().GetData ();
    bool    result = (Character::Compare (thisData.first, thisData.first + subStrLen, subStrStart, subStrStart + subStrLen, co) == 0);
    Assert (result == referenceResult);
    return result;
}

bool    String::EndsWith (const Character& c, CompareOptions co) const
{
    _SafeRepAccessor    accessor { *this };
    const _IRep&    useRep = accessor._GetRep ();
    size_t  thisStrLen = useRep._GetLength ();
    if (thisStrLen == 0) {
        return false;
    }
    return useRep.GetAt (thisStrLen - 1).Compare (c, co) == 0;
}

bool    String::EndsWith (const String& subString, CompareOptions co) const
{
    _SafeRepAccessor    accessor { *this };
    size_t      thisStrLen = accessor._GetRep ()._GetLength ();
    size_t      subStrLen = subString.GetLength ();
    if (subStrLen >  thisStrLen) {
        return false;
    }
#if     qDebug
    bool    referenceResult = (SubString (thisStrLen - subStrLen, thisStrLen).Compare (subString, co) == 0);    // this check isnt threadsafe - redo
#endif
    const Character*    subStrStart = reinterpret_cast<const Character*> (subString.c_str ());
    pair<const Character*, const Character*> thisData = accessor._GetRep ().GetData ();
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
    const wchar_t*  start   =   reinterpret_cast<const wchar_t*> (thisAccessor._GetRep ().Peek ()) + from;
    size_t          len     =   to - from;
    const wchar_t*  end     =   start + len;
    Assert (start <= end);
    // Slightly faster to re-use pre-built empty string
    if (start == end) {
        return mkEmpty_ ();
    }
    if (len == thisLen) {
        Assert (from == 0);
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
    // @todo inefficient - use StringBuffer
    String  result;
    for (unsigned int i = 0; i < count; ++i) {
        result += *this;
    }
    return result;
}

String  String::LTrim (bool (*shouldBeTrimmmed) (Character)) const
{
    RequireNotNull (shouldBeTrimmmed);
    // @todo - NOT ENVELOPE THREADSAFE

    _SafeRepAccessor    accessor { *this };
    //TODO: FIX HORRIBLE PERFORMANCE!!!
    // Could be much more efficient if pushed into REP - so we avoid each character virtual call...
    size_t length = accessor._GetRep ()._GetLength ();
    for (size_t i = 0; i < length; ++i) {
        if (not (*shouldBeTrimmmed) (accessor._GetRep ().GetAt (i))) {

            // @todo - NOT THREADAFE - BUGGY - MUST USE ACCESSOR TO RECONSTRUCT WHAT WE STRING WE RETURN!!! - EVEN FOR SUBSTR

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
    // @todo - NOT ENVELOPE THREADSAFE
    RequireNotNull (shouldBeTrimmmed);
    //TODO: FIX HORRIBLE PERFORMANCE!!!
    // Could be much more efficient if pushed into REP - so we avoid each character virtual call...
    _SafeRepAccessor    accessor { *this };

    ptrdiff_t length = accessor._GetRep ()._GetLength ();
    ptrdiff_t endOfFirstTrim = length;
    for (; endOfFirstTrim != 0; --endOfFirstTrim) {
        if ((*shouldBeTrimmmed) (accessor._GetRep ().GetAt (endOfFirstTrim - 1))) {
            // keep going backwards
        }
        else {
            break;
        }
    }

    // @todo - NOT THREADAFE - BUGGY - MUST USE ACCESSOR TO RECONSTRUCT WHAT WE STRING WE RETURN!!! - EVEN FOR SUBSTR

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
    size_t              n   =   accessor._GetRep ()._GetLength ();
    bool    anyChange   =   false;
    for (size_t i = 0; i < n; ++i) {
        Character   c   =   accessor._GetRep ().GetAt (i);
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
    size_t              n   =   accessor._GetRep ()._GetLength ();
    bool    anyChange   =   false;
    for (size_t i = 0; i < n; ++i) {
        Character   c   =   accessor._GetRep ().GetAt (i);
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
    *into = WideStringToUTF8 (As<wstring> ());  //tmphack impl (but shoudl work)
}

SDKString String::AsSDKString () const
{
    SDKString result;
    AsSDKString (&result);
    return result;
}

void    String::AsSDKString (SDKString* into) const
{
    RequireNotNull (into);
    *into = Wide2SDKString (As<wstring> ());  // poor inefficient implementation
}

string  String::AsNarrowSDKString () const
{
    string result;
    AsNarrowSDKString (&result);
    return result;
}

void    String::AsNarrowSDKString (string* into) const
{
    RequireNotNull (into);
    *into = Characters::WideStringToNarrowSDKString (As<wstring> ());  // poor inefficient implementation
}

template    <>
void    String::AsASCII (string* into) const
{
    RequireNotNull (into);
    into->clear ();
    const String  threadSafeCopy  { *this };
    size_t  len =   threadSafeCopy.GetLength ();
    into->reserve (len);
    for (size_t i = 0; i < len; ++i) {
        into->push_back (threadSafeCopy[i].GetAsciiCode ());
    }
}

const wchar_t*  String::c_str_ () const
{
    const   wchar_t*    result = _GetRep ().c_str_peek ();
    if (result == nullptr) {
        // Then we must force it to be NUL-terminated
        //
        // We want to DECLARE c_str() as const, becuase it doesn't conceptually change the underlying data, but to get the
        // fRep cloning stuff to work right, we must access it through a non-cost pointer
        String* REALTHIS    =   const_cast<String*> (this);
        try {
            return REALTHIS->_GetRep ().c_str_change ();
        }
        catch (const _IRep::UnsupportedFeatureException&) {
            _SafeRepAccessor copyAccessor (*this);
            pair<const Character*, const Character*> d = copyAccessor._GetRep ().GetData ();
            String  tmp = mk_ (reinterpret_cast<const wchar_t*> (d.first), reinterpret_cast<const wchar_t*> (d.second), d.second - d.first + 1);
            // @todo - CONSIDER CAREFULLY IF THIS IS THREADAFE??? DOCUMENT WHY IF IT IS!!! --LGP 2013-12-17
            *REALTHIS = tmp;
            return REALTHIS->_GetRep ().c_str_change ();
        }
    }
    else {
        return result;
    }
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
    pair<const Character*, const Character*> rhsD   =   rhsAccessor._GetRep ().GetData ();
    size_t  lhsLen      =   ::wcslen (lhs);
    size_t  totalLen    =   lhsLen + (rhsD.second - rhsD.first);
//    String::_SharedPtrIRep  sRep { new String_BufferedArray_Rep_ (reinterpret_cast<const wchar_t*> (lhs), reinterpret_cast<const wchar_t*> (lhs + lhsLen), totalLen) };
    Traversal::IterableBase::_USING_SHARED_IMPL_<String_BufferedArray_Rep_> sRep { DEBUG_NEW String_BufferedArray_Rep_ (reinterpret_cast<const wchar_t*> (lhs), reinterpret_cast<const wchar_t*> (lhs + lhsLen), totalLen) };
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
