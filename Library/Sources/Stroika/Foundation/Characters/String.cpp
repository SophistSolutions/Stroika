/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
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
#include    "SDKString.h"

#include    "Concrete/Private/String_BufferedStringRep.h"

#include    "String.h"





using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;










namespace   {
    class   String_BufferedArray_Rep_ : public Concrete::Private::BufferedStringRep::_Rep {
    private:
        typedef Concrete::Private::BufferedStringRep::_Rep  inherited;
    public:
        String_BufferedArray_Rep_ (const wchar_t* start, const wchar_t* end)
            : inherited (start, end) {
        }
        String_BufferedArray_Rep_ (const wchar_t* start, const wchar_t* end, size_t reserve)
            : inherited (start, end, reserve) {
        }
        virtual shared_ptr<_IRep>   Clone () const override {
            return shared_ptr<_IRep> (DEBUG_NEW String_BufferedArray_Rep_ (_fStart, _fEnd));
        }
    public:
        DECLARE_USE_BLOCK_ALLOCATION(String_BufferedArray_Rep_);
    };
}
















namespace   {
    // For now - disable this class cuz its broken - run regression tests to see.
    // I'm not clear on how this is supposed to work. While read-only, it works FINE. But if anyone ever makes ANY modifications (removall, etc),
    // we REALLY want to switch to a different REP!!! Like o flength goes zero - we want to return the global kEmptyStringRep (maybe not).
    // But we more likely want to swithc the the string buffered rep or something. Maybe have virtual methods like InsertAt<>(in rep) return shared_ptr<Rep> so it can be changed?
    // Or take ref-param of that guy so it can be changed???
#define qString_SubStringClassWorks 0

#if     qString_SubStringClassWorks
    struct String_Substring_ : public String {
        class   MyRep_ : public String::_IRep {
        public:
            MyRep_ (const _SharedRepByValuePtr& baseString, size_t from, size_t length);

            virtual     _IRep*   Clone () const override;

            virtual     size_t  GetLength () const override;
            virtual     void    RemoveAll () override;

            virtual     Character   GetAt (size_t index) const override;
            virtual     void        SetAt (Character item, size_t index) override;
            virtual     void        InsertAt (const Character* srcStart, const Character* srcEnd, size_t index) override;
            virtual     void        RemoveAt (size_t index, size_t amountToRemove) override;

            virtual     void    SetLength (size_t newLength) override;

            virtual     const Character*    Peek () const override;
            virtual int Compare (const _IRep& rhs, CompareOptions co) const override;

        private:
            _SharedRepByValuePtr fBase;

            size_t  fFrom;
            size_t  fLength;
        };
    };
#endif
}











namespace   {
    regex_constants::syntax_option_type mkOption_ (RegularExpression::SyntaxType st)
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
        static  shared_ptr<String::_IRep>   Clone_ (const _IRep& rep) {
            return (rep.Clone ());
        }
        static  _SharedRepByValuePtr mkEmptyStrRep_ () {
            static  bool                            sInited_    =   false;
            static  _SharedRepByValuePtr s_;
            if (not sInited_) {
                sInited_ = true;
                s_ = _SharedRepByValuePtr (DEBUG_NEW String_BufferedArray_Rep_ (nullptr, 0));
            }
            return s_;
        }
    };
}

String::String ()
    : _fRep (MyEmptyString_::mkEmptyStrRep_ ())
{
}

String::String (const char16_t* cString)
    : _fRep (MyEmptyString_::mkEmptyStrRep_ ())
{
    RequireNotNull (cString);
    // Horrible, but temporarily OK impl
    for (const char16_t* i = cString; *i != '\0'; ++i) {
        InsertAt (*i, (i - cString));
    }
}

String::String (const wchar_t* cString)
    : _fRep (cString[0] == '\0' ? MyEmptyString_::mkEmptyStrRep_ () : _SharedRepByValuePtr (DEBUG_NEW String_BufferedArray_Rep_ (cString, cString + wcslen (cString))))
{
    RequireNotNull (cString);
}

String::String (const wchar_t* from, const wchar_t* to)
    : _fRep ((from == to) ? MyEmptyString_::mkEmptyStrRep_ () : _SharedRepByValuePtr (DEBUG_NEW String_BufferedArray_Rep_ (from, to)))
{
    Require (from <= to);
    Require (from != nullptr or from == to);
}

