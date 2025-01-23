/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"
#include "Stroika/Foundation/Streams/ToText.h"
#include "Stroika/Foundation/Streams/iostream/OutputStreamFromStdOStream.h"

#include "Writer.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Streams;

/*
 ********************************************************************************
 ******************************* Variant::Writer ********************************
 ********************************************************************************
 */
Memory::BLOB Variant::Writer::_WriteAsBLOBHelper (const function<void (Streams::OutputStream::Ptr<byte>)>& f)
{
    MemoryStream::Ptr<byte> buf = MemoryStream::New<byte> ();
    f (buf);
    return buf.As<Memory::BLOB> ();
}

String Variant::Writer::_WriteAsStringHelper (const function<void (Streams::OutputStream::Ptr<Characters::Character>)>& f)
{
    MemoryStream::Ptr<Characters::Character> buf = MemoryStream::New<Characters::Character> ();
    f (buf);
    return buf.As<String> ();
}

OutputStream::Ptr<byte> Variant::Writer::_WrapBinaryOutput (ostream& out)
{
    return Streams::iostream::OutputStreamFromStdOStream::New<byte> (out);
}

OutputStream::Ptr<Characters::Character> Variant::Writer::_WrapTextOutput (wostream& out)
{
    return Streams::iostream::OutputStreamFromStdOStream::New<Characters::Character> (out);
}

Memory::BLOB Variant::Writer::WriteAsBLOB (const VariantValue& v) const
{
    return _WriteAsBLOBHelper ([&v, this] (const OutputStream::Ptr<byte>& out) { Write (v, out); });
}

String Variant::Writer::WriteAsString (const VariantValue& v) const
{
    return _WriteAsStringHelper ([&v, this] (const OutputStream::Ptr<Characters::Character>& out) { Write (v, out); });
}
