/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_iostream_FStreamSupport_h_
#define _Stroika_Foundation_Streams_iostream_FStreamSupport_h_  1

#include    "../../StroikaPreComp.h"

#include    <fstream>

#include    "../../Characters/TString.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {
            namespace   iostream {


                using   Characters::TString;


                /*
                 *  OpenInputFileStream () is like the ifstream::open CTOR - but throws FILE exceptions on failure.
                 *
                 * There is no need to include ios_base::in among the open-modes as this is included automatically.
                 *
                 * A very common thing to include would be ios_base::binary (DEFAULT ON), if you want to assure no CR/LF transformations done, or pass '0'
                 * to remove the 'ios_base::binary' default.
                 *
                 */
                ifstream&   OpenInputFileStream (ifstream* ifStream, const TString& fileName, ios_base::openmode _Mode = ios_base::binary);
                ifstream&   OpenInputFileStream (ifstream& tmpIFStream, const TString& fileName, ios_base::openmode _Mode = ios_base::binary);


                /*
                 *  OpenOutputFileStream () is like the ofstream::open CTOR - but throws FILE exceptions on failure.
                 *
                 * There is no need to include ios_base::out among the open-modes as this is included automatically.
                 *
                 * A very common thing to include would be ios_base::binary (DEFAULT ON), if you want to assure no CR/LF transformations done.
                 *
                 * Another very common thing to include would be ios_base::trunc (DEFAULT ON).
                 *
                 */
                ofstream&   OpenOutputFileStream (ofstream* ofStream, const TString& fileName, ios_base::openmode _Mode = ios_base::trunc | ios_base::binary);
                ofstream&   OpenOutputFileStream (ofstream& tmpOfStream, const TString& fileName, ios_base::openmode _Mode = ios_base::trunc | ios_base::binary);


            }
        }
    }
}
#endif  /*_Stroika_Foundation_Streams_iostream_FStreamSupport_h_*/
