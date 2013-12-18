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

#include    "Private/String_BufferedStringRep.h"

#include    "String_BufferedArray.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Characters::Concrete;






namespace   {
    class   String_BufferedArray_Rep_ : public Concrete::Private::BufferedStringRep::_Rep {
    private:
        typedef Concrete::Private::BufferedStringRep::_Rep  inherited;
    public:
        String_BufferedArray_Rep_ (const wchar_t* start, const wchar_t* end)
            : inherited (start, end)
        {
        }
        String_BufferedArray_Rep_ (const wchar_t* start, const wchar_t* end, size_t reserve)
            : inherited (start, end, reserve)
        {
        }
        virtual _IterableSharedPtrIRep   Clone () const override
        {
            return _SharedPtrIRep (DEBUG_NEW String_BufferedArray_Rep_ (_fStart, _fEnd));
        }
    public:
        DECLARE_USE_BLOCK_ALLOCATION(String_BufferedArray_Rep_);
    };
}












/*
 ********************************************************************************
 ***************************** String_BufferedArray *****************************
 ********************************************************************************
 */
String_BufferedArray::String_BufferedArray ()
    : String (_SharedPtrIRep (DEBUG_NEW String_BufferedArray_Rep_ (nullptr, 0)))
{
}

String_BufferedArray::String_BufferedArray (size_t reserve)
    : String (_SharedPtrIRep (DEBUG_NEW String_BufferedArray_Rep_ (nullptr, 0, reserve)))
{
}

String_BufferedArray::String_BufferedArray (const wchar_t* cString)
    : String (_SharedPtrIRep (DEBUG_NEW String_BufferedArray_Rep_ (cString, cString + wcslen (cString))))
{
}

String_BufferedArray::String_BufferedArray (const wchar_t* cString, size_t reserve)
    : String (_SharedPtrIRep (DEBUG_NEW String_BufferedArray_Rep_ (cString, cString + wcslen (cString), reserve)))
{
    Require (GetLength () <= reserve);
}

String_BufferedArray::String_BufferedArray (const wstring& str)
    : String (_SharedPtrIRep (DEBUG_NEW String_BufferedArray_Rep_ (str.data (), str.data () + str.length ())))
{
}

String_BufferedArray::String_BufferedArray (const wstring& str, size_t reserve)
    : String (_SharedPtrIRep (DEBUG_NEW String_BufferedArray_Rep_ (str.data (), str.data () + str.length (), reserve)))
{
    Require (GetLength () <= reserve);
}

String_BufferedArray::String_BufferedArray (const String& from)
    : String (_SharedPtrIRep (DEBUG_NEW String_BufferedArray_Rep_ (from.As<const wchar_t*> (), from.As<const wchar_t*> () + from.GetLength ())))
{
}

String_BufferedArray::String_BufferedArray (const String& from, size_t reserve)
    : String (_SharedPtrIRep (DEBUG_NEW String_BufferedArray_Rep_ (from.As<const wchar_t*> (), from.As<const wchar_t*> () + from.GetLength (), reserve)))
{
    Require (GetLength () <= reserve);
}

size_t  String_BufferedArray::capacity () const
{
    AssertMember (&_GetRep (), String_BufferedArray_Rep_);
    return static_cast<const String_BufferedArray_Rep_*> (&_GetRep ())->capacity ();
}

void    String_BufferedArray::reserve (size_t n)
{
    AssertMember (&_GetRep (), String_BufferedArray_Rep_);
    return static_cast<String_BufferedArray_Rep_*> (&_GetRep ())->reserve (n);
}
