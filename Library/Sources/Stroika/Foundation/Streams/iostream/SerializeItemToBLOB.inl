/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_iostream_SerializeItemToBLOB_inl_
#define _Stroika_Foundation_Streams_iostream_SerializeItemToBLOB_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    <ostream>
#include    <sstream>

namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {
            namespace   iostream {

                template    <typename T>
                Memory::BLOB  SerializeItemToBLOB (const T& elt)
                {
                    stringstream    out;
                    out << elt;
                    string  tmp = out.str ();
                    return Memory::BLOB (vector<Byte> (Containers::Start (tmp), Containers::End (tmp)));
                }


            }
        }
    }
}
#endif  /*_Stroika_Foundation_Streams_iostream_SerializeItemToBLOB_inl_*/
