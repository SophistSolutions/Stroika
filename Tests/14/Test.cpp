/*
 * Copyright(c) Records For Living, Inc. 2004-2012.  All rights reserved
 */
#include	"Stroika/Foundation/StroikaPreComp.h"

#include	<iostream>

#include	"Stroika/Foundation/IO/Network/Transfer/Client.h"

#include	"../TestHarness/TestHarness.h"

using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::IO;
using	namespace	Stroika::Foundation::IO::Network;
using	namespace	Stroika::Foundation::IO::Network::Transfer;



namespace	{
	void	Test_1_SimpleFetch_Google_C_ (Connection c)
		{
			c.SetURL (URL (L"http://www.google.com"));
			Response	r	=	c.Get ();
			VerifyTestResult (r.GetSucceeded ());
			VerifyTestResult (r.fData.size () > 1);
		}
	void	Test_2_SimpleFetch_SSL_Google_C_ (Connection c)
		{
			c.SetURL (URL (L"https://www.google.com"));
			Response	r	=	c.Get ();
			VerifyTestResult (r.GetSucceeded ());
			VerifyTestResult (r.fData.size () > 1);
		}
}


//// CREATE TEMPLATE THAT ITERATES OVER ALL USEFUL CONNECTION TYPES (default one, and special chosen depending on available defines - so eventually
// on windows, we test BTOH win32 and curl (if avaialble).

namespace	{
	void	DoRegressionTests_ ()
		{
			Test_1_SimpleFetch_Google_C_ (CreateConnection ());
			Test_2_SimpleFetch_SSL_Google_C_ (CreateConnection ());
		}
}





int main (int argc, const char* argv[])
{
	Stroika::TestHarness::Setup ();
	Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
	return EXIT_SUCCESS;
}
