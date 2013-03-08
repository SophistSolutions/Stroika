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
#include    "TString.h"

#include    "String.h"






using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;

using   Memory::SharedByValue;




namespace   {
    /*
     *  UnsupportedFeatureException_
     *
     *          Some 'reps' - don't supprot some features. For exmaple - a READONLY char* rep won't support
     *      any operaiton that modifies the string. Its up to the CONTAINER change the rep to a generic one
     *      that supports everything todo that.
     *
     *          We COULD have done this by having a SUPPORTSX() predicate method called on each rep before
     *      each all, or have an extra return value about if it succeeded. But - that would be alot of
     *      overhead for something likely quite rate. In other words, it will be EXCEPTIONAL that one tries
     *      to change a string that happened to come from a 'readonly' source. We can handle that internally,
     *      and transparently by thorwing an excpetion that never makes it out of the String module/cpp file.
     */
    class   UnsupportedFeatureException_ {};
}



namespace   {

    // This is a utility class to implement most of the basic String::_IRep functionality
    struct  HELPER_ : String {
        struct  _ReadOnlyRep : public String::_IRep {
        public:
            _ReadOnlyRep (const wchar_t* start, const wchar_t* end)
                : _fStart (start)
                , _fEnd (end) {
            }
            nonvirtual  void    _SetData (const wchar_t* start, const wchar_t* end) {
                Require (_fStart <= _fEnd);
                _fStart = start;
                _fEnd = end;
            }
            virtual     size_t  GetLength () const override {
                Assert (_fStart <= _fEnd);
                return _fEnd - _fStart;
            }
            virtual     Character   GetAt (size_t index) const override {
                Assert (_fStart <= _fEnd);
                Require (index < GetLength ());
                return _fStart[index];
            }
            virtual     const Character*    Peek () const override {
                Assert (_fStart <= _fEnd);
                static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
                return ((const Character*)_fStart);
            }
            virtual pair<const Character*, const Character*> GetData () const override {
                Assert (_fStart <= _fEnd);
                static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
                return pair<const Character*, const Character*> ((const Character*)_fStart, (const Character*)_fEnd);
            }
            nonvirtual  int Compare_CS_ (const Character* rhsStart, const Character* rhsEnd) const {
// TODO: Need a more efficient implementation - but this should do for starters...
                Assert (_fStart <= _fEnd);
                size_t lLen = GetLength ();
                size_t rLen = (rhsEnd - rhsStart);
                size_t length   =   min (lLen, rLen);
                for (size_t i = 0; i < length; i++) {
                    if (_fStart[i] != rhsStart[i]) {
                        return (_fStart[i] - rhsStart[i].GetCharacterCode ());
                    }
                }
                return Containers::CompareResultNormalizeHelper<ptrdiff_t, int> (static_cast<ptrdiff_t> (lLen) - static_cast<ptrdiff_t> (rLen));
            }
            nonvirtual  int Compare_CI_ (const Character* rhsStart, const Character* rhsEnd) const {
// TODO: Need a more efficient implementation - but this should do for starters...
                Assert (_fStart <= _fEnd);
                // Not sure wcscasecmp even helps because of convert to c-str
                //return ::wcscasecmp (l.c_str (), r.c_str ());;
                size_t lLen = GetLength ();
                size_t rLen = (rhsEnd - rhsStart);
                size_t length   =   min (lLen, rLen);
                for (size_t i = 0; i < length; i++) {
                    Character   lc  =   Character (_fStart[i]).ToLowerCase ();
                    Character   rc  =   rhsStart[i].ToLowerCase ();
                    if (lc.GetCharacterCode () != rc.GetCharacterCode ()) {
                        return (lc.GetCharacterCode () - rc.GetCharacterCode ());
                    }
                }
                return Containers::CompareResultNormalizeHelper<ptrdiff_t, int> (static_cast<ptrdiff_t> (lLen) - static_cast<ptrdiff_t> (rLen));
            }
            virtual int Compare (const Character* rhsStart, const Character* rhsEnd, CompareOptions co) const override {
                Require (co == CompareOptions::eWithCase or co == CompareOptions::eCaseInsensitive);
                Assert (_fStart <= _fEnd);
                switch (co) {
                    case    CompareOptions::eWithCase:
                        return Compare_CS_ (rhsStart, rhsEnd);
                    case    CompareOptions::eCaseInsensitive:
                        return Compare_CI_ (rhsStart, rhsEnd);
                    default:
                        AssertNotReached ();
                        return 0;
                }
            }
            virtual void    InsertAt (const Character* srcStart, const Character* srcEnd, size_t index) override {
                Execution::DoThrow (UnsupportedFeatureException_ ());
            }
            virtual void    RemoveAll () override {
                Execution::DoThrow (UnsupportedFeatureException_ ());
            }
            virtual void    SetAt (Character item, size_t index) {
                Execution::DoThrow (UnsupportedFeatureException_ ());
            }
            virtual void    RemoveAt (size_t index, size_t amountToRemove) {
                Execution::DoThrow (UnsupportedFeatureException_ ());
            }
            virtual void    SetLength (size_t newLength) override {
                Execution::DoThrow (UnsupportedFeatureException_ ());
            }
            virtual const wchar_t*  c_str_peek () const  noexcept override {
                return nullptr;
            }
            virtual const wchar_t*      c_str_change () override {
                Execution::DoThrow (UnsupportedFeatureException_ ());
            }

