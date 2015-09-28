/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/Format.h"
#include    "../Execution/Finally.h"
#include    "../Streams/iostream/InputStreamFromStdIStream.h"

#include    "ArchiveReader.h"


#if     qHasFeature_LZMA
extern "C" {
#include    <lzma/7z.h>
#include    <lzma/7zCrc.h>
}
#endif




#if     qHasFeature_LZMA && defined (_MSC_VER)
// Use #pragma comment lib instead of explicit entry in the lib entry of the project file
#pragma comment (lib, "lzma.lib")
#endif



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchange;


using   Streams::iostream::InputStreamFromStdIStream;

