/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#include	"Stroika/Foundation/StroikaPreComp.h"

#include	<iostream>
#include	<sstream>

#include	"Stroika/Foundation/Debug/Assertions.h"
#include	"Stroika/Foundation/Debug/Trace.h"
#include	"Stroika/Foundation/Time/Date.h"
#include	"Stroika/Foundation/Time/DateTime.h"
#include	"Stroika/Foundation/Time/Duration.h"
#include	"Stroika/Foundation/Time/Realtime.h"

#include	"../TestHarness/TestHarness.h"


using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Characters;

using	Stroika::Foundation::Debug::TraceContextBumper;




static	void	SimpleTimeTests ();
static	void	TestTime ();

void	SimpleTimeTests ()
{
#if 0
Real	x = 0.001;
x *= 1000000.0;
TestCondition (x == 1000.0);
TestCondition ((0.001 * 1000000.0) == 1000.0);
TestCondition ((0.01 * 1000000.0) == 10000.0);
TestCondition ((0.1 * 1000000.0) == 100000.0);
TestCondition ((0.0001 * 1000000.0) == 100.0);
TestCondition ((0.00001 * 1000000.0) == 10.0);
TestCondition ((0.000001 * 1000000.0) == 1.0);

TestCondition ((0.002 * 1000000.0) == 2000.0);
TestCondition ((0.003 * 1000000.0) == 3000.0);
TestCondition ((0.004 * 1000000.0) == 4000.0);
TestCondition ((0.005 * 1000000.0) == 5000.0);
TestCondition ((0.006 * 1000000.0) == 6000.0);
TestCondition ((0.007 * 1000000.0) == 7000.0);
TestCondition ((0.008 * 1000000.0) == 8000.0);
TestCondition ((0.009 * 1000000.0) == 9000.0);
TestCondition ((0.0010 * 1000000.0) == 1000.0);
TestCondition ((0.0011 * 1000000.0) == 1100.0);

TestCondition ((0.000001 * 100000.0) == 	.1);
TestCondition ((0.000001 * 10000.0) ==  	.01);
TestCondition ((0.000001 * 1000.0) == 		.001);
TestCondition ((0.000001 * 100.0) == 		.0001);
TestCondition ((0.000001 * 10.0) == 		.00001);
TestCondition ((0.000001 * 1.0) == 			.000001); 
TestCondition ((0.000001 * .1) == 			.0000001);

TestCondition ((0.0015 * 1000000.0) == 1500.0);
TestCondition ((0.000999 * 1000000.0) == 999.0);
#endif

	//VerifyTestResult (Time () == 0);
#if 0
//	TestCondition ((kOneMicroSecond * 1000) == kOneMilliSecond);
Time f = Time::kOneMicroSecond * 1000;
//TestCondition (f == kOneMilliSecond);
//cerr << "f = " << f.fSeconds << endl;
//cerr << "kOneMilliSecond = " << kOneMilliSecond.fSeconds << endl;
//	TestCondition ((kOneMilliSecond * 1000) == kOneSecond);
f = Time::kOneMilliSecond * 1000;
//TestCondition (f == kOneSecond);
	TestCondition ((Time::kOneMicroSecond * 1000000) == Time::kOneSecond);
f = Time::kOneMicroSecond * 1000000;
TestCondition (f == Time::kOneSecond);
	
//	cerr << "kOneMicroSecond = " << kOneMicroSecond.fSeconds << endl;
//	cerr << "kOneMicroSecond * 1000 = " << (kOneMicroSecond * 1000).fSeconds << endl;
//	cerr << "kOneMilliSecond = " << kOneMilliSecond.fSeconds << endl;
//	cerr << "kOneMilliSecond * 1000 = " << (kOneMilliSecond * 1000).fSeconds << endl;

	Time t = Time::kOneSecond - Time::kOneMicroSecond;
	TestCondition (t < Time::kOneSecond);
	TestCondition (t <= Time::kOneSecond);
//	t += kOneMicroSecond;
t = t + Time::kOneMicroSecond;
	TestCondition (t <= Time::kOneSecond);
	TestCondition (t == Time::kOneSecond);
	TestCondition (t >= Time::kOneSecond);
//	t += kOneMicroSecond;
t = t + Time::kOneMicroSecond;
	TestCondition (t > Time::kOneSecond);
	TestCondition (t >= Time::kOneSecond);
t = t - Time::kOneMicroSecond;
	TestCondition (t == Time::kOneSecond);
	t = t / 5;
	TestCondition (floor (t.GetSeconds ()) == 0);
//	TestCondition (t.fSeconds == 200000);
#endif
}

void	TestTime ()
{
	SimpleTimeTests ();
	
#if 0
	Time t = GetCurrentTime ();
//	cerr << "current time = " << t.GetSeconds () << ", " << t.fSeconds << endl;
	Time t1 = GetCurrentTime ();
	Assert (t <= t1);
#endif
}


namespace	{

	void	DoRegressionTests_ ()
		{
			TestTime ();
		}
}





int		main (int argc, const char* argv[])
{
	Stroika::TestHarness::Setup ();
	Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
	return EXIT_SUCCESS;
}

