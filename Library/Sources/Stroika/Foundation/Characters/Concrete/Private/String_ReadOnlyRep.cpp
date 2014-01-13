/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
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
    struct MyIterRep_ : Iterator<Character>::IRep {
        _SharedPtrIRep  fStr;           // effectively RO, since if anyone modifies, our copy will remain unchanged
        size_t          fCurIdx;
        MyIterRep_ (const _SharedPtrIRep& r, size_t idx = 0)
            : fStr (r)
            , fCurIdx (idx)
        {
            Require (fCurIdx <= fStr->GetLength ());
        }
        virtual SharedIRepPtr   Clone () const override
        {
            return SharedIRepPtr (new MyIterRep_ (fStr, fCurIdx));
        }
        virtual const void* GetOwner () const override
        {
            //tmphack but adequate
            // should NOT require locking is readonly immutable value provided at construction
            return nullptr;
        }
        virtual void    More (Memory::Optional<Character>* result, bool advance) override
        {
            RequireNotNull (result);
            if (advance) {
                Require (fCurIdx <= fStr->GetLength ());
                fCurIdx++;
            }
            if (fCurIdx < fStr->GetLength ()) {
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
    return Iterator<Character> (Iterator<Character>::SharedIRepPtr (new MyIterRep_ (dynamic_pointer_cast<String::_SharedPtrIRep::element_type> (Clone ()))));
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
