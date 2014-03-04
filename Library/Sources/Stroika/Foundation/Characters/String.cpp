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
#include    "Concrete/String_BufferedArray.h"
#include    "Concrete/Private/String_BufferedStringRep.h"
#include    "SDKString.h"
#include    "String_Constant.h"

#include    "String.h"





using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;


using   Traversal::IteratorOwnerID;







namespace   {
    class   String_BufferedArray_Rep_ : public Concrete::Private::BufferedStringRep::_Rep {
    private:
        using   inherited   =   Concrete::Private::BufferedStringRep::_Rep;
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
    // THIS SHOULD BE SIMPLER NOW (cuz of immutable string technique! - but what we have here is probably sucky/broken
    // or at least must be reviewed
    //      --LGP 2014-03-04
#define qString_SubStringClassWorks 1

#if     qString_SubStringClassWorks
    struct String_Substring_ : public Concrete::Private::ReadOnlyRep {
        class   MyRep_ : public _Rep {
            using inherited = _Rep;
        public:
            MyRep_ (const String::_SafeRepAccessor& savedSP, const wchar_t* start, const wchar_t* end)
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
            String::_SafeRepAccessor  fSaved_;
        public:
            DECLARE_USE_BLOCK_ALLOCATION(MyRep_);
        };
    };
#endif
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
 ************************************* String ***********************************
 ********************************************************************************
 */
static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");

namespace   {
    struct  MyEmptyString_ : String {
        static  _SharedPtrIRep mkEmptyStrRep_ ()
        {
            static  bool                            sInited_    =   false;
            static  _SharedPtrIRep s_;
            if (not sInited_) {
                sInited_ = true;
                s_ = _SharedPtrIRep (DEBUG_NEW String_BufferedArray_Rep_ (nullptr, 0));
            }
            return s_;
        }
    };
}

String::String ()
    : inherited (MyEmptyString_::mkEmptyStrRep_ ())
{
#if     qDebug
    _GetRep (); // just make sure non-null and right type
#endif
}

String::String (const char16_t* cString)
    : inherited (MyEmptyString_::mkEmptyStrRep_ ())
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

String::String (const wchar_t* cString)
    : inherited (cString[0] == '\0' ? MyEmptyString_::mkEmptyStrRep_ () : _SharedPtrIRep (DEBUG_NEW String_BufferedArray_Rep_ (cString, cString + wcslen (cString))))
{
    RequireNotNull (cString);
#if     qDebug
    _GetRep (); // just make sure non-null and right type
#endif
}

String::String (const wchar_t* from, const wchar_t* to)
    : inherited ((from == to) ? MyEmptyString_::mkEmptyStrRep_ () : _SharedPtrIRep (DEBUG_NEW String_BufferedArray_Rep_ (from, to)))
{
    Require (from <= to);
    Require (from != nullptr or from == to);
#if     qDebug
    _GetRep (); // just make sure non-null and right type
#endif
}

String::String (const Character* from, const Character* to)
    : inherited ((from == to) ? MyEmptyString_::mkEmptyStrRep_ () : _SharedPtrIRep (DEBUG_NEW String_BufferedArray_Rep_ (reinterpret_cast<const wchar_t*> (from), reinterpret_cast<const wchar_t*> (to))))
{
    static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
    Require (from <= to);
    Require (from != nullptr or from == to);
#if     qDebug
    _GetRep (); // just make sure non-null and right type
#endif
}

String::String (const std::wstring& r)
    : inherited (r.empty () ? MyEmptyString_::mkEmptyStrRep_ () : _SharedPtrIRep (DEBUG_NEW String_BufferedArray_Rep_ (r.data (), r.data () + r.length ())))
{
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

String::String (const _SharedPtrIRep& rep)
    : inherited (rep)
{
#if     qDebug
    _GetRep (); // just make sure non-null and right type
#endif
}

String::String (_SharedPtrIRep&& rep)
    : inherited (std::move (rep))
{
#if     qDebug
    _GetRep (); // just make sure non-null and right type
#endif
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

String  String::operator+ (const String& rhs) const
{
    if (rhs.empty ()) {
        return *this;
    }
    Concrete::String_BufferedArray    tmp = Concrete::String_BufferedArray (*this, size () + rhs.size ());
    tmp += rhs;
    return tmp;
}

void    String::SetLength (size_t newLength)
{
    try {
        if (newLength == 0) {
            _GetRep ().RemoveAll ();
        }
        else {
            _GetRep ().SetLength (newLength);
        }
    }
    catch (const _IRep::UnsupportedFeatureException&) {
        Concrete::String_BufferedArray    tmp =   Concrete::String_BufferedArray (*this, max (GetLength (), newLength));
        // @todo - CONSIDER CAREFULLY IF THIS IS THREADAFE??? DOCUMENT WHY IF IT IS!!! --LGP 2013-12-17
        *this = tmp;
        if (newLength == 0) {
            _GetRep ().RemoveAll ();
        }
        else {
            _GetRep ().SetLength (newLength);
        }
    }
}

void    String::SetCharAt (Character c, size_t i)
{
    Require (i >= 0);
    Require (i < GetLength ());
    try {
        _GetRep ().SetAt (c, i);
    }
    catch (const _IRep::UnsupportedFeatureException&) {
        Concrete::String_BufferedArray    tmp =   Concrete::String_BufferedArray (*this);
        // @todo - CONSIDER CAREFULLY IF THIS IS THREADAFE??? DOCUMENT WHY IF IT IS!!! --LGP 2013-12-17
        *this = tmp;
        _GetRep ().SetAt (c, i);
    }
}

void    String::InsertAt (const Character* from, const Character* to, size_t at)
{
    Require (at >= 0);
    Require (at <= (GetLength ()));
    Require (from <= to);
    Require (from != nullptr or from == to);
    Require (to != nullptr or from == to);
    if (from == to) {
        return;
    }
    try {
        _GetRep ().InsertAt (from, to, at);
    }
    catch (const _IRep::UnsupportedFeatureException&) {
        Concrete::String_BufferedArray    tmp =   Concrete::String_BufferedArray (*this, GetLength () + (to - from));
        // @todo - CONSIDER CAREFULLY IF THIS IS THREADAFE??? DOCUMENT WHY IF IT IS!!! --LGP 2013-12-17
        *this = tmp;
        _GetRep ().InsertAt (from, to, at);
    }
}

void    String::RemoveAt (size_t from, size_t to)
{
    Require (from <= to);
    Require (to <= GetLength ());
    try {
        _GetRep ().RemoveAt (from, to);
    }
    catch (const _IRep::UnsupportedFeatureException&) {
        Concrete::String_BufferedArray    tmp =   Concrete::String_BufferedArray (*this);
        // @todo - CONSIDER CAREFULLY IF THIS IS THREADAFE??? DOCUMENT WHY IF IT IS!!! --LGP 2013-12-17
        *this = tmp;
        _GetRep ().RemoveAt (from, to);
    }
}

String        String::RemoveAt_nu (size_t from, size_t to) const
{
    Require (from <= to);
    Require (to <= GetLength ());
    try {
        String  tmp =   *this;
        tmp._GetRep ().RemoveAt (from, to);
        return tmp;
    }
    catch (const _IRep::UnsupportedFeatureException&) {
        Concrete::String_BufferedArray    tmp =   Concrete::String_BufferedArray (*this);
        tmp._GetRep ().RemoveAt (from, to);
        return tmp;
    }
}

void    String::Remove (Character c)
{
    DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
    DISABLE_COMPILER_MSC_WARNING_START(4996)
    // @TODO - NOT THREADSAFE
    size_t index = Find (c, CompareOptions::eWithCase);
    if (index != kBadIndex) {
        RemoveAt (index);
    }
    DISABLE_COMPILER_MSC_WARNING_END(4996)
    DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
}

size_t  String::Find (Character c, size_t startAt, CompareOptions co) const
{
    //@todo could improve performance with strength reduction
    _SafeRepAccessor    accessor (*this);
    Require (startAt <= accessor._GetRep ().GetLength ());
    size_t length = accessor._GetRep ().GetLength ();
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
    _SafeRepAccessor    accessor (*this);
    Require (startAt <= accessor._GetRep ().GetLength ());

    size_t  subStrLen   =   subString.GetLength ();
    if (subStrLen == 0) {
        return (accessor._GetRep ().GetLength () == 0) ? kBadIndex : 0;
    }
    if (accessor._GetRep ().GetLength () < subStrLen) {
        return kBadIndex;   // important test cuz size_t is unsigned
    }

    size_t  limit       =   accessor._GetRep ().GetLength () - subStrLen;
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
    const String  threadSafeCopy  =   *this;
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
    const String  threadSafeCopy  =   *this;
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
    wstring         tmp = As<wstring> ();
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
    wstring tmp     =   As<wstring> ();
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
    _SafeRepAccessor accessor (*this);
    const _IRep&    useRep = accessor._GetRep ();
    size_t length = useRep.GetLength ();
    for (size_t i = length; i > 0; --i) {
        if (useRep.GetAt (i - 1) == c) {
            return i - 1;
        }
    }
    return kBadIndex;
}

size_t  String::RFind (const String& subString) const
{
    const String  threadSafeCopy  =   *this;
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
    _SafeRepAccessor accessor (*this);
    if (accessor._GetRep ().GetLength () == 0) {
        return false;
    }
    return accessor._GetRep ().GetAt (0).Compare (c, co) == 0;
}

bool    String::StartsWith (const String& subString, CompareOptions co) const
{
    _SafeRepAccessor accessor (*this);
    size_t  subStrLen = subString.GetLength ();
    if (subStrLen >  accessor._GetRep ().GetLength ()) {
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
    _SafeRepAccessor accessor (*this);
    const _IRep&    useRep = accessor._GetRep ();
    size_t  thisStrLen = useRep.GetLength ();
    if (thisStrLen == 0) {
        return false;
    }
    return useRep.GetAt (thisStrLen - 1).Compare (c, co) == 0;
}

bool    String::EndsWith (const String& subString, CompareOptions co) const
{
    _SafeRepAccessor accessor (*this);
    size_t  thisStrLen = accessor._GetRep ().GetLength ();
    size_t  subStrLen = subString.GetLength ();
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
    wstring tmp =   As<wstring> ();
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
    String  result = *this;
    size_t  i   =   0;
    while ((i = result.Find (string2SearchFor, i, co)) != String::npos) {
        result = result.SubString (0, i) + with + result.SubString (i + string2SearchFor.length ());
        i += with.length ();
    }
    return result;
}

String  String::SubString (size_t from, size_t to) const
{
    _SafeRepAccessor accessor (*this);
    size_t  myLength    =   accessor._GetRep ().GetLength ();

    Require ((from <= to) or (to == kBadIndex));
    Require ((to <= myLength) or (to == kBadIndex));

    size_t  length  =   (to == kBadIndex) ? (myLength - from) : (to - from);
    if (length == 0) {
        return String ();
    }
    if ((from == 0) and (length == myLength)) {
        return *this;       // just bump reference count
    }
#if     qString_SubStringClassWorks
    const wchar_t*  start =   reinterpret_cast<const wchar_t*> (accessor._GetRep ().Peek ()) + from;
    return String
           (
               String::_SharedPtrIRep (
                   DEBUG_NEW String_Substring_::MyRep_ (
                       accessor,
                       start,
                       start + length
                   )
               )
           );
#else
    return (String (accessor._GetRep ().Peek () + from, accessor._GetRep ().Peek () + from + length));
#endif
}

String      String::CircularSubString (ptrdiff_t from, ptrdiff_t to) const
{
    const String  threadSafeCopy  =   *this;
    size_t  f = from < 0 ? (threadSafeCopy.GetLength () + from) : from;
    size_t  t = to < 0 ? (threadSafeCopy.GetLength () + to) : to;
    Require (f != kBadIndex);
    Require (t != kBadIndex);
    Require (f <= t);
    Require (t <= threadSafeCopy.GetLength ());
    return threadSafeCopy.SubString (f, t);
}

String  String::Repeat (unsigned int count) const
{
    // @todo ineffcient - use StringBuffer
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

    _SafeRepAccessor accessor (*this);
    //TODO: FIX HORRIBLE PERFORMANCE!!!
    // Could be much more efficient if pushed into REP - so we avoid each character virtual call...
    size_t length = accessor._GetRep ().GetLength ();
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
    _SafeRepAccessor accessor (*this);

    ptrdiff_t length = accessor._GetRep ().GetLength ();
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
    String  result  =   *this;
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
    //TODO: FIX HORRIBLE PERFORMANCE!!! (e.g. access wchar_t* const and do there?
    // use StringBuffer
    //

    // Copy the string first (cheap cuz just refcnt) - but be sure to check if any real change before calling SetAt cuz SetAt will do the actual copy-on-write
    String  result  =   *this;
    size_t  n   =   result.GetLength ();
    for (size_t i = 0; i < n; ++i) {
        Character   c   =   result[i];
        if (c.IsUpperCase ()) {
            result.SetCharAt (c.ToLowerCase (), i);
        }
    }
    return result;
}

String  String::ToUpperCase () const
{
    //TODO: FIX HORRIBLE PERFORMANCE!!! (e.g. access wchar_t* const and do there?
    // Copy the string first (cheap cuz just refcnt) - but be sure to check if any real change before calling SetAt cuz SetAt will do the actual copy-on-write
    String  result  =   *this;
    size_t  n   =   result.GetLength ();
    for (size_t i = 0; i < n; ++i) {
        Character   c   =   result[i];
        if (c.IsLowerCase ()) {
            result.SetCharAt (c.ToUpperCase (), i);
        }
    }
    return result;
}

bool String::IsWhitespace () const
{
    // It is all whitespace if the first non-whatspace character is 'EOF'
    return not FindFirstThat ([] (Character c) -> bool { return not c.IsWhitespace (); });
}

String  String::LimitLength (size_t maxLen, bool keepLeft) const
{
#if 1
    static  const String kELIPSIS_  = String_Constant (L"\u2026");
#else
    static  const String kELIPSIS_  = String_Constant (L"...");
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
    const String  threadSafeCopy  =   *this;
    size_t  len =   threadSafeCopy.GetLength ();
    into->reserve (len);
    for (size_t i = 0; i < len; ++i) {
        into->push_back (threadSafeCopy[i].GetAsciiCode ());
    }
}

const wchar_t*  String::c_str () const
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
            Concrete::String_BufferedArray    tmp =   Concrete::String_BufferedArray (*this, GetLength () + 1);
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
    DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
    DISABLE_COMPILER_MSC_WARNING_START(4996)
    // @todo - NOT ENVELOPE THREADSAFE
    // TODO: Double check STL definition - but I think they allow for count to be 'too much' - and silently trim to end...
    size_t  max2Erase    =   static_cast<size_t> (max (static_cast<ptrdiff_t> (0), static_cast<ptrdiff_t> (GetLength ()) - static_cast<ptrdiff_t> (from)));
    if (count == kBadIndex) {
        RemoveAt (from, GetLength ());
    }
    else {
        RemoveAt (from,  from + min (count, max2Erase));
    }
    DISABLE_COMPILER_MSC_WARNING_END(4996)
    DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
}

String      String::substr (size_t from, size_t count) const
{
    // TODO: Double check STL definition - but I think they allow for count to be 'too much' - and silently trim to end...
    if (count == kBadIndex) {
        return SubString (from);
    }
    else {
        const String  threadSafeCopy  =   *this;
        size_t  end =   min (from + count, threadSafeCopy.GetLength ());   // really should worry more about overflow (say if count is kBadIndex-1)
        Assert (from <= end);
        return threadSafeCopy.SubString (from, end);
    }
}







#if     qString_SubStringClassWorks && 0
/*
 ********************************************************************************
 ************************** String_Substring_::MyRep_ ***************************
 ********************************************************************************
 */
String_Substring_::MyRep_::MyRep_ (const _SharedRepByValuePtr& baseString, size_t from, size_t length)
    : fBase (baseString)
    , fFrom (from)
    , fLength (length)
{
    Require (from >= 0);
    Require (length >= 0);
    Require ((from + length) <= fBase->GetLength ());
}

shared_ptr<String::_IRep>   String_Substring_::MyRep_::Clone () const
{
    return (new String_CharArray::MyRep_ (Peek (), GetLength ()));
}

size_t  String_Substring_::MyRep_::GetLength () const
{
    return (fLength);
}

bool    String_Substring_::MyRep_::Contains (Character item) const
{
    for (size_t i = 0; i < fLength; i++) {
        if (GetAt (i) == item) {
            return (true);
        }
    }
    return (false);
}

void    String_Substring_::MyRep_::RemoveAll ()
{
    fBase->RemoveAll ();
    fFrom = 0;
    fLength = 0;
}

Character   String_Substring_::MyRep_::GetAt (size_t index) const
{
    Require (index < GetLength ());
    Assert ((fFrom + index) < fBase->GetLength ());
    return (fBase->GetAt (fFrom + index));
}

void    String_Substring_::MyRep_::SetAt (Character item, size_t index)
{
    Require (index < GetLength ());
    Assert ((fFrom + index) < fBase->GetLength ());
    fBase->SetAt (item, (fFrom + index));
}

void    String_Substring_::MyRep_::InsertAt (const Character* srcStart, const Character* srcEnd, size_t index)
{
    Require (index <= GetLength ());
    Assert ((fFrom + index) <= fBase->GetLength ());

    size_t  lenBeforeInsert =   GetLength();


    fBase->InsertAt (srcStart, srcEnd, (fFrom + index));
    fLength++;
}

void    String_Substring_::MyRep_::RemoveAt (size_t index, size_t amountToRemove)
{
    Require (index < GetLength ());
    Require ((index + amountToRemove) <= GetLength ());

    Assert ((fFrom + index + amountToRemove) < fBase->GetLength ());
    fBase->RemoveAt (index, amountToRemove);
    fLength -= amountToRemove;
}

void    String_Substring_::MyRep_::SetLength (size_t newLength)
{
    size_t delta = newLength - fLength;
    if (delta != 0) {
        fBase->SetLength (fBase->GetLength () + delta);
        fLength += delta;
    }
}

const Character*    String_Substring_::MyRep_::Peek () const
{
    Assert (fFrom <= fBase->GetLength ());

    const Character* buffer = fBase->Peek ();
    return (&buffer[fFrom]);
}
#endif








#if 0
/*
 ********************************************************************************
 ********************************** operator+ ***********************************
 ********************************************************************************
 */
String  Stroika::Foundation::Characters::operator+ (const String& lhs, const String& rhs)
{
    Concrete::String_BufferedArray    tmp =   Concrete::String_BufferedArray (lhs, lhs.size () + rhs.size ());
    tmp.InsertAt (rhs, tmp.GetLength ());
    return tmp;
}
#endif






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
