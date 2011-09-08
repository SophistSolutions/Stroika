/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#include	"Stroika/Foundation/StroikaPreComp.h"

#include	<iostream>

#include	"Stroika/Foundation/Debug/Assertions.h"
#include	"Stroika/Foundation/Containers/Common.h"
#include	"Stroika/Foundation/Time/Realtime.h"
#include	"Stroika/Foundation/Characters/String.h"

#include	"../TestHarness/TestHarness.h"


using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Characters;









namespace	{
	unsigned ipow (unsigned n, unsigned toPow)
		{
			// quick hack since pow didnt seem to do what I want - just blindly
			// multiply and dont worry about overflow...
			unsigned result = 1;
			while (toPow-- != 0) {
				result *= n;
			}
			return (result);
		}
}

namespace	{
	namespace	Test2Helpers_ {
		#if qDebug
			//const	int	kLoopEnd = 1000;
			const	int	kLoopEnd = 500;		// 1000 generates stackoverflow (because of String_Catentate stuff) on Windoze - clearly a Stroika bug!!! To use so much stack
		#else
			const	int	kLoopEnd = 2000;
		#endif

		void	StressTest1_ (String big)
			{
				for (int j = 1; j <= kLoopEnd/50; j++) {
					String_ReadOnlyChar	a (L"a");
					for (int i = 0; i <= kLoopEnd; i++) {
						big += a;
						Assert ((big.GetLength () -1) == i);
						Assert (big[i] == 'a');
					}
					big.SetLength (0);
				}

				String	s1	=	L"test strings";
				for (int i = 1; i <= kLoopEnd; i++) {
					big += s1;
					Assert (big.GetLength () == s1.GetLength () * i);
				}
			}
		void	StressTest2_ (String big)
			{
				String	s1	=	L"test strings";
				for (int i = 1; i <= kLoopEnd; i++) {
					big = big + s1;
					Assert (big.GetLength () == s1.GetLength () * i);
			#if 0
					for (int j = 0; j < big.GetLength (); ++j) {
						Character c = big[j];
						int breahere=1;
					}
			#endif
				}
			}
		void	StressTestStrings ()
			{
			#if		qPrintTimings
				cout << tab << "Stress testing strings..." << endl;
				Time t = GetCurrentTime ();
			#endif

				{
					String s (L"");
					StressTest1_ (s);
				}

			#if		qPrintTimings
				t = GetCurrentTime () - t;
				cout << tab << "finished Stress testing strings += ... time elapsed = " << t << endl;
				t = GetCurrentTime ();
			#endif

				{
					String s (L"");
					StressTest2_ (s);
				}

			#if		qPrintTimings
				t = GetCurrentTime () - t;
				cout << tab << "finished Stress testing strings + ... time elapsed = " << t << endl;
			#endif
		}
	void	StressTestBufferedStrings ()
		{
		#if		qPrintTimings
			cout << tab << "Stress testing buffered strings..." << endl;
			Time t = GetCurrentTime ();
		#endif

			{
				String_BufferedCharArray s (L"");
				StressTest1_ (s);
			}

		#if		qPrintTimings
			t = GetCurrentTime () - t;
			cout << tab << "finished stress testing buffered strings  += ... time elapsed = " << t << endl;
			t = GetCurrentTime ();
		#endif

			{
				String_BufferedCharArray s (L"");
				StressTest2_ (s);
			}

		#if		qPrintTimings
			t = GetCurrentTime () - t;
			cout << tab << "finished stress testing buffered strings + ... at " << t << endl;
		#endif
		}
	}
}










namespace	{
	void	Test2_Helper_ (String& s1, String& s2)
		{
			Assert (s1.GetLength () == 12);
			Assert (String (s1).GetLength () == 12);
			Assert (s1 == s2);
			Assert (! (s1 != s2));
			Assert (s1 + s1 == s2 + s2);
			Assert ((s1 + s1).GetLength () == s1.GetLength () *2);
			Assert (s1[2] == 's');
			Assert ('s' == s1[2]);
			Assert (s1.GetLength () == 12);

			String s3;
			s3 += s1;
			s3 += s2;

			s1 += L"\n";
			Assert (s1.GetLength () == 13);
		}

