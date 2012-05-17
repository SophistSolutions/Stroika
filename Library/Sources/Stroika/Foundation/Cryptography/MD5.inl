/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    <ostream>
#include    <sstream>

namespace   Stroika {
    namespace   Foundation {
        namespace   Cryptography {
            template    <typename T>
            string  ComputeMD5Digest_UsingOStream (const T& elt) {
                stringstream    out;
                out << elt;
                string  tmp = out.str ();
                return ComputeMD5Digest (vector<Byte> (Containers::Start (tmp), Containers::End (tmp)));
            }
        }
    }
}
