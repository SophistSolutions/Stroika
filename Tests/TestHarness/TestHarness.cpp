/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#include	"Stroika/Foundation/StroikaPreComp.h"

#include	<iostream>


#include	"Stroika/Foundation/Containers/Common.h"
#include	"Stroika/Foundation/Debug/Assertions.h"

#include	"TestHarness.h"


using	namespace	Stroika;
using	namespace	Stroika::Foundation;
using	namespace	Stroika::TestHarness;



namespace	{
	void	_ASSERT_HANDLER_(const char* assertCategory, const char* assertionText, const char* fileName, int lineNum, const char* functionName)
		{
			cerr << "FAILED: " << fileName << ": " << lineNum << endl;
			_exit (EXIT_FAILURE);
		}
}




void	TestHarness::Setup ()
{
#if		qDebug
	Stroika::Foundation::Debug::SetAssertionHandler (_ASSERT_HANDLER_);
#endif
}


void	TestHarness::PrintPassOrFail (void (*regressionTest) ())
{
	try {
		(*regressionTest) ();
		cout << "Succeeded" << endl;
	}
	catch (...) {
		cerr << "FAILED: REGRESSION TEST EXCEPTION" << endl;
		cout << "Failed" << endl;
		_exit (EXIT_FAILURE);
	}
}