	void	Test1_ ()
		{
			/*
			 * Some simple tests to start off with.
			 */
			{
				Assert (String (String (L"fred") + String (L"joe")).GetLength () == 7);

				Assert (String (L"fred") + String (L"joe") == String (L"fredjoe"));
				{
					String	s1	=	String_CharArray (L"test strings");
					String	s2	=	String_CharArray (L"test strings");
					Test2_Helper_ (s1, s2);
				}

				{
					String_BufferedCharArray s1 (L"test strings");
					String_BufferedCharArray s2 (L"test strings");

					Assert (Character ('a') == 'a');

					Test2_Helper_ (s1, s2);
				}

				{
					String	s1	=	String_ReadOnlyChar (L"test strings");
					String	s2	=	String_ReadOnlyChar (L"test strings");
					Test2_Helper_ (s1, s2);
				}
			}
		}

	void	Test2_ ()
		{
			Test2Helpers_::StressTestStrings ();
			Test2Helpers_::StressTestBufferedStrings ();
		}

	void	Test3_ ()
		{
			String	t1;
			String	t2	=	t1;
			String	t3	=	L"a";
			String	t4	=	L"a";

			Assert (t1 == L"");
			Assert (t1 == String ());
			Assert (t1 == String (L""));
			Assert (t1 == t2);
			Assert (t3 == L"a");
			Assert (t3 == String(L"a"));
			Assert (t4 == L"a");
			t1 = t1;
			Assert (t1 == L"");

			t1 += 'F';
			t1 += 'r';
			t1 += 'e';
			t1 += 'd';
			t1 += L" Flintstone";
			Assert (t1 == L"Fred Flintstone");
			Assert (L"Fred Flintstone" == t1);
			Assert (String (L"Fred Flintstone") == t1);
			Assert (t1 == String (L"Fred Flintstone"));
			Assert (t2 != L"Fred Flintstone");
			Assert (L"Fred Flintstone" != t2);
			Assert (String (L"Fred Flintstone") != t2);
			Assert (t2 != String (L"Fred Flintstone"));

			Assert (t1.GetLength () == 15);
			t1.SetLength (20);
			Assert (t1.GetLength () == 20);
			t1.SetLength (4);
			Assert (t1.GetLength () == 4);
			Assert (t1 == L"Fred");

			Assert (t1[0] == 'F');
			Assert (t1[1] == 'r');
			Assert (t1[2] == 'e');
			Assert (t1[3] == 'd');

			Assert (t1[0] == 'F');
			Assert (t1[1] == 'r');
			Assert (t1[2] == 'e');
			Assert (t1[3] == 'd');

			String	a[10];
			Assert (a[2] == L"");
			a [3] = L"Fred";
			Assert (a[3] == L"Fred");
			Assert (a[2] != L"Fred");
		}

