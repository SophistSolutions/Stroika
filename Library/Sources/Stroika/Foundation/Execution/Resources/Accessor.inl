/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::Execution::Resources {

    /*
     ********************************************************************************
     ************************************* Accessor *********************************
     ********************************************************************************
     */
    inline Accessor::Accessor (span<const byte> s)
        : fData_{s}
    {
    }
    inline const byte* Accessor::begin () const
    {
        return fData_.data ();
    }
    inline const byte* Accessor::end () const
    {
        return fData_.data () + fData_.size ();
    }
    inline size_t Accessor::size () const
    {
        return fData_.size ();
    }
    inline span<const byte> Accessor::GetData () const
    {
        return fData_;
    }

}
