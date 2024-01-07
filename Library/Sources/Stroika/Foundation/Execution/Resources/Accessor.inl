/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Resources_Accessor_inl_
#define _Stroika_Foundation_Execution_Resources_Accessor_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
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

#endif /*_Stroika_Foundation_Execution_Resources_Accessor_inl_*/
