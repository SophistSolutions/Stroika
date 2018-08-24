/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
    inline Accessor::Accessor (const std::byte* start, const std::byte* end)
        : fDataStart_ (start)
        , fDataEnd_ (end)
    {
        Require (start != nullptr or (start == end));
        Require (end != nullptr or (start == end));
        Require (start <= end);
    }
    inline const std::byte* Accessor::begin () const
    {
        return fDataStart_;
    }
    inline const std::byte* Accessor::end () const
    {
        return fDataEnd_;
    }
    inline size_t Accessor::size () const
    {
        return fDataEnd_ - fDataStart_;
    }
    inline vector<std::byte> Accessor::GetData () const
    {
        return vector<std::byte> (fDataStart_, fDataEnd_);
    }

}

#endif /*_Stroika_Foundation_Execution_Resources_Accessor_inl_*/
