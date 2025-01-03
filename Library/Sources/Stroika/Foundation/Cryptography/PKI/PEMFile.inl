/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::Cryptography::PKI {

    /*
     ********************************************************************************
     ************************ Cryptography::PEMFile::Ptr ****************************
     ********************************************************************************
     */
    inline Memory::BLOB PEMFile::Ptr::GetData () const
    {
        return get ()->GetData ();
    }
    inline auto PEMFile::Ptr::GetEntries () const -> Sequence<EntryType>
    {
        return get ()->GetEntries ();
    }
}
