/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#include	"Stroika/Foundation/StroikaPreComp.h"

#include	<iostream>

#include	"Stroika/Foundation/DataExchangeFormat/XML/SAXReader.h"
#include	"Stroika/Foundation/Debug/Assertions.h"
#include	"Stroika/Foundation/Containers/Common.h"
#include	"Stroika/Foundation/Time/Realtime.h"

#include	"../TestHarness/TestHarness.h"


using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Characters;







namespace	{

	void	DoRegressionTests_ ()
		{
		}
}





int		main (int argc, const char* argv[])
{
	Stroika::TestHarness::Setup ();
	Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
	return EXIT_SUCCESS;
}

