/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"
#include "Stroika/Foundation/Streams/TextReader.h"
#include "Stroika/Foundation/Streams/iostream/OutputStreamFromStdOStream.h"

#include "Writer.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;

/*
 ********************************************************************************
 ******************************* Variant::Writer ********************************
 ********************************************************************************
 */
Memory::BLOB Variant::Writer::_WriteAsBLOBHelper (const function<void (Streams::OutputStream::Ptr<byte>)>& f)
{
    Streams::MemoryStream::Ptr<byte> buf = Streams::MemoryStream::New<byte> ();
    f (buf);
    return buf.As<Memory::BLOB> ();
}

String Variant::Writer::_WriteAsStringHelper (const function<void (Streams::OutputStream::Ptr<Characters::Character>)>& f)
{
    Streams::MemoryStream::Ptr<Characters::Character> buf = Streams::MemoryStream::New<Characters::Character> ();
    f (buf);
    return buf.As<String> ();
}

Streams::OutputStream::Ptr<byte> Variant::Writer::_WrapBinaryOutput (ostream& out)
{
    return Streams::iostream::OutputStreamFromStdOStream::New<byte> (out);
}

Streams::OutputStream::Ptr<Characters::Character> Variant::Writer::_WrapTextOutput (wostream& out)
{
    return Streams::iostream::OutputStreamFromStdOStream::New<Characters::Character> (out);
}

Memory::BLOB Variant::Writer::WriteAsBLOB (const VariantValue& v) const
{
    return _WriteAsBLOBHelper ([&v, this] (const Streams::OutputStream::Ptr<byte>& out) { Write (v, out); });
}

String Variant::Writer::WriteAsString (const VariantValue& v) const
{
    return _WriteAsStringHelper ([&v, this] (const Streams::OutputStream::Ptr<Characters::Character>& out) { Write (v, out); });
}
