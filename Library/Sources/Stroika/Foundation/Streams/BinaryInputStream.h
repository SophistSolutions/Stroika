/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BinaryInputStream_h_
#define _Stroika_Foundation_Streams_BinaryInputStream_h_    1

#include    "../StroikaPreComp.h"

#include    "../Memory/Common.h"

#include    "InputStream.h"


/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   Either #include Memory/BLOB.h or document in the forward declare why not.
 *
 *      @todo   Redo BinaryInputStream::ReadAll() more efficiently checking isSeekable() and the
 *              precomputing size and doing just one read.
 *
 *      @todo   Perhaps add a GetBytesAvailable() method. This is effectively like converting
 *              blocking to safe read calls...
 *
 *      @todo   Consider adding GetLength() - as an alias for:
 *                  oldOffset o = GetOffset();
 *                  l =  Seek (eFromEnd, 0);
 *                  Seek (eFromStart, o);
 *                  return l;
 *              The reason to add this - as a virtual function - is that it can be more efficient and
 *              semantically pure (multithreading). Do here - not in Seekable - cuz only sensible for
 *              read streams, not write streams. Well defined. Issue is - probably REQUIRE Seekable()
 *              for this. But its possibly sensible even for non-seekable streams???
 *
 */



#pragma message ("Warning: BinaryInputStream FILE DEPRECATED - use InputStream<Byte>")




namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            using   Memory::Byte;

			using	BinaryInputStream = InputStream<Byte>


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Foundation_Streams_BinaryInputStream_h_*/