        protected:
            const wchar_t*  _fStart;
            const wchar_t*  _fEnd;
        };

        // This is a utility class to implement most of the basic String::_IRep functionality. This implements functions that change the string, but dont GROW it,
        // since we don't know in general we can (thats left to subtypes)
        struct  _ReadWriteRep : public _ReadOnlyRep {
        public:
            _ReadWriteRep (wchar_t* start, wchar_t* end)
                : _ReadOnlyRep (start, end) {
            }
            virtual void    RemoveAll () override {
                Assert (_fStart <= _fEnd);
                _fEnd = _fStart;
            }
            virtual void    SetAt (Character item, size_t index) {
                Assert (_fStart <= _fEnd);
                Require (index < GetLength ());
                PeekStart ()[index] = item.As<wchar_t> ();
            }
            virtual void    RemoveAt (size_t index, size_t amountToRemove) {
                Assert (_fStart <= _fEnd);
                Require (index + amountToRemove <= GetLength ());
                wchar_t*    lhs =   &PeekStart () [index];
                wchar_t*    rhs =   &lhs [amountToRemove];
                for (size_t i = (_fEnd - _fStart) - index - amountToRemove; i > 0; i--) {
                    *lhs++ = *rhs++;
                }
                _fEnd -= amountToRemove;
                Ensure (_fStart <= _fEnd);
            }

            //Presume fStart is really a WRITABLE pointer
            nonvirtual  wchar_t*    PeekStart () {
                return const_cast<wchar_t*> (_fStart);
            }
        };


