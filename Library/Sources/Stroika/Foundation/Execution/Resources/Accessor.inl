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

    //  class   Accessor
    inline Accessor::Accessor (const Byte* start, const Byte* end)
        : fDataStart_ (start)
        , fDataEnd_ (end)
    {
        Require (start != nullptr or (start == end));
        Require (end != nullptr or (start == end));
        Require (start <= end);
    }
    inline const Byte* Accessor::begin () const
    {
        return fDataStart_;
    }
    inline const Byte* Accessor::end () const
    {
        return fDataEnd_;
    }
    inline size_t Accessor::size () const
    {
        return fDataEnd_ - fDataStart_;
    }
    inline vector<Byte> Accessor::GetData () const
    {
        return vector<Byte> (fDataStart_, fDataEnd_);
    }
}

#endif /*_Stroika_Foundation_Execution_Resources_Accessor_inl_*/
