/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Archive_Zip_Reader_h_
#define _Stroika_Foundation_DataExchange_Archive_Zip_Reader_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/DataExchange/Archive/Reader.h"
#include "Stroika/Foundation/Streams/InputStream.h"

/**
 *  \file
 *
 * TODO:
 */

namespace Stroika::Foundation::DataExchange::Archive::Zip::Reader {

#if qStroika_HasComponent_zlib
    /**
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter</a>
     *          Its const methods too: each moves the reader's cursor in the archive - and so in readFrom, which nothing else may
     *          read or seek meanwhile.
     */
    Archive::Reader::Ptr New (const Streams::InputStream::Ptr<byte>& readFrom);
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_DataExchange_Archive_Zip_Reader_h_*/
