/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Archive_Zip_Writer_h_
#define _Stroika_Foundation_DataExchange_Archive_Zip_Writer__h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Streams/OutputStream.h"

#include "Stroika/Foundation/DataExchange/Archive/Writer.h"

/**
 *  \file
 *
 * TODO:
 */

namespace Stroika::Foundation::DataExchange::Archive::Zip::Writer {

#if qStroika_HasComponent_zlib
    Archive::Writer::Ptr New (const Streams::OutputStream::Ptr<byte>& writeTo);
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_DataExchange_Archive_Zip_Reader_h_*/
