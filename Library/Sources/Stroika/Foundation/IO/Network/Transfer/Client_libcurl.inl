/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Transfer_Client_libcurl_inl_
#define _Stroika_Foundation_IO_Network_Transfer_Client_libcurl_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
CompileTimeFlagChecker_HEADER (Stroika::Foundation::IO::Network::Transfer, qHasFeature_LibCurl, qHasFeature_LibCurl);

namespace Stroika::Foundation::IO::Network::Transfer {

#if qHasFeature_LibCurl

	inline void ThrowIfError (CURLcode status)
	{
		if (status != CURLE_OK)
			[[UNLIKELY_ATTR]]
		{
			Execution::Throw (Execution::SystemErrorException<> (status, LibCurl_error_category ()));
		}
	}

    class [[deprecated ("Since v2.1d18, use SystemErrorException{ hr, LibCurl_error_category () }")]] LibCurlException : public Execution::SystemErrorException<>
    {
    public:
        using CURLcode = int; // tried directly to reference libcurl CURLcode but tricky cuz its an enum -- LGP 2012-05-08
    public:
        LibCurlException (CURLcode ccode);

    public:
        // throw Exception () type iff the status indicates a real exception code. This MAY not throw an exception of type LibCurlException,
        // but MAY map to any other exception type
        static void ThrowIfError (CURLcode status);

    public:
        nonvirtual CURLcode GetCode () const
        {
            return fCurlCode_;
        }

    private:
        CURLcode fCurlCode_;
    };
#endif

}

#endif /*_Stroika_Foundation_IO_Network_Transfer_Client_libcurl_inl_*/
