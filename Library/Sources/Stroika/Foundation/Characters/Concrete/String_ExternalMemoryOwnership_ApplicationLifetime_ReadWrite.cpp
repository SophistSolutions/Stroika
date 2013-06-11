/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    <algorithm>
#include    <cstdarg>
#include    <climits>
#include    <istream>
#include    <string>
#include    <regex>

#include    "../../Containers/Common.h"
#include    "../../Execution/Exceptions.h"
#include    "../../Math/Common.h"
#include    "../../Memory/Common.h"
#include    "../../Memory/BlockAllocated.h"

#include    "String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite.h"





using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Characters::Concrete;




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
                Execution::DoThrow (UnsupportedFeatureException ());
            }
            virtual void    RemoveAll () override {
                Execution::DoThrow (UnsupportedFeatureException ());
            }
            virtual void    SetAt (Character item, size_t index) {
                Execution::DoThrow (UnsupportedFeatureException ());
            }
            virtual void    RemoveAt (size_t index, size_t amountToRemove) {
                Execution::DoThrow (UnsupportedFeatureException ());
            }
            virtual void    SetLength (size_t newLength) override {
                Execution::DoThrow (UnsupportedFeatureException ());
            }
            virtual const wchar_t*  c_str_peek () const  noexcept override {
                return nullptr;
            }
            virtual const wchar_t*      c_str_change () override {
                Execution::DoThrow (UnsupportedFeatureException ());
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
















/*
 ********************************************************************************
 ********** String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite ********
 ********************************************************************************
 */
String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite::String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite (wchar_t* cString)
    : String (_SharedRepByValuePtr::shared_ptr_type (DEBUG_NEW MyRep_ (cString, cString + wcslen (cString))))
{
}