        // Experimental block allocation scheme for strings. We COULD enahce this to have 2 block sizes - say 16 and 32 characters?
        // But experiment with this a bit first, and see how it goes...
        //      -- LGP 2011-12-04
#ifndef qUseBlockAllocatedForSmallBufStrings
#define qUseBlockAllocatedForSmallBufStrings        qAllowBlockAllocation
#endif
#if     qUseBlockAllocatedForSmallBufStrings
        // #define until we have constexpr working
#define kBlockAllocMinSizeInwchars  16
        struct BufferedStringRepBlock_ {
            wchar_t data[kBlockAllocMinSizeInwchars];
        };
#endif
        struct  BufferedStringRep_ : HELPER_::_ReadWriteRep {
        private:
            typedef HELPER_::_ReadWriteRep  inherited;
        public:
            BufferedStringRep_ (const wchar_t* start, const wchar_t* end)
                : inherited (nullptr, nullptr)
                , fCapacity_ (0) {
                size_t  len     =   end - start;
                ReserveAtLeast_ (len);
                if (len != 0) {
                    AssertNotNull (PeekStart ());
                    (void)::memcpy (PeekStart (), start, len * sizeof (wchar_t));
                    _fEnd = _fStart + len;
                }
            }
            BufferedStringRep_ (const wchar_t* start, const wchar_t* end, size_t reserve)
                : inherited (nullptr, nullptr)
                , fCapacity_ (0) {
                size_t  len     =   end - start;
                ReserveAtLeast_ (max (len, reserve));
                if (len != 0) {
                    AssertNotNull (PeekStart ());
                    (void)::memcpy (PeekStart (), start, len * sizeof (wchar_t));
                    _fEnd = _fStart + len;
                }
            }
            ~BufferedStringRep_ () {
#if     qUseBlockAllocatedForSmallBufStrings
                if (fCapacity_ == kBlockAllocMinSizeInwchars) {
                    Memory::BlockAllocated<BufferedStringRepBlock_>::operator delete (PeekStart ());
                    return;
                }
#endif
                delete[] PeekStart ();
            }
            virtual     void    InsertAt (const Character* srcStart, const Character* srcEnd, size_t index) override {
                Require (index >= 0);
                Require (index <= GetLength ());
                Require (srcStart < srcEnd);

                // cannot insert pointer FROM THIS REP! Doing so would be buggy! MAYBE I need to handle this with special case logic - before I do the resize?
                //      -- LGP 2011-12-04
                Assert (not (_fStart <= reinterpret_cast<const wchar_t*> (srcStart) and reinterpret_cast<const wchar_t*> (srcStart) <= _fEnd));

                size_t      origLen     =   GetLength ();
                size_t      amountToAdd =   (srcEnd - srcStart);
                SetLength (origLen + amountToAdd);
                size_t      newLen      =   origLen + amountToAdd;
                Assert (newLen == GetLength ());
                wchar_t*    gapStart    =   PeekStart () + index;
                // make space for insertion
                (void)::memmove (gapStart + amountToAdd, gapStart, (origLen - index) * sizeof (wchar_t));
                // now copy in new characters
                (void)::memcpy (gapStart, srcStart, amountToAdd * sizeof (wchar_t));
                Ensure (_fStart <= _fEnd);
            }
            virtual     void        SetLength (size_t newLength) override {
                if (newLength < capacity ()) {
                    // We COULD shrink the capacity at this point, but not critical right now...
                }
                else {
                    ReserveAtLeast_ (newLength);
                }
                _fEnd = _fStart + newLength;    // we don't bother doing anything to added/removed characters
                Ensure (GetLength () == newLength);
            }
            virtual     const wchar_t*  c_str_peek () const noexcept override {
                size_t  len =   GetLength ();
                if (len < fCapacity_) {
                    const_cast<wchar_t*> (_fStart)[len] = '\0';     // Cheaper to always set than to check, and maybe set
                    return _fStart;
                }
                return nullptr;
            }
            virtual     const wchar_t*      c_str_change () override {
                ReserveAtLeast_ (GetLength () + 1);
                return c_str_peek ();
            }
            // size() function defined only so we can use Containers::ReserveSpeedTweekAddN() template
            nonvirtual  size_t  size () const {
                return GetLength ();
            }
            nonvirtual  size_t  capacity () const {
                return fCapacity_;
            }
            nonvirtual  void    reserve (size_t newCapacity) {
                if (GetLength () > newCapacity) {
                    // ignore and return
                }
                // Force capacity to match request... even if 'unwise'
                if (fCapacity_ != newCapacity) {
                    size_t      len     =   GetLength ();
                    wchar_t*    newBuf  =   nullptr;
                    if (newCapacity != 0) {
#if     qUseBlockAllocatedForSmallBufStrings
                        if (newCapacity <= kBlockAllocMinSizeInwchars) {
                            newCapacity = kBlockAllocMinSizeInwchars;
                            static_assert (sizeof (BufferedStringRepBlock_) == sizeof (wchar_t) * kBlockAllocMinSizeInwchars, "sizes should match");
                            newBuf = reinterpret_cast<wchar_t*> (Memory::BlockAllocated<BufferedStringRepBlock_>::operator new (sizeof (BufferedStringRepBlock_)));
                        }
#endif
                        if (newBuf == nullptr) {
                            newBuf = DEBUG_NEW wchar_t [newCapacity];
                        }
                        if (len != 0) {
                            (void)::memcpy (newBuf, _fStart, len * sizeof (wchar_t));
                        }
                    }
#if     qUseBlockAllocatedForSmallBufStrings
                    if (fCapacity_ == kBlockAllocMinSizeInwchars) {
                        Memory::BlockAllocated<BufferedStringRepBlock_>::operator delete (PeekStart ());
                    }
                    else {
                        delete[] PeekStart ();
                    }
#else
                    delete[] PeekStart ();
#endif
                    _SetData (newBuf, newBuf + len);
                    fCapacity_ = newCapacity;
                }
            }
            nonvirtual  void    ReserveAtLeast_ (size_t newCapacity) {
                const   size_t  kChunkSize  =   32;
                size_t          len         =   GetLength ();
                if (newCapacity == 0 and len == 0) {
                    reserve (0);
                }
                else {
                    ptrdiff_t   n2Add   =   static_cast<ptrdiff_t> (newCapacity) - static_cast<ptrdiff_t> (len);
                    if (n2Add > 0) {
                        // Could be more efficent inlining the reserve code here - cuz we can avoid the call to length()
                        Containers::ReserveSpeedTweekAddN (*this, static_cast<size_t> (n2Add), kChunkSize);
                    }
                }
            }

