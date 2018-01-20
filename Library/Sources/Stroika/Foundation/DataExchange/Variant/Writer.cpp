/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Characters/Format.h"
#include "../../Streams/MemoryStream.h"
#include "../../Streams/TextReader.h"
#include "../../Streams/iostream/OutputStreamFromStdOStream.h"

#include "Writer.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;

using Streams::iostream::OutputStreamFromStdOStream;

/*
 ********************************************************************************
 ******************************* Variant::Writer ********************************
 ********************************************************************************
 */
void Variant::Writer::Write (const VariantValue& v, ostream& out)
{
    Write (v, OutputStreamFromStdOStream<Memory::Byte>::New (out));
}

void Variant::Writer::Write (const VariantValue& v, wostream& out)
{
    Write (v, OutputStreamFromStdOStream<Characters::Character>::New (out));
}

Memory::BLOB Variant::Writer::WriteAsBLOB (const VariantValue& v)
{
    Streams::MemoryStream<Memory::Byte>::Ptr buf = Streams::MemoryStream<Memory::Byte>::New ();
    Write (v, buf);
    return buf.As<Memory::BLOB> ();
}

String Variant::Writer::WriteAsString (const VariantValue& v)
{
    return Streams::TextReader::New (WriteAsBLOB (v)).ReadAll ();
}
