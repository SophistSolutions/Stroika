/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include <ostream>
#include <sstream>

namespace Stroika::Foundation::Streams::iostream {

    template <typename T>
    Memory::BLOB SerializeItemToBLOB (const T& elt)
    {
        stringstream out;
        out << elt;
        string tmp = out.str ();
        return Memory::BLOB (
            vector<byte> (reinterpret_cast<const byte*> (Containers::Start (tmp)), reinterpret_cast<const byte*> (Containers::End (tmp))));
    }

}