String::String (const Character* from, const Character* to)
    : _fRep ((from == to) ? MyEmptyString_::mkEmptyStrRep_ () : _SharedRepByValuePtr (DEBUG_NEW String_BufferedArray_Rep_ (reinterpret_cast<const wchar_t*> (from), reinterpret_cast<const wchar_t*> (to))))
{
    static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
    Require (from <= to);
    Require (from != nullptr or from == to);
}

String::String (const std::wstring& r)
    : _fRep (r.empty () ? MyEmptyString_::mkEmptyStrRep_ () : _SharedRepByValuePtr (DEBUG_NEW String_BufferedArray_Rep_ (r.data (), r.data () + r.length ())))
{
}

String::String (const _SharedRepByValuePtr::shared_ptr_type& rep)
    : _fRep (rep, _Rep_Cloner ())
{
    RequireNotNull (rep.get ());
}

String::String (const _SharedRepByValuePtr::shared_ptr_type&&  rep)
    : _fRep (std::move (rep), _Rep_Cloner ())
{
    RequireNotNull (rep.get ());
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

void    String::SetLength (size_t newLength)
{
    try {
        if (newLength == 0) {
            _fRep->RemoveAll ();
        }
        else {
            _fRep->SetLength (newLength);
        }
    }
    catch (const _IRep::UnsupportedFeatureException&) {
        Concrete::String_BufferedArray    tmp =   Concrete::String_BufferedArray (*this, max (GetLength (), newLength));
        _fRep = tmp._fRep;
        if (newLength == 0) {
            _fRep->RemoveAll ();
        }
        else {
            _fRep->SetLength (newLength);
        }
    }
}

void    String::SetCharAt (Character c, size_t i)
{
    Require (i >= 0);
    Require (i < GetLength ());
    try {
        _fRep->SetAt (c, i);
    }
    catch (const _IRep::UnsupportedFeatureException&) {
        Concrete::String_BufferedArray    tmp =   Concrete::String_BufferedArray (*this);
        _fRep = tmp._fRep;
        _fRep->SetAt (c, i);
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
        _fRep->InsertAt (from, to, at);
    }
    catch (const _IRep::UnsupportedFeatureException&) {
        Concrete::String_BufferedArray    tmp =   Concrete::String_BufferedArray (*this, GetLength () + (to - from));
        _fRep = tmp._fRep;
        _fRep->InsertAt (from, to, at);
    }
}

void    String::RemoveAt (size_t index, size_t nCharsToRemove)
{
    Require (index + nCharsToRemove <= GetLength ());
    try {
        _fRep->RemoveAt (index, nCharsToRemove);
    }
    catch (const _IRep::UnsupportedFeatureException&) {
        Concrete::String_BufferedArray    tmp =   Concrete::String_BufferedArray (*this);
        _fRep = tmp._fRep;
        _fRep->RemoveAt (index, nCharsToRemove);
    }
}

void    String::Remove (Character c)
{
    size_t index = Find (c, CompareOptions::eWithCase);
    if (index != kBadIndex) {
        RemoveAt (index, 1);
    }
}

size_t  String::Find (Character c, size_t startAt, CompareOptions co) const
{
    Require (startAt <= GetLength ());
    //TODO: HORRIBLE PERFORMANCE!!!
    size_t length = GetLength ();
    switch (co) {
        case CompareOptions::eCaseInsensitive: {
                for (size_t i = startAt; i < length; i++) {
                    if (_fRep->GetAt (i).ToLowerCase () == c.ToLowerCase ()) {
                        return (i);
                    }
                }
            }
            break;
        case CompareOptions::eWithCase: {
                for (size_t i = startAt; i < length; i++) {
                    if (_fRep->GetAt (i) == c) {
                        return (i);
                    }
                }
            }
            break;
    }
    return (kBadIndex);
}

size_t  String::Find (const String& subString, size_t startAt, CompareOptions co) const
{
    Require (startAt <= GetLength ());
    if (subString.GetLength () == 0) {
        return ((GetLength () == 0) ? kBadIndex : 0);
    }
    if (GetLength () < subString.GetLength ()) {
        return (kBadIndex);   // important test cuz size_t is unsigned
    }

    //TODO: FIX HORRIBLE PERFORMANCE!!!
    size_t  subStrLen   =   subString.GetLength ();
    size_t  limit       =   GetLength () - subStrLen;
    switch (co) {
        case CompareOptions::eCaseInsensitive: {
                for (size_t i = startAt; i <= limit; i++) {
                    for (size_t j = 0; j < subStrLen; j++) {
                        if (_fRep->GetAt (i + j).ToLowerCase () != subString._fRep->GetAt (j).ToLowerCase ()) {
                            goto nogood1;
                        }
                    }
                    return (i);
nogood1:
                    ;
                }
            }
            break;
        case CompareOptions::eWithCase: {
                for (size_t i = startAt; i <= limit; i++) {
                    for (size_t j = 0; j < subStrLen; j++) {
                        if (_fRep->GetAt (i + j) != subString._fRep->GetAt (j)) {
                            goto nogood2;
                        }
                    }
                    return (i);
nogood2:
                    ;
                }
            }
            break;
    }
    return (kBadIndex);
}

pair<size_t, size_t>  String::Find (const RegularExpression& regEx, size_t startAt) const
{
    Require (startAt <= GetLength ());
    Assert (startAt == 0);  // else NYI
    wstring tmp     =   As<wstring> ();
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
    AssertNotImplemented ();
    return result;
}

vector<pair<size_t, size_t>>  String::FindEach (const RegularExpression& regEx) const
{
    vector<pair<size_t, size_t>>  result;
#if     qCompilerAndStdLib_Supports_regex
    wstring         tmp     =   As<wstring> ();
    std::wsmatch    res;
    regex_search (tmp, res, regEx.GetCompiled ());
    result.reserve (res.size ());
    size_t  nMatches    =   res.size ();
    for (size_t mi = 0; mi < nMatches; ++mi) {
        result.push_back (pair<size_t, size_t> (res.position (mi), res.length (mi)));
    }
#else
    AssertNotImplemented ();
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
    //TODO: FIX HORRIBLE PERFORMANCE!!!
    size_t length = GetLength ();
    for (size_t i = length; i > 0; --i) {
        if (_fRep->GetAt (i - 1) == c) {
            return (i - 1);
        }
    }
    return (kBadIndex);
}

size_t  String::RFind (const String& subString) const
{
    //TODO: FIX HORRIBLE PERFORMANCE!!!
    /*
     * Do quickie implementation, and dont worry about efficiency...
     */
    if (subString.GetLength () == 0) {
        return ((GetLength () == 0) ? kBadIndex : GetLength () - 1);
    }

    size_t  subStrLen   =   subString.GetLength ();
    size_t  limit       =   GetLength () - subStrLen + 1;
    for (size_t i = limit; i > 0; --i) {
        if (SubString (i - 1, i - 1 + subStrLen) == subString) {
            return (i - 1);
        }
    }
    return (kBadIndex);
}

bool    String::StartsWith (const String& subString, CompareOptions co) const
{
    if (subString.GetLength () >  GetLength ()) {
        return false;
    }
    return SubString (0, subString.GetLength ()).Compare (subString, co) == 0;
}

bool    String::EndsWith (const String& subString, CompareOptions co) const
{
    if (subString.GetLength () >  GetLength ()) {
        return false;
    }
    return SubString (GetLength () - subString.GetLength (), GetLength ()).Compare (subString, co) == 0;
}

bool    String::Match (const RegularExpression& regEx) const
{
    wstring tmp =   As<wstring> ();
    return regex_match (tmp.begin(), tmp.end(), regEx.GetCompiled ());
}

String  String::ReplaceAll (const RegularExpression& regEx, const String& with, CompareOptions co) const
{
#if     qCompilerAndStdLib_Supports_regex
    return String (regex_replace (As<wstring> (), regEx.GetCompiled (), with.As<wstring> ()));
#else
    AssertNotImplemented ();
    return String ();
#endif
}

String  String::ReplaceAll (const String& string2SearchFor, const String& with, CompareOptions co) const
{
    AssertNotImplemented ();
#if     ! (defined (__GNUC__) && (__GNUC__ < 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ <= 6))))
    // This causes GCC 4.6.2 or earlier to barf, and since it doesnt work anyhow, dont disable the rest...
    return String (regex_replace (As<wstring> (), wregex (string2SearchFor.As<wstring> ()), with.As<wstring> ()));
#endif
}

String  String::SubString (size_t from, size_t to) const
{
    Require ((from <= to) or (to == kBadIndex));
    Require ((to <= GetLength ()) or (to == kBadIndex));

    size_t  myLength    =   GetLength ();
    size_t  length  =   (to == kBadIndex) ? (myLength - from) : (to - from);
    if (length == 0) {
        return String ();
    }
    if ((from == 0) and (length == myLength)) {
        return *this;       // just bump reference count
    }
#if     qString_SubStringClassWorks
    return (String (DEBUG_NEW String_Substring_::MyRep_ (_fRep, from, length), false));
#else
    return (String (_fRep->Peek () + from, _fRep->Peek () + from + length));
#endif
}

String  String::LTrim (bool (*shouldBeTrimmmed) (Character)) const
{
    RequireNotNull (shouldBeTrimmmed);
    //TODO: FIX HORRIBLE PERFORMANCE!!!
    // Could be much more efficient if pushed into REP - so we avoid each character virtual call...
    size_t length = GetLength ();
    for (size_t i = 0; i < length; ++i) {
        if (not (*shouldBeTrimmmed) (_fRep->GetAt (i))) {
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
    RequireNotNull (shouldBeTrimmmed);
    //TODO: FIX HORRIBLE PERFORMANCE!!!
    // Could be much more efficient if pushed into REP - so we avoid each character virtual call...
    size_t length = GetLength ();
    if (length != 0) {
        for (ptrdiff_t i = length - 1; i != 0; --i) {
            if (not (*shouldBeTrimmmed) (_fRep->GetAt (i))) {
                size_t  nCharsRemoved   =   (length - 1) - i;
                if (nCharsRemoved == 0) {
                    // no change in string
                    return *this;
                }
                else {
                    return SubString (0, length - nCharsRemoved);
                }
            }
        }
    }
    // all trimmed
    return String ();
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
    RequireNotNull (removeCharIf);

    // TODO: optimize special case where removeCharIf is always false
    //
    // Walk string and find first character we need to remove
    size_t  n   =   GetLength ();
    for (size_t i = 0; i < n; ++i) {
        Character   c   =   operator[] (i);
        if ((removeCharIf) (c)) {
            // on first removal, clone part of string done so far, and start appending
            String  tmp =   SubString (0, i);
            // Now keep iterating IN THIS LOOP appending characters and return at the end of this loop
            i++;
            for (; i < n; ++i) {
                c   =   operator[] (i);
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
    //TODO: FIX HORRIBLE PERFORMANCE!!! (e.g. access wchar_t* const and do there?
    size_t  n   =   GetLength ();
    for (size_t i = 0; i < n; ++i) {
        Character   c   =   operator[] (i);
        if (not c.IsWhitespace ()) {
            return false;
        }
    }
    return true;
}

String  String::LimitLength (size_t maxLen, bool keepLeft) const
{
    static  const String kELIPSIS_  = L"...";
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
    size_t  len =   GetLength ();
    into->reserve (len);
    for (size_t i = 0; i < len; ++i) {
        into->push_back (operator[] (i).GetAsciiCode ());
    }
}

const wchar_t*  String::c_str () const
{
    const   wchar_t*    result  =   _fRep->c_str_peek ();
    if (result == nullptr) {
        // Then we must force it to be NUL-terminated
        //
        // We want to DECLARE c_str() as const, becuase it doesn't conceptually change the underlying data, but to get the
        // fRep cloning stuff to work right, we must access it through a
        String* REALTHIS    =   const_cast<String*> (this);
        try {
            return REALTHIS->_fRep->c_str_change ();
        }
        catch (const _IRep::UnsupportedFeatureException&) {
            Concrete::String_BufferedArray    tmp =   Concrete::String_BufferedArray (*this, GetLength () + 1);
            REALTHIS->_fRep = tmp._fRep;
            return REALTHIS->_fRep->c_str_change ();
        }
    }
    else {
        return result;
    }
}

void    String::erase (size_t from, size_t count)
{
    // TODO: Double check STL definition - but I think they allow for count to be 'too much' - and silently trim to end...
    size_t  max2Erase    =   static_cast<size_t> (max (static_cast<ptrdiff_t> (0), static_cast<ptrdiff_t> (GetLength ()) - static_cast<ptrdiff_t> (from)));
    if (count == kBadIndex) {
        RemoveAt (from, max2Erase);
    }
    else {
        RemoveAt (from,  min (count, max2Erase));
    }
}

String      String::substr (size_t from, size_t count) const
{
    // TODO: Double check STL definition - but I think they allow for count to be 'too much' - and silently trim to end...
    if (count == kBadIndex) {
        return SubString (from);
    }
    else {
        size_t  end =   min (from + count, GetLength ());   // really should worry more about overflow (say if count is kBadIndex-1)
        Assert (from <= end);
        return SubString (from, end);
    }
}







#if     qString_SubStringClassWorks
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
    : fBlockAllocationDependency (Memory::MakeModuleDependency_BlockAllocated ())
{
}
