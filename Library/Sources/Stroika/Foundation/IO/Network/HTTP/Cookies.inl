/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_Cookies_inl_
#define _Stroika_Foundation_IO_Network_HTTP_Cookies_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::IO::Network::HTTP {

    /*
     ********************************************************************************
     ************************************ HTTP::Cookie ******************************
     ********************************************************************************
     */
    inline Cookie::Cookie (const String& name, const String& value)
        : KeyValuePair<String, String>{name, value}
    {
    }
    inline String HTTP::Cookie::ToString () const
    {
        return Encode ();
    }

}

#endif /*_Stroika_Foundation_IO_Network_HTTP_Cookies_inl_*/
