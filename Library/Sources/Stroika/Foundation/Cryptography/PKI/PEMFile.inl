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
    template <Common::IAnyOf<Certificate::Ptr, PrivateKey::Ptr> T>
    inline auto PEMFile::Ptr::GetByType () const -> Iterable<T>
    {
        return GetEntries ().Map<Iterable<T>> ([] (const auto& e) -> optional<T> {
            if (auto o = get_if<T> (&e)) {
                return *o;
            }
            else {
                return nullopt;
            }
        });
    }

}
