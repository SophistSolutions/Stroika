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

	void	Test_1_TestTickCountGrowsMonotonically_ ()
		{
			DurationSecondsType	start	=	Time::GetTickCount ();
			Execution::Sleep (0.1);
			VerifyTestResult (start <= Time::GetTickCount ());
		}

}

namespace	{

	void	Test_2_TestDate_ ()
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

	void	Test_3_TestDateTime_ ()
		{
			DateTime	d	=	Date (Date::Year (1903), Date::eApril, Date::DayOfMonth (4));
			VerifyTestResult (d.Format4XML () == L"1903-04-04");
		}

}



namespace	{

	void	Test_4_DateTimeTimeT_ ()
		{
			{
				DateTime	d	=	Date (Date::Year (2000), Date::eApril, Date::DayOfMonth (20));
				VerifyTestResult (d.GetUNIXEpochTime () == 956188800);	// source - http://www.onlineconversion.com/unix_time.htm
			}
			{
				DateTime	d	=	DateTime (Date (Date::Year (1995), Date::eJune, Date::DayOfMonth (4)), TimeOfDay (L"3pm"));
				VerifyTestResult (d.GetUNIXEpochTime () == 802278000);	// source - http://www.onlineconversion.com/unix_time.htm
			}
			{
				DateTime	d	=	DateTime (Date (Date::Year (1995), Date::eJune, Date::DayOfMonth (4)), TimeOfDay (L"3:00"));
				VerifyTestResult (d.GetUNIXEpochTime () == 802234800);	// source - http://www.onlineconversion.com/unix_time.htm
			}
#if 0
//this fails - on windows - fix asap
			{
				const	time_t	kTEST	=	802234800;
				DateTime	d	=	DateTime (kTEST;
				VerifyTestResult (d.GetUNIXEpochTime () == kTEST);	// source - http://www.onlineconversion.com/unix_time.htm
			}
#endif
		}

}



namespace	{

	void	DoRegressionTests_ ()
		{
			Test_1_TestTickCountGrowsMonotonically_ ();
			Test_2_TestDate_ ();
			Test_3_TestDateTime_ ();
			Test_4_DateTimeTimeT_ ();
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