        private:
            size_t      fCapacity_;
        };
    };
}












namespace   {
    class   String_BufferedArray_Rep_ : public HELPER_::BufferedStringRep_ {
    private:
        typedef BufferedStringRep_  inherited;
    public:
        String_BufferedArray_Rep_ (const wchar_t* start, const wchar_t* end)
            : BufferedStringRep_ (start, end) {
        }
        String_BufferedArray_Rep_ (const wchar_t* start, const wchar_t* end, size_t reserve)
            : BufferedStringRep_ (start, end, reserve) {
        }
        virtual shared_ptr<_IRep>   Clone () const override {
            return shared_ptr<_IRep> (DEBUG_NEW String_BufferedArray_Rep_ (_fStart, _fEnd));
        }
    public:
        DECLARE_USE_BLOCK_ALLOCATION(String_BufferedArray_Rep_);
    };
}












class   String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly::MyRep_ : public HELPER_::_ReadOnlyRep {
public:
    MyRep_ (const wchar_t* start, const wchar_t* end)
        : _ReadOnlyRep (start, end) {
        Require (start + ::wcslen (start) == end);
    }
    virtual shared_ptr<_IRep>   Clone () const override {
        /*
         * Subtle point. If we are making a clone, its cuz caller wants to change the buffer, and they cannot cuz its readonly, so
         * make a rep that is modifyable
         */
        return shared_ptr<_IRep> (DEBUG_NEW String_BufferedArray_Rep_ (_fStart, _fEnd));
    }
    virtual const wchar_t*  c_str_peek () const  noexcept override {
        // This class ALWAYS constructed with String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly and ALWAYS with NUL-terminated string
        Assert (_fStart + ::wcslen (_fStart) == _fEnd);
        return _fStart;
    }
public:
    DECLARE_USE_BLOCK_ALLOCATION(MyRep_);
};









/*
 * TODO:
 *      o   COULD do better - saving ORIGNIAL BUFFER SIZE - in addition to memory range.
 *          Right now - this class takes a big buffer (possibly) and lets you modify it, and possibly shrunk the string, but if you ever try to insert,
 *          its forgotten its original size (bufsize), and so it mallocs a new buffer (by thorwing unsupported).
 *
 *          Not a biggie issue for now since this class really isn't used (much).
 *              -- LGP 2011-12-03
 *
 */
class   String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite::MyRep_ : public HELPER_::_ReadWriteRep {
public:
    MyRep_ (wchar_t* start, wchar_t* end)
        : _ReadWriteRep (start, end) {
    }
    virtual shared_ptr<_IRep>   Clone () const override {
        /*
         * Subtle point - but since this code involves SHARING buffer space, we cannot have two different string reps both sharing the same pointer. Only
         * one can use it, and the other must make a copy.
         */
        return shared_ptr<_IRep> (DEBUG_NEW String_BufferedArray_Rep_ (_fStart, _fEnd));
    }
public:
    DECLARE_USE_BLOCK_ALLOCATION(MyRep_);
};








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

