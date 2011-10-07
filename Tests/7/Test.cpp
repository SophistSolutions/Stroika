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

	void	Test_2_TestTimeOfDay_ ()
		{
			{
				TimeOfDay	t;
				VerifyTestResult (t.empty ());
				TimeOfDay	t2 (2);
				VerifyTestResult (t < t2);
				VerifyTestResult (t.GetAsSecondsCount () == 0);
				VerifyTestResult (not t2.empty ());
				VerifyTestResult (t.Format ().empty ());
				VerifyTestResult (not t2.Format ().empty ());
				VerifyTestResult (t2.GetHours () == 0);
				VerifyTestResult (t2.GetMinutes () == 0);
				VerifyTestResult (t2.GetSeconds () == 2);
			}
			{
				TimeOfDay	t2 (5 * 60 * 60 + 3 * 60 + 49);
				VerifyTestResult (t2.GetHours () == 5);
				VerifyTestResult (t2.GetMinutes () == 3);
				VerifyTestResult (t2.GetSeconds () == 49);
			}
			{
				TimeOfDay	t2 (25 * 60 * 60);
				VerifyTestResult (t2.GetHours () == 23);
				VerifyTestResult (t2.GetMinutes () == 59);
				VerifyTestResult (t2.GetSeconds () == 59);
				VerifyTestResult (t2 == TimeOfDay::kMax);
			}
		}

}


namespace	{

	void	Test_3_TestDate_ ()
		{
			{
				Date	d (Year (1903), eApril, DayOfMonth (4));
				VerifyTestResult (d.Format (Date::eXML_PF) == L"1903-04-04");
				d.AddDays (4);
				VerifyTestResult (d.Format (Date::eXML_PF) == L"1903-04-08");
				d.AddDays (-4);
				VerifyTestResult (d.Format (Date::eXML_PF) == L"1903-04-04");
			}
			{
				Date	d	=	Date::Parse (L"1752-09-14", Date::eCurrentLocale_PF);
				VerifyTestResult (not d.empty ());
				VerifyTestResult (d == Date::kMin);
				VerifyTestResult (d.Format (Date::eXML_PF) == L"1752-09-14");	// xml cuz otherwise we get confusion over locale - COULD use hardwired US locale at some point?
			}
			{
				Date	d;
				VerifyTestResult (d.empty ());
				VerifyTestResult (d < DateTime::GetToday ());
				VerifyTestResult (not (DateTime::GetToday () > d));
			}
			{
				Date	d	=	Date::kMin;
				VerifyTestResult (not d.empty ());
				VerifyTestResult (d < DateTime::Now ().GetDate ());
				VerifyTestResult (not (DateTime::Now ().GetDate () < d));
				VerifyTestResult (d.Format (Date::eXML_PF) == L"1752-09-14");	// xml cuz otherwise we get confusion over locale - COULD use hardwired US locale at some point?
			}
		}

}


namespace	{

	void	Test_4_TestDateTime_ ()
		{
			{
				DateTime	d	=	Date (Year (1903), eApril, DayOfMonth (4));
				VerifyTestResult (d.Format (DateTime::eXML_PF) == L"1903-04-04");
			}
			{
				DateTime	d;
				VerifyTestResult (d.empty ());
				VerifyTestResult (d < DateTime::Now ());
				VerifyTestResult (not (DateTime::Now () > d));
			}
			{
				DateTime	d	=	DateTime::kMin;
				VerifyTestResult (not d.empty ());
				VerifyTestResult (d < DateTime::Now ());
				VerifyTestResult (not (DateTime::Now () > d));
				VerifyTestResult (d.Format (DateTime::eXML_PF) == L"1752-09-14T00:00:00");	// xml cuz otherwise we get confusion over locale - COULD use hardwired US locale at some point?
			}
		}

}



namespace	{

	void	Test_5_DateTimeTimeT_ ()
		{
			{
				DateTime	d	=	Date (Year (2000), eApril, DayOfMonth (20));
				VerifyTestResult (d.GetUNIXEpochTime () == 956188800);	// source - http://www.onlineconversion.com/unix_time.htm
			}
			{
				DateTime	d	=	DateTime (Date (Year (1995), eJune, DayOfMonth (4)), TimeOfDay::Parse (L"3pm", TimeOfDay::eCurrentLocale_PF));
				VerifyTestResult (d.GetUNIXEpochTime () == 802278000);	// source - http://www.onlineconversion.com/unix_time.htm
			}
			{
				DateTime	d	=	DateTime (Date (Year (1995), eJune, DayOfMonth (4)), TimeOfDay::Parse (L"3:00", TimeOfDay::eCurrentLocale_PF));
				VerifyTestResult (d.GetUNIXEpochTime () == 802234800);	// source - http://www.onlineconversion.com/unix_time.htm
			}
#if 0
//this fails - on windows - fix asap
			{
				const	time_t	kTEST	=	802234800;
				DateTime	d	=	DateTime (kTEST);
				VerifyTestResult (d.GetUNIXEpochTime () == kTEST);	// source - http://www.onlineconversion.com/unix_time.htm
			}
#endif
		}

}



namespace	{

	void	DoRegressionTests_ ()
		{
			Test_1_TestTickCountGrowsMonotonically_ ();
			Test_2_TestTimeOfDay_ ();
			Test_3_TestDate_ ();
			Test_4_TestDateTime_ ();
			Test_5_DateTimeTimeT_ ();
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

