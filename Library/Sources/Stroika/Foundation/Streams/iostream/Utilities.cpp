/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../Characters/String_Constant.h"
#include    "../../Containers/Common.h"
#include    "../../Execution/Exceptions.h"
#include    "../../Execution/ErrNoException.h"
#include    "../../Memory/SmallStackBuffer.h"

#include    "Utilities.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::Memory;
using   namespace   Stroika::Foundation::Streams;
using   namespace   Stroika::Foundation::Streams::iostream;

using   Characters::String_Constant;







/*
 ********************************************************************************
 ********************* Streams::iostream::ReadTextStream ************************
 ********************************************************************************
 */
wstring Streams::iostream::ReadTextStream (istream& in)
{
    streamoff   start   =   in.tellg ();
    in.seekg (0, ios_base::end);
    streamoff   end     =   in.tellg ();
    Assert (start <= end);
    DISABLE_COMPILER_MSC_WARNING_START(6237)
    if ((sizeof (streamoff) > sizeof (size_t)) and ((end - start) > static_cast<streamoff> (numeric_limits<ptrdiff_t>::max ()))) {
        Execution::Throw (Execution::StringException (String_Constant (L"stream too large")));
    }
    size_t  bufLen  =   static_cast<size_t> (end - start);
    Memory::SmallStackBuffer<Byte>  buf (bufLen);
    in.seekg (start, ios_base::beg);
    in.read (reinterpret_cast<char*> (buf.begin ()), bufLen);
    size_t readLen = static_cast<size_t> (in.gcount ());
    Assert (readLen <= bufLen);
    const char* startOfBuf  =   reinterpret_cast<const char*> (static_cast<const Byte*> (buf));
    return Characters::MapUNICODETextWithMaybeBOMTowstring (startOfBuf, startOfBuf + readLen);
    DISABLE_COMPILER_MSC_WARNING_END(6237)
}

wstring Streams::iostream::ReadTextStream (wistream& in)
{
    streamoff   start   =   in.tellg ();
    in.seekg (0, ios_base::end);
    streamoff   end     =   in.tellg ();
    Assert (start <= end);
    DISABLE_COMPILER_MSC_WARNING_START(6237)
    if ((sizeof (streamoff) > sizeof (size_t)) and ((end - start) > static_cast<streamoff> (numeric_limits<ptrdiff_t>::max ()))) {
        Execution::Throw (Execution::StringException (String_Constant (L"stream too large")));
    }
    size_t  bufLen  =   static_cast<size_t> (end - start);
    Memory::SmallStackBuffer<wchar_t>   buf (bufLen);
    in.seekg (start, ios_base::beg);
    in.read (reinterpret_cast<wchar_t*> (buf.begin ()), bufLen);
    size_t readLen = static_cast<size_t> (in.gcount ());
    Assert (readLen <= bufLen);
    const wchar_t*  startOfBuf  =   reinterpret_cast<const wchar_t*> (static_cast<const wchar_t*> (buf));
    return wstring (startOfBuf, startOfBuf + readLen);
    DISABLE_COMPILER_MSC_WARNING_END(6237)
}




/*
 ********************************************************************************
 *********************** Streams::iostream::ReadBytes ***************************
 ********************************************************************************
 */
vector<Byte>    Streams::iostream::ReadBytes (istream& in)
{
    streamoff   start   =   in.tellg ();
    in.seekg (0, ios_base::end);
    streamoff   end     =   in.tellg ();
    Assert (start <= end);
    DISABLE_COMPILER_MSC_WARNING_START(6237)
    if ((sizeof (streamoff) > sizeof (size_t)) and ((end - start) > static_cast<streamoff> (numeric_limits<ptrdiff_t>::max ()))) {
        Execution::Throw (StringException (String_Constant (L"stream too large")));
    }
    size_t  len =   static_cast<size_t> (end - start);
    SmallStackBuffer<Byte>  buf (len);
    in.seekg (start, ios_base::beg);
    in.read (reinterpret_cast<char*> (buf.begin ()), len);
    size_t xxx = static_cast<size_t> (in.gcount ());
    Assert (xxx <= len);
    return vector<Byte> (static_cast<const Byte*> (buf), static_cast<const Byte*> (buf) + xxx);
    DISABLE_COMPILER_MSC_WARNING_END(6237)
}








/*
 ********************************************************************************
 ********************** Streams::iostream::WriteBytes ***************************
 ********************************************************************************
 */
void    Streams::iostream::WriteBytes (ostream& out, const vector<Byte>& s)
{
    out.write (reinterpret_cast<const char*> (Containers::Start (s)), s.size ());
}