	void	Test4_ ()
		{
			const	wchar_t	frobaz[]	=	L"abc";

			String	t1;
			String	t3	=	L"a";
			String	t5	=	String (frobaz);
			String*	t6	=	new String (L"xyz");
			delete (t6);

			t5 = t1;
			t1 = t5;
			t1 = t1;
			Assert (t1 == L"");
			Assert (t5 == L"");

			t1 += 'F';
			t1 += 'r';
			t1 += 'e';
			t1 += 'd';
			t1 += L" Flintstone";
			Assert (t1 == L"Fred Flintstone");
			t5 = t1.SubString (5, 10);
			Assert (t5 == L"Flintstone")

			t1.SetLength (20);
			t1.SetLength (4);

			t5 = t1;
			t5.SetCharAt ('f', 0);
			t5.SetCharAt ('R', 1);
			t5.SetCharAt ('E', 2);
			t5.SetCharAt ('D', 3);

			Assert (t5[0] == 'f');
			Assert (t5[1] == 'R');
			Assert (t5[2] == 'E');
			Assert (t5[3] == 'D');
			Assert (t5.IndexOf ('f') == 0);
			Assert (t5.IndexOf (L"f") == 0);
			Assert (t5.IndexOf (L"fR") == 0);
			Assert (t5.IndexOf (L"fRE") == 0);
			Assert (t5.IndexOf (L"fRED") == 0);
			Assert (t5.IndexOf (L"fRD") == kBadStringIndex);
			Assert (t5.IndexOf ('R') == 1);
			Assert (t5.IndexOf ('E') == 2);
			Assert (t5.IndexOf ('D') == 3);
			Assert (t5.IndexOf (L"D") == 3);

			Assert (t5.RIndexOf ('f') == 0);
			Assert (t5.RIndexOf ('R') == 1);
			Assert (t5.RIndexOf ('E') == 2);
			Assert (t5.RIndexOf ('D') == 3);
			Assert (t5.RIndexOf (L"D") == 3);
			Assert (t5.RIndexOf (L"ED") == 2);
			Assert (t5.RIndexOf (L"RED") == 1);
			Assert (t5.RIndexOf (L"fRED") == 0);
			Assert (t5.RIndexOf (L"fr") == kBadStringIndex);
			Assert (t5.RIndexOf (L"f") == 0);

			t5.SetCharAt ('D', 0);
			t5.SetCharAt ('D', 1);
			t5.SetCharAt ('D', 2);
			t5.SetCharAt ('D', 3);
			Assert (t5.IndexOf ('D') == 0);
			Assert (t5.IndexOf (L"D") == 0);
			Assert (t5.RIndexOf ('D') == 3);
			Assert (t5.RIndexOf (L"D") == 3);

			Assert (t5.IndexOf ('f') == kBadStringIndex);
			Assert (t5.IndexOf (L"f") == kBadStringIndex);
			Assert (t5.RIndexOf ('f') == kBadStringIndex);
			Assert (t5.RIndexOf (L"f") == kBadStringIndex);

			Assert ((t5.Peek ())[0] == 'D');
			Assert ((t5.Peek ())[1] == 'D');
			Assert ((t5.Peek ())[2] == 'D');
			Assert ((t5.Peek ())[3] == 'D');
		}

	void	Test5_ ()
		{
			String arr [100];
			arr [3] = L"fred";
			Assert (arr[3] == L"fred");
			String*	l	=	new String [100];
			l[3] = L"FRED";
			Assert (l[3] == L"FRED");
			Assert (l[99] == L"");
			delete[] (l);
			size_t nSlots = 100;
			l	=	new String [size_t (nSlots)];
			delete[] (l);
		}

	String	Test6_ (const String& a, int depth)
		{
			String	b = a;
			b += a;
			if (depth > 0) {
				b = Test6_ (b, depth-1) + Test6_ (b, depth-1);
			}
			return (b);
		}

	void	Test7_ ()
		{
			Assert (String (L"1") <= String (L"1"));
			Assert (String (L"1") <= String (L"10"));
			Assert (not (String (L"1") > String (L"10")));
			Assert (not (String (L"1") >= String (L"10")));
			Assert (String (L"1") < String (L"10"));

			Assert (String (L"20") > String (L"11"));
			Assert (String (L"20") >= String (L"11"));
			Assert (not (String (L"20") < String (L"11")));
			Assert (not (String (L"20") <= String (L"11")));
			Assert (String (L"11") < String (L"20"));
			Assert (String (L"11") <= String (L"20"));
			Assert (not (String (L"11") > String (L"20")));
			Assert (not (String (L"11") >= String (L"20")));

			Assert (String (L"aac") > String (L"aab"));
			Assert (String (L"aac") >= String (L"aab"));
			Assert (not (String (L"aac") < String (L"aab")));
			Assert (not (String (L"aac") <= String (L"aab")));

			Assert (String (L"apple") < String (L"apples"));
			Assert (String (L"apple") <= String (L"apples"));
			Assert (not (String (L"apple") > String (L"apples")));
			Assert (not (String (L"apple") >= String (L"apples")));
		}

