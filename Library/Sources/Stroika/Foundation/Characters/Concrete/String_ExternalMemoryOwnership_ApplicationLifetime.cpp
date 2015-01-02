/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    <algorithm>
#include    <climits>
#include    <string>

#include    "../../Containers/Common.h"
#include    "../../Execution/Exceptions.h"
#include    "../../Math/Common.h"
#include    "../../Memory/Common.h"
#include    "../../Memory/BlockAllocated.h"

#include    "Private/String_BufferedStringRep.h"

#include    "String_ExternalMemoryOwnership_ApplicationLifetime.h"





using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Characters::Concrete;


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
        virtual _IterableSharedPtrIRep   Clone (IteratorOwnerID forIterableEnvelope) const override
        {
            // Because of 'Design Choice - Iterable<T> / Iterator<T> behavior' in String class docs - we
            // ignore suggested IteratorOwnerID
            return _SharedPtrIRep (new String_BufferedArray_Rep_ (_fStart, _fEnd));
        }
    public:
        DECLARE_USE_BLOCK_ALLOCATION(String_BufferedArray_Rep_);
    };
}





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
        return _SharedPtrIRep (new String_BufferedArray_Rep_ (_fStart, _fEnd));
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




