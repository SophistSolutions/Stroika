/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#include	"Stroika/Foundation/StroikaPreComp.h"

#include	<iostream>
#include	<sstream>

#include	"Stroika/Foundation/Debug/Assertions.h"
#include	"Stroika/Foundation/Debug/Trace.h"
#include	"Stroika/Foundation/Execution/Sleep.h"
#include	"Stroika/Foundation/Time/Date.h"
#include	"Stroika/Foundation/Time/DateTime.h"
#include	"Stroika/Foundation/Time/Duration.h"
#include	"Stroika/Foundation/Time/Realtime.h"

#include	"../TestHarness/TestHarness.h"


using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Time;

using	Stroika::Foundation::Debug::TraceContextBumper;



namespace	{

	void	TestTickCountGrowsMonotonically_ ()
		{
			DurationSecondsType	start	=	Time::GetTickCount ();
			Execution::Sleep (0.1);
			VerifyTestResult (start <= Time::GetTickCount ());
		}

}

namespace	{

	void	TestDate_ ()
		{
			Date	d (Date::Year (1903), Date::eApril, Date::DayOfMonth (4));
			VerifyTestResult (d.Format4XML () == L"1903-04-04");
			d.AddDays (4);
			VerifyTestResult (d.Format4XML () == L"1903-04-08");
			d.AddDays (-4);
			VerifyTestResult (d.Format4XML () == L"1903-04-04");
		}

}


namespace	{

	void	TestDateTime_ ()
		{
			DateTime	d	=	Date (Date::Year (1903), Date::eApril, Date::DayOfMonth (4));
			VerifyTestResult (d.Format4XML () == L"1903-04-04");
		}

}




namespace	{

	void	DoRegressionTests_ ()
		{
			TestTickCountGrowsMonotonically_ ();
			TestDate_ ();
			TestDateTime_ ();
		}
}




#if qOnlyOneMain
extern  int TestDateAndTime ()
#else
int main (int argc, const char* argv[])
#endif
{
	Stroika::TestHarness::Setup ();
	Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
	return EXIT_SUCCESS;
}