String::String (const _SharedRepByValuePtr::shared_ptr_type && rep)
    : _fRep (std::move (rep), _Rep_Cloner ())
{
    RequireNotNull (rep.get ());
}

String  String::FromUTF8 (const char* from)
{
    return UTF8StringToWide (from);
}

String  String::FromUTF8 (const std::string& from)
{
    return UTF8StringToWide (from);
}

String  String::FromTString (const TChar* from)
{
    return TString2Wide (from);
}

String  String::FromTString (const basic_string<TChar>& from)
{
    return TString2Wide (from);
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
    catch (const UnsupportedFeatureException_&) {
        String_BufferedArray    tmp =   String_BufferedArray (*this, max (GetLength (), newLength));
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
    catch (const UnsupportedFeatureException_&) {
        String_BufferedArray    tmp =   String_BufferedArray (*this);
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
    catch (const UnsupportedFeatureException_&) {
        String_BufferedArray    tmp =   String_BufferedArray (*this, GetLength () + (to - from));
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
    catch (const UnsupportedFeatureException_&) {
        String_BufferedArray    tmp =   String_BufferedArray (*this);
        _fRep = tmp._fRep;
        _fRep->RemoveAt (index, nCharsToRemove);
    }
}

void    String::Remove (Character c)
{
    size_t index = Find (c, CompareOptions::eWithCase);
    if (index != kBadStringIndex) {
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
    return (kBadStringIndex);
}

size_t  String::Find (const String& subString, size_t startAt, CompareOptions co) const
{
    Require (startAt <= GetLength ());
    if (subString.GetLength () == 0) {
        return ((GetLength () == 0) ? kBadStringIndex : 0);
    }
    if (GetLength () < subString.GetLength ()) {
        return (kBadStringIndex);   // important test cuz size_t is unsigned
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
    return (kBadStringIndex);
}

pair<size_t, size_t>  String::Find (const RegularExpression& regEx, size_t startAt) const
{
    Require (startAt <= GetLength ());
    Assert (startAt == 0);  // else NYI
    wstring tmp     =   As<wstring> ();
    wregex  regExp  =   wregex (regEx.GetAsStr ().As<wstring> (), mkOption_ (regEx.GetSyntaxType ()));
    std::wsmatch res;
    regex_search (tmp, res, regExp);
    if (res.size () >= 1) {
        return pair<size_t, size_t> (res.position (), res.position () + res.length ());
    }
    return pair<size_t, size_t> (kBadStringIndex, kBadStringIndex);
}

vector<String>  String::FindAll (const RegularExpression& regEx) const
{
    vector<String>  result;
    wstring tmp     =   As<wstring> ();
    wregex  regExp  =   wregex (regEx.GetAsStr ().As<wstring> (), mkOption_ (regEx.GetSyntaxType ()));
    std::wsmatch res;
    regex_search (tmp, res, regExp);
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
    return (kBadStringIndex);
}

size_t  String::RFind (const String& subString) const
{
    //TODO: FIX HORRIBLE PERFORMANCE!!!
    /*
     * Do quickie implementation, and dont worry about efficiency...
     */
    if (subString.GetLength () == 0) {
        return ((GetLength () == 0) ? kBadStringIndex : GetLength () - 1);
    }

    size_t  subStrLen   =   subString.GetLength ();
    size_t  limit       =   GetLength () - subStrLen + 1;
    for (size_t i = limit; i > 0; --i) {
        if (SubString (i - 1, i - 1 + subStrLen) == subString) {
            return (i - 1);
        }
    }
    return (kBadStringIndex);
}

bool    String::Match (const RegularExpression& regEx, CompareOptions co) const
{
    wstring tmp =   As<wstring> ();
    return regex_match (tmp.begin(), tmp.end(), wregex (regEx.GetAsStr ().As<wstring> ()));
}

vector<pair<size_t, size_t>>  String::Search (const String& string2SearchFor, CompareOptions co) const
{
    vector<pair<size_t, size_t>>  result;
    AssertNotImplemented ();
    return result;
}

vector<pair<size_t, size_t>>  String::Search (const RegularExpression& regEx, CompareOptions co) const
{
    vector<pair<size_t, size_t>>  result;
#if     qCompilerAndStdLib_Supports_regex_replace
    wstring         tmp     =   As<wstring> ();
    wregex          regExp  =   wregex (regEx.GetAsStr ().As<wstring> (), mkOption_ (regEx.GetSyntaxType ()));
    std::wsmatch    res;
    regex_search (tmp, res, regExp);
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

String  String::ReplaceAll (const RegularExpression& regEx, const String& with, CompareOptions co) const
{
#if     qCompilerAndStdLib_Supports_regex_replace
    return String (regex_replace (As<wstring> (), wregex (regEx.GetAsStr ().As<wstring> (), mkOption_ (regEx.GetSyntaxType ())), with.As<wstring> ()));
#else
    AssertNotImplemented ();
    return String ();
#endif
}

String  String::ReplaceAll (const String& string2SearchFor, const String& with, CompareOptions co) const
{
    AssertNotImplemented ();
    return String (regex_replace (As<wstring> (), wregex (string2SearchFor.As<wstring> ()), with.As<wstring> ()));
}

String  String::SubString (size_t from, size_t to) const
{
    Require ((from <= to) or (to == kBadStringIndex));

    size_t  myLength    =   GetLength ();
    size_t  length  =   (to == kBadStringIndex) ? (myLength - from) : (to - from);
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

template    <>
void    String::AsUTF8 (string* into) const
{
    RequireNotNull (into);
    *into = WideStringToUTF8 (As<wstring> ());  //tmphack impl (but shoudl work)
}

TString String::AsTString () const
{
    TString result;
    AsTString (&result);
    return result;
}

void    String::AsTString (TString* into) const
{
    RequireNotNull (into);
    *into = Wide2TString (As<wstring> ());  // poor inefficient implementation
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
        catch (const UnsupportedFeatureException_&) {
            String_BufferedArray    tmp =   String_BufferedArray (*this, GetLength () + 1);
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
    if (count == kBadStringIndex) {
        RemoveAt (from, max2Erase);
    }
    else {
        RemoveAt (from,  min (count, max2Erase));
    }
}

String      String::substr (size_t from, size_t count) const
{
    // TODO: Double check STL definition - but I think they allow for count to be 'too much' - and silently trim to end...
    if (count == kBadStringIndex) {
        return SubString (from);
    }
    else {
        size_t  end =   min (from + count, GetLength ());   // really should worry more about overflow (say if count is kBadStringIndex-1)
        Assert (from <= end);
        return SubString (from, end);
    }
}







/*
 ********************************************************************************
 ***************************** String_BufferedArray *****************************
 ********************************************************************************
 */
String_BufferedArray::String_BufferedArray ()
    : String (_SharedRepByValuePtr::shared_ptr_type (DEBUG_NEW String_BufferedArray_Rep_ (nullptr, 0)))
{
}

String_BufferedArray::String_BufferedArray (size_t reserve)
    : String (_SharedRepByValuePtr::shared_ptr_type (DEBUG_NEW String_BufferedArray_Rep_ (nullptr, 0, reserve)))
{
}

String_BufferedArray::String_BufferedArray (const wchar_t* cString)
    : String (_SharedRepByValuePtr::shared_ptr_type (DEBUG_NEW String_BufferedArray_Rep_ (cString, cString + wcslen (cString))))
{
}

String_BufferedArray::String_BufferedArray (const wchar_t* cString, size_t reserve)
    : String (_SharedRepByValuePtr::shared_ptr_type (DEBUG_NEW String_BufferedArray_Rep_ (cString, cString + wcslen (cString), reserve)))
{
    Require (GetLength () <= reserve);
}

String_BufferedArray::String_BufferedArray (const wstring& str)
    : String (_SharedRepByValuePtr::shared_ptr_type (DEBUG_NEW String_BufferedArray_Rep_ (str.data (), str.data () + str.length ())))
{
}

String_BufferedArray::String_BufferedArray (const wstring& str, size_t reserve)
    : String (_SharedRepByValuePtr::shared_ptr_type (DEBUG_NEW String_BufferedArray_Rep_ (str.data (), str.data () + str.length (), reserve)))
{
    Require (GetLength () <= reserve);
}

String_BufferedArray::String_BufferedArray (const String& from)
    : String (_SharedRepByValuePtr::shared_ptr_type (DEBUG_NEW String_BufferedArray_Rep_ (from.As<const wchar_t*> (), from.As<const wchar_t*> () + from.GetLength ())))
{
}

String_BufferedArray::String_BufferedArray (const String& from, size_t reserve)
    : String (_SharedRepByValuePtr::shared_ptr_type (DEBUG_NEW String_BufferedArray_Rep_ (from.As<const wchar_t*> (), from.As<const wchar_t*> () + from.GetLength (), reserve)))
{
    Require (GetLength () <= reserve);
}

size_t  String_BufferedArray::capacity () const
{
    AssertNotNull (dynamic_cast<String_BufferedArray_Rep_*> (const_cast<String_BufferedArray*> (this)->_fRep.get ()));
    return dynamic_cast<String_BufferedArray_Rep_*> (const_cast<String_BufferedArray*> (this)->_fRep.get ())->capacity ();
}

void    String_BufferedArray::reserve (size_t n)
{
    AssertNotNull (dynamic_cast<String_BufferedArray_Rep_*> (const_cast<String_BufferedArray*> (this)->_fRep.get ()));
    dynamic_cast<String_BufferedArray_Rep_*> (_fRep.get ())->reserve (n);
}











/*
 ********************************************************************************
 ********** String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly *********
 ********************************************************************************
 */
String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly::String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly (const wchar_t* cString)
    : String (_SharedRepByValuePtr::shared_ptr_type (DEBUG_NEW MyRep_ (cString, cString + wcslen (cString))))
{
}








/*
 ********************************************************************************
 ********** String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite ********
 ********************************************************************************
 */
String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite::String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite (wchar_t* cString)
    : String (_SharedRepByValuePtr::shared_ptr_type (DEBUG_NEW MyRep_ (cString, cString + wcslen (cString))))
{
}








/*
 ********************************************************************************
 ********* String_ExternalMemoryOwnership_StackLifetime_ReadOnly ****************
 ********************************************************************************
 */
String_ExternalMemoryOwnership_StackLifetime_ReadOnly::String_ExternalMemoryOwnership_StackLifetime_ReadOnly (const wchar_t* cString)
    : String (cString)
{
    /* TODO: FIX PERFORMANCE!!!
     *      This implementation conforms to the requirements of the API, so that this class CAN be used safely. However, it does NOT exhibit the performance
     *  advantages the class description promises.
     *
     *      TODO so - it must do its own rep (similar to String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite::MyRep_) - except that it must ALSO have an extra method - FREEZE (or some such).
     *  Then in the DTOR for this envelope, we call FREEZE on that rep - causing it to throw away its unsafe pointer. That must ONLY be done if refcount > 1 (in our DTOR).
     */
}







/*
 ********************************************************************************
 *********** String_ExternalMemoryOwnership_StackLifetime_ReadWrite *************
 ********************************************************************************
 */
String_ExternalMemoryOwnership_StackLifetime_ReadWrite::String_ExternalMemoryOwnership_StackLifetime_ReadWrite (wchar_t* cString)
    : String (cString)
{
    /* TODO: FIX PERFORMANCE!!!
     *      This implementation conforms to the requirements of the API, so that this class CAN be used safely. However, it does NOT exhibit the performance
     *  advantages the class description promises.
     *
     *      TODO so - it must do its own rep (similar to String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite::MyRep_) - except that it must ALSO have an extra method - FREEZE (or some such).
     *  Then in the DTOR for this envelope, we call FREEZE on that rep - causing it to throw away its unsafe pointer. That must ONLY be done if refcount > 1 (in our DTOR).
     */
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
    String_BufferedArray    tmp =   String_BufferedArray (lhs, lhs.size () + rhs.size ());
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
