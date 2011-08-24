/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#include	"Stroika/Foundation/StroikaPreComp.h"

#include	<iostream>
#include	<sstream>

#if		qPlatform_Windows
	#include	<Windows.h>
	#include	<winerror.h>
	#include	<wininet.h>		// for error codes
#endif

#include	"Stroika/Foundation/Execution/Exceptions.h"


using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Execution;



namespace	{
	void	_ASSERT_HANDLER_(const char* fileName, int lineNum)
		{
			cerr << "FAILED: " << fileName << ": " << lineNum << endl;
			_exit (EXIT_FAILURE);
		}
}




namespace	{
	void	RegressionTest1_ ()
		{
#if			qPlatform_Windows
			Assert (Platform::Windows::Exception::kERROR_INTERNET_TIMEOUT == ERROR_INTERNET_TIMEOUT);
			Assert (Platform::Windows::Exception::kERROR_INTERNET_INVALID_URL == ERROR_INTERNET_INVALID_URL);
			Assert (Platform::Windows::Exception::kERROR_INTERNET_UNRECOGNIZED_SCHEME == ERROR_INTERNET_UNRECOGNIZED_SCHEME);
			Assert (Platform::Windows::Exception::kERROR_INTERNET_NAME_NOT_RESOLVED == ERROR_INTERNET_NAME_NOT_RESOLVED);
			Assert (Platform::Windows::Exception::kERROR_INTERNET_PROTOCOL_NOT_FOUND == ERROR_INTERNET_PROTOCOL_NOT_FOUND);
			Assert (Platform::Windows::Exception::kERROR_INTERNET_CANNOT_CONNECT == ERROR_INTERNET_CANNOT_CONNECT);
#endif
		}
}



namespace	{

	void	DoRegressionTests_ ()
		{
			try {
				RegressionTest1_ ();
			}
			catch (...) {
				cerr << "FAILED: REGRESSION TEST EXCEPTION" << endl;
				_exit (EXIT_FAILURE);
			}
		}
}




int main(int argc, const char* argv[])
{
#if		defined (_DEBUG)
	Stroika::Foundation::Debug::SetAssertionHandler (_ASSERT_HANDLER_);
#endif
	DoRegressionTests_ ();

	cout << "Succeeded" << endl;
	return EXIT_SUCCESS;
}
