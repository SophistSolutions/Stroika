/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Characters/Format.h"
#include "../../Streams/MemoryStream.h"
#include "../../Streams/TextReader.h"
#include "../../Streams/iostream/OutputStreamFromStdOStream.h"

#include "Writer.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;

using Streams::iostream::OutputStreamFromStdOStream;

/*
 ********************************************************************************
 ******************************* Variant::Writer ********************************
 ********************************************************************************
 */
Memory::BLOB Variant::Writer::_WriteAsBLOBHelper (const function<void (Streams::OutputStream<byte>::Ptr)>& f)
{
    Streams::MemoryStream<byte>::Ptr buf = Streams::MemoryStream<byte>::New ();
    f (buf);
    return buf.As<Memory::BLOB> ();
}

String Variant::Writer::_WriteAsStringHelper (const function<void (Streams::OutputStream<Characters::Character>::Ptr)>& f)
{
    Streams::MemoryStream<Characters::Character>::Ptr buf = Streams::MemoryStream<Characters::Character>::New ();
    f (buf);
    return buf.As<String> ();
}

Streams::OutputStream<byte>::Ptr Variant::Writer::_WrapBinaryOutput (ostream& out)
{
    return OutputStreamFromStdOStream<byte>::New (out);
}

Streams::OutputStream<Characters::Character>::Ptr Variant::Writer::_WrapTextOutput (wostream& out)
{
    return OutputStreamFromStdOStream<Characters::Character>::New (out);
}

Memory::BLOB Variant::Writer::WriteAsBLOB (const VariantValue& v)
{
    return _WriteAsBLOBHelper ([&v, this] (const Streams::OutputStream<byte>::Ptr& out) { Write (v, out); });
}

String Variant::Writer::WriteAsString (const VariantValue& v)
{
    return _WriteAsStringHelper ([&v, this] (const Streams::OutputStream<Characters::Character>::Ptr& out) { Write (v, out); });
}