	void    Test8_ReadOnlyStrings_ ()
		{
			String_ReadOnlyChar s (L"fred");
			Assert (s[0] == 'f');
			s.SetLength (3);
			Assert (s[0] == 'f');
			Assert (s.GetLength () == 3);
			s += L"x";
			Assert (s.GetLength () == 4);
			Assert (s[3] == 'x');
		}

	namespace	{
		namespace	Test9Support {
			template	<typename	STRING>
				void	DoTest1 (STRING s)
					{
						STRING	t1	=	s;
						for (size_t i = 0; i < 100; ++i) {
							t1 += L"X";
						}
						STRING	t2	=	t1;
						if (t1 != t2) {
							Assert (false);
						}
					}
		}
	}
	void	Test9_StringVersusStdCString_ ()
		{
			// EMBELLISH THIS MORE ONCE WE HAVE TIMING SUPPORT WORKING - SO WE CNA COMPARE PERFORMANCE - AND COME UP WITH MORE REASONABLE TESTS
			//
			//		-- LGP 2011-09-01
			Test9Support::DoTest1<String> (L"Hello");
			Test9Support::DoTest1<std::wstring> (L"Hello");
		}
	void	Test10_ConvertToFromSTDStrings_ ()
		{
			const	wstring	kT1	=	L"abcdefh124123985213129314234";
			String	t1	=	kT1;
			Assert (t1.As<wstring> () == kT1);
			Assert (t1 == kT1);
		}
}


namespace	{
	#if		!qCompilerAndStdLib_Supports_lambda_default_argument || !qCompilerAndStdLib_lamba_closureCvtToFunctionPtrSupported
	static	bool	Test11_TRIM_ISALPHA (Character c)				{		return c.IsAlphabetic ();		}
	#endif
	void	Test11_Trim_ ()
		{
			const	String	kT1	=	L"  abc";
			Assert (kT1.RTrim () == kT1);
			Assert (kT1.LTrim () == kT1.Trim ());
			Assert (kT1.Trim () == L"abc");

			#if		qCompilerAndStdLib_lamba_closureCvtToFunctionPtrSupported
				Assert (kT1.Trim ([] (Character c) -> bool { return c.IsAlphabetic (); }) == L"  ");
			#else
				Assert (kT1.Trim (Test11_TRIM_ISALPHA) == L"  ");
			#endif
		}
}











namespace	{

	void	DoRegressionTests_ ()
		{
			Test1_ ();
			Test2_ ();
			Test3_ ();
			Test4_ ();
			Test5_ ();

			const	int	kRecurseDepth = 6;
			String	testString = L"some dump test";

		#if		qPrintTimings
			cout << '\t' << "Recursive build test with depth " << kRecurseDepth << " for " << testString << endl;
			Time t = GetCurrentTime ();
		#endif

			String s = Test6_ (testString, kRecurseDepth);	// returns length 114688 for depth 6
			Assert (s.GetLength () ==  (ipow (4,kRecurseDepth) * 2 * testString.GetLength ()));

		#if		qPrintTimings
			t = GetCurrentTime () - t;
			cout << tab << "finished Recursive build test. Time elapsed = " << t << " length = " << s.GetLength () << endl;
		#endif

			Test7_ ();
			Test8_ReadOnlyStrings_ ();
			Test9_StringVersusStdCString_ ();
			Test10_ConvertToFromSTDStrings_ ();
			Test11_Trim_ ();
		}
}





int		main (int argc, const char* argv[])
{
	Stroika::TestHarness::Setup ();
	Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
	return EXIT_SUCCESS;
}

