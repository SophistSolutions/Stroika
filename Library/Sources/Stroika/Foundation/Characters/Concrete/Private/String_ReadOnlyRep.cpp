/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#include    "../../../Containers/Common.h"
#include    "../../../Debug/Assertions.h"
#include    "../../../Execution/Exceptions.h"

#include    "String_ReadOnlyRep.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Characters::Concrete::Private;


using   Traversal::Iterator;


/*
 ********************************************************************************
 **************************** ReadOnlyRep::_Rep *********************************
 ********************************************************************************
 */
Traversal::Iterator<Character>  ReadOnlyRep::_Rep::MakeIterator () const
{
    // @todo - FIX FOR THREADAFETY AND SAFETY WHEN UPDATING - BROKEN
    // --LGP 2013-12-17
    struct MyIterRep_ : Iterator<Character>::IRep {
        //UNSAFE OR DOC WHY SAFE??? KEEPS PTR TO BASE STRING REP BUT NOT THREADSAFTY ETC CHECKS
        // MAYBE SHOULD STORE SMART PTR? BUT HOW WITHOUT enabled_shared_from_this which has significnat overhead

        const String::_IRep&    fStringRep_;    // effectively RO, since if anyone modifies, our copy will remain unchanged
        size_t                  fCurIdx;

        MyIterRep_ (const String::_IRep& r, size_t idx = 0)
            : fStringRep_ (r)
            , fCurIdx (idx)
        {
            Require (fCurIdx <= fStringRep_.GetLength ());
        }
        virtual SharedIRepPtr   Clone () const override
        {
            return SharedIRepPtr (new MyIterRep_ (fStringRep_, fCurIdx));
        }
        virtual void    More (Memory::Optional<Character>* result, bool advance) override
        {
            RequireNotNull (result);
            if (advance) {
                Require (fCurIdx <= fStringRep_.GetLength ());
                fCurIdx++;
            }
            if (fCurIdx < fStringRep_.GetLength ()) {
                *result = fStringRep_.GetAt (fCurIdx);
            }
            else {
                result->clear ();
            }
        }
        virtual bool    StrongEquals (const IRep* rhs) const
        {
            AssertNotImplemented ();
            return false;
        }
    };
    return Iterator<Character> (Iterator<Character>::SharedIRepPtr (new MyIterRep_ (*this)));
}

size_t  ReadOnlyRep::_Rep::GetLength () const
{
    Assert (_fStart <= _fEnd);
    return _fEnd - _fStart;
}

bool  ReadOnlyRep::_Rep::IsEmpty () const
{
    Assert (_fStart <= _fEnd);
    return _fEnd == _fStart;
}

void  ReadOnlyRep::_Rep::Apply (_APPLY_ARGTYPE doToElement) const
{
    _Apply (doToElement);
}

Traversal::Iterator<Character>  ReadOnlyRep::_Rep::ApplyUntilTrue (_APPLYUNTIL_ARGTYPE doToElement) const
{
    return _ApplyUntilTrue (doToElement);
}

Character   ReadOnlyRep::_Rep::GetAt (size_t index) const
{
    Assert (_fStart <= _fEnd);
    Require (index < GetLength ());
    return _fStart[index];
}

const Character*    ReadOnlyRep::_Rep::Peek () const
{
    Assert (_fStart <= _fEnd);
    static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
    return ((const Character*)_fStart);
}

pair<const Character*, const Character*> ReadOnlyRep::_Rep::GetData () const
{
    Assert (_fStart <= _fEnd);
    static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
    return pair<const Character*, const Character*> ((const Character*)_fStart, (const Character*)_fEnd);
}

inline  int ReadOnlyRep::_Rep::Compare_CS_ (const Character* rhsStart, const Character* rhsEnd) const
{
// TODO: Need a more efficient implementation - but this should do for starters...
    Assert (_fStart <= _fEnd);
    size_t lLen = _GetLength ();
    size_t rLen = (rhsEnd - rhsStart);
    size_t length   =   min (lLen, rLen);
    for (size_t i = 0; i < length; i++) {
        if (_fStart[i] != rhsStart[i].GetCharacterCode ()) {
            return (_fStart[i] - rhsStart[i].GetCharacterCode ());
        }
    }
    return Containers::CompareResultNormalizeHelper<ptrdiff_t, int> (static_cast<ptrdiff_t> (lLen) - static_cast<ptrdiff_t> (rLen));
}

inline  int ReadOnlyRep::_Rep::Compare_CI_ (const Character* rhsStart, const Character* rhsEnd) const
{
// TODO: Need a more efficient implementation - but this should do for starters...
    Assert (_fStart <= _fEnd);
    // Not sure wcscasecmp even helps because of convert to c-str
    //return ::wcscasecmp (l.c_str (), r.c_str ());;
    size_t lLen = _GetLength ();
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

int ReadOnlyRep::_Rep::Compare (const Character* rhsStart, const Character* rhsEnd, CompareOptions co) const
{
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

void    ReadOnlyRep::_Rep::InsertAt (const Character* srcStart, const Character* srcEnd, size_t index)
{
    Execution::DoThrow (UnsupportedFeatureException ());
}

void    ReadOnlyRep::_Rep::RemoveAll ()
{
    Execution::DoThrow (UnsupportedFeatureException ());
}

void    ReadOnlyRep::_Rep::SetAt (Character item, size_t index)
{
    Execution::DoThrow (UnsupportedFeatureException ());
}

void    ReadOnlyRep::_Rep::RemoveAt (size_t from, size_t to)
{
    Execution::DoThrow (UnsupportedFeatureException ());
}

void    ReadOnlyRep::_Rep::SetLength (size_t newLength)
{
    Execution::DoThrow (UnsupportedFeatureException ());
}

const wchar_t*  ReadOnlyRep::_Rep::c_str_peek () const  noexcept
{
    return nullptr;
}

const wchar_t*      ReadOnlyRep::_Rep::c_str_change ()
{
    Execution::DoThrow (UnsupportedFeatureException ());
}
