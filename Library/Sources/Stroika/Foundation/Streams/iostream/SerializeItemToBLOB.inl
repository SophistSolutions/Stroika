/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_iostream_SerializeItemToBLOB_inl_
#define _Stroika_Foundation_Streams_iostream_SerializeItemToBLOB_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <ostream>
#include <sstream>

namespace Stroika::Foundation::Streams::iostream {

    template <typename T>
    Memory::BLOB SerializeItemToBLOB (const T& elt)
    {
        using Memory::Byte;
        stringstream out;
        out << elt;
        string tmp = out.str ();
        return Memory::BLOB (vector<Byte> (reinterpret_cast<const Byte*> (Containers::Start (tmp)), reinterpret_cast<const Byte*> (Containers::End (tmp))));
    }

}
#endif /*_Stroika_Foundation_Streams_iostream_SerializeItemToBLOB_inl_*/
