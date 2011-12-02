/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#include	"Stroika/Foundation/StroikaPreComp.h"

#include	<iostream>

#include	"Stroika/Foundation/Characters/Format.h"
#include	"Stroika/Foundation/Characters/String.h"
#include	"Stroika/Foundation/Containers/Common.h"
#include	"Stroika/Foundation/Debug/Assertions.h"
#include	"Stroika/Foundation/Memory/SmallStackBuffer.h"
#include	"Stroika/Foundation/Time/Realtime.h"

#include	"../TestHarness/TestHarness.h"


using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Characters;


/*
 * TODO:
 *
 *
 *		(o)	Must write ASAP a performance comparison suite for String class as part of string automated test.
 *			Print a table of results (X/Y – wstring versus String (maybe versus other string types?)).
 *			Compare inserting into start of vector<STRINGTYPE> - to test copying.
 */







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
			const	int	kLoopEnd = 1000;
		#else
			const	int	kLoopEnd = 2000;
		#endif

		void	StressTest1_ (String big)
			{
				for (int j = 1; j <= kLoopEnd/50; j++) {
					String_ExternalMemoryOwnership	a (L"a");
					for (int i = 0; i <= kLoopEnd; i++) {
						big += a;
						VerifyTestResult ((big.GetLength () -1) == i);
						VerifyTestResult (big[i] == 'a');
					}
					big.SetLength (0);
				}

				String	s1	=	L"test strings";
				for (int i = 1; i <= kLoopEnd; i++) {
					big += s1;
					VerifyTestResult (big.GetLength () == s1.GetLength () * i);
				}
			}
		void	StressTest2_ (String big)
			{
				String	s1	=	L"test strings";
				for (int i = 1; i <= kLoopEnd; i++) {
					big = big + s1;
					VerifyTestResult (big.GetLength () == s1.GetLength () * i);
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
			VerifyTestResult (s1.GetLength () == 12);
			VerifyTestResult (String (s1).GetLength () == 12);
			VerifyTestResult (s1 == s2);
			VerifyTestResult (! (s1 != s2));
			VerifyTestResult (s1 + s1 == s2 + s2);
			VerifyTestResult ((s1 + s1).GetLength () == s1.GetLength () *2);
			VerifyTestResult (s1[2] == 's');
			VerifyTestResult ('s' == s1[2]);
			VerifyTestResult (s1.GetLength () == 12);

			String s3;
			s3 += s1;
			s3 += s2;

			s1 += L"\n";
			VerifyTestResult (s1.GetLength () == 13);
		}

	void	Test1_ ()
		{
			/*
			 * Some simple tests to start off with.
			 */
			{
				VerifyTestResult (String (String (L"fred") + String (L"joe")).GetLength () == 7);

				VerifyTestResult (String (L"fred") + String (L"joe") == String (L"fredjoe"));
				{
					String	s1	=	String_CharArray (L"test strings");
					String	s2	=	String_CharArray (L"test strings");
					Test2_Helper_ (s1, s2);
				}

				{
					String_BufferedCharArray s1 (L"test strings");
					String_BufferedCharArray s2 (L"test strings");

					VerifyTestResult (Character ('a') == 'a');

					Test2_Helper_ (s1, s2);
				}

				{
					String	s1	=	String_ExternalMemoryOwnership (L"test strings");
					String	s2	=	String_ExternalMemoryOwnership (L"test strings");
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

			VerifyTestResult (t1 == L"");
			VerifyTestResult (t1 == String ());
			VerifyTestResult (t1 == String (L""));
			VerifyTestResult (t1 == t2);
			VerifyTestResult (t3 == L"a");
			VerifyTestResult (t3 == String(L"a"));
			VerifyTestResult (t4 == L"a");
			t1 = t1;
			VerifyTestResult (t1 == L"");

			t1 += 'F';
			t1 += 'r';
			t1 += 'e';
			t1 += 'd';
			t1 += L" Flintstone";
			VerifyTestResult (t1 == L"Fred Flintstone");
			VerifyTestResult (L"Fred Flintstone" == t1);
			VerifyTestResult (String (L"Fred Flintstone") == t1);
			VerifyTestResult (t1 == String (L"Fred Flintstone"));
			VerifyTestResult (t2 != L"Fred Flintstone");
			VerifyTestResult (L"Fred Flintstone" != t2);
			VerifyTestResult (String (L"Fred Flintstone") != t2);
			VerifyTestResult (t2 != String (L"Fred Flintstone"));

			VerifyTestResult (t1.GetLength () == 15);
			t1.SetLength (20);
			VerifyTestResult (t1.GetLength () == 20);
			t1.SetLength (4);
			VerifyTestResult (t1.GetLength () == 4);
			VerifyTestResult (t1 == L"Fred");

			VerifyTestResult (t1[0] == 'F');
			VerifyTestResult (t1[1] == 'r');
			VerifyTestResult (t1[2] == 'e');
			VerifyTestResult (t1[3] == 'd');

			VerifyTestResult (t1[0] == 'F');
			VerifyTestResult (t1[1] == 'r');
			VerifyTestResult (t1[2] == 'e');
			VerifyTestResult (t1[3] == 'd');

			String	a[10];
			VerifyTestResult (a[2] == L"");
			a [3] = L"Fred";
			VerifyTestResult (a[3] == L"Fred");
			VerifyTestResult (a[2] != L"Fred");
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
			VerifyTestResult (t1 == L"");
			VerifyTestResult (t5 == L"");

			t1 += 'F';
			t1 += 'r';
			t1 += 'e';
			t1 += 'd';
			t1 += L" Flintstone";
			VerifyTestResult (t1 == L"Fred Flintstone");
			t5 = t1.SubString (5, 5 + 10);
			VerifyTestResult (t5 == L"Flintstone");

			t1.SetLength (20);
			t1.SetLength (4);

			t5 = t1;
			t5.SetCharAt ('f', 0);
			t5.SetCharAt ('R', 1);
			t5.SetCharAt ('E', 2);
			t5.SetCharAt ('D', 3);

			VerifyTestResult (t5[0] == 'f');
			VerifyTestResult (t5[1] == 'R');
			VerifyTestResult (t5[2] == 'E');
			VerifyTestResult (t5[3] == 'D');
			VerifyTestResult (t5.IndexOf ('f') == 0);
			VerifyTestResult (t5.IndexOf (L"f") == 0);
			VerifyTestResult (t5.IndexOf (L"fR") == 0);
			VerifyTestResult (t5.IndexOf (L"fRE") == 0);
			VerifyTestResult (t5.IndexOf (L"fRED") == 0);
			VerifyTestResult (t5.IndexOf (L"fRD") == kBadStringIndex);
			VerifyTestResult (t5.IndexOf ('R') == 1);
			VerifyTestResult (t5.IndexOf ('E') == 2);
			VerifyTestResult (t5.IndexOf ('D') == 3);
			VerifyTestResult (t5.IndexOf (L"D") == 3);

			VerifyTestResult (t5.RIndexOf ('f') == 0);
			VerifyTestResult (t5.RIndexOf ('R') == 1);
			VerifyTestResult (t5.RIndexOf ('E') == 2);
			VerifyTestResult (t5.RIndexOf ('D') == 3);
			VerifyTestResult (t5.RIndexOf (L"D") == 3);
			VerifyTestResult (t5.RIndexOf (L"ED") == 2);
			VerifyTestResult (t5.RIndexOf (L"RED") == 1);
			VerifyTestResult (t5.RIndexOf (L"fRED") == 0);
			VerifyTestResult (t5.RIndexOf (L"fr") == kBadStringIndex);
			VerifyTestResult (t5.RIndexOf (L"f") == 0);

			t5.SetCharAt ('D', 0);
			t5.SetCharAt ('D', 1);
			t5.SetCharAt ('D', 2);
			t5.SetCharAt ('D', 3);
			VerifyTestResult (t5.IndexOf ('D') == 0);
			VerifyTestResult (t5.IndexOf (L"D") == 0);
			VerifyTestResult (t5.RIndexOf ('D') == 3);
			VerifyTestResult (t5.RIndexOf (L"D") == 3);

			VerifyTestResult (t5.IndexOf ('f') == kBadStringIndex);
			VerifyTestResult (t5.IndexOf (L"f") == kBadStringIndex);
			VerifyTestResult (t5.RIndexOf ('f') == kBadStringIndex);
			VerifyTestResult (t5.RIndexOf (L"f") == kBadStringIndex);

			VerifyTestResult (t5[0] == 'D');
			VerifyTestResult (t5[1] == 'D');
			VerifyTestResult (t5[2] == 'D');
			VerifyTestResult (t5[3] == 'D');
		}

	void	Test5_ ()
		{
			String arr [100];
			arr [3] = L"fred";
			VerifyTestResult (arr[3] == L"fred");
			String*	l	=	new String [100];
			l[3] = L"FRED";
			VerifyTestResult (l[3] == L"FRED");
			VerifyTestResult (l[99] == L"");
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
			VerifyTestResult (String (L"1") <= String (L"1"));
			VerifyTestResult (String (L"1") <= String (L"10"));
			VerifyTestResult (not (String (L"1") > String (L"10")));
			VerifyTestResult (not (String (L"1") >= String (L"10")));
			VerifyTestResult (String (L"1") < String (L"10"));

			VerifyTestResult (String (L"20") > String (L"11"));
			VerifyTestResult (String (L"20") >= String (L"11"));
			VerifyTestResult (not (String (L"20") < String (L"11")));
			VerifyTestResult (not (String (L"20") <= String (L"11")));
			VerifyTestResult (String (L"11") < String (L"20"));
			VerifyTestResult (String (L"11") <= String (L"20"));
			VerifyTestResult (not (String (L"11") > String (L"20")));
			VerifyTestResult (not (String (L"11") >= String (L"20")));

			VerifyTestResult (String (L"aac") > String (L"aab"));
			VerifyTestResult (String (L"aac") >= String (L"aab"));
			VerifyTestResult (not (String (L"aac") < String (L"aab")));
			VerifyTestResult (not (String (L"aac") <= String (L"aab")));

			VerifyTestResult (String (L"apple") < String (L"apples"));
			VerifyTestResult (String (L"apple") <= String (L"apples"));
			VerifyTestResult (not (String (L"apple") > String (L"apples")));
			VerifyTestResult (not (String (L"apple") >= String (L"apples")));
		}

	void    Test8_ReadOnlyStrings_ ()
		{
			String_ExternalMemoryOwnership s (L"fred");
			VerifyTestResult (s[0] == 'f');
			s.SetLength (3);
			VerifyTestResult (s[0] == 'f');
			VerifyTestResult (s.GetLength () == 3);
			s += L"x";
			VerifyTestResult (s.GetLength () == 4);
			VerifyTestResult (s[3] == 'x');
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
							VerifyTestResult (false);
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
			VerifyTestResult (t1.As<wstring> () == kT1);
			VerifyTestResult (t1 == kT1);
		}
}


namespace	{
	#if		!qCompilerAndStdLib_Supports_lambda_default_argument || !qCompilerAndStdLib_lamba_closureCvtToFunctionPtrSupported
	static	bool	Test11_TRIM_ISALPHA (Character c)				{		return c.IsAlphabetic ();		}
	#endif
	void	Test11_Trim_ ()
		{
			const	String	kT1	=	L"  abc";
			VerifyTestResult (kT1.RTrim () == kT1);
			VerifyTestResult (kT1.LTrim () == kT1.Trim ());
			VerifyTestResult (kT1.Trim () == L"abc");

			#if		qCompilerAndStdLib_lamba_closureCvtToFunctionPtrSupported
				VerifyTestResult (kT1.Trim ([] (Character c) -> bool { return c.IsAlphabetic (); }) == L"  ");
			#else
				VerifyTestResult (kT1.Trim (Test11_TRIM_ISALPHA) == L"  ");
			#endif
		}
}



namespace	{
	void	Test12_CodePageConverter_ ()
		{
			wstring w = L"<PHRMode";
			using namespace Characters;
			using namespace Memory;
			CodePageConverter	cpc (kCodePage_UTF8, CodePageConverter::eHandleBOM);
			size_t				sz	=		cpc.MapFromUNICODE_QuickComputeOutBufSize (w.c_str (), w.length ());
			SmallStackBuffer<char>	buf (sz + 1);
			size_t	charCnt	=	sz;
			cpc.MapFromUNICODE (w.c_str (), w.length (), buf, &charCnt);
			VerifyTestResult (string (buf.begin (), buf.begin () + charCnt) == "﻿<PHRMode");
		}
}





namespace	{
	void	Test13_ToLowerUpper_ ()
		{
			String w = L"Lewis";
			VerifyTestResult (w.ToLowerCase () == L"lewis");
			VerifyTestResult (w.ToUpperCase () == L"LEWIS");
			VerifyTestResult (w == L"Lewis");
		}
}



namespace	{
	void    Test14_String_StackLifetime_ ()
		{
			wchar_t	buf[1024]	=	L"fred";
			{
				String_StackLifetime s (buf);
				VerifyTestResult (s[0] == 'f');
				s.SetLength (3);
				VerifyTestResult (s[0] == 'f');
				VerifyTestResult (s.GetLength () == 3);
				s += L"x";
				VerifyTestResult (s.GetLength () == 4);
				VerifyTestResult (s[3] == 'x');
			}
			VerifyTestResult (::wcscmp (buf, L"fred") == 0);
		}

}




namespace	{
	#if		!qCompilerAndStdLib_lamba_closureCvtToFunctionPtrSupported
	static	bool	Test15_STRIPALLTEST_ (Character c)				{		return c.IsWhitespace ();		}
	#endif
	void	Test15_StripAll_ ()
		{
			String w = L"Le wis";
			#if		qCompilerAndStdLib_lamba_closureCvtToFunctionPtrSupported
				VerifyTestResult (w.StripAll ([](Character c) -> bool { return c.IsWhitespace (); }) == L"Lewis");
			#else
				VerifyTestResult (w.StripAll (Test15_STRIPALLTEST_) == L"Lewis");
			#endif

			w = L"This is a very good test    ";
			#if		qCompilerAndStdLib_lamba_closureCvtToFunctionPtrSupported
				VerifyTestResult (w.StripAll ([](Character c) -> bool { return c.IsWhitespace (); }) == L"Thisisaverygoodtest");
			#else
				VerifyTestResult (w.StripAll (Test15_STRIPALLTEST_) == L"Thisisaverygoodtest");
			#endif
		}
}




namespace	{
	void	Test16_Format_ ()
		{
			VerifyTestResult (Format ("%d", 123) == "123");
			VerifyTestResult (Format ("%s", "123") == "123");

			// SUBTLE - this would FAIL with vsnprintf on gcc -- See docs in Header for
			// Format string
			VerifyTestResult (Format (L"%s", L"123") == L"123");

			VerifyTestResult (Format (L"%20s", L"123") == L"                 123");
			VerifyTestResult (Format (L"%.20s", L"123") == L"123");

			for (int i = 1; i < 1000; ++i) {
				String	format	=	Format (L"%%%ds", i);
				VerifyTestResult (Format (format.As<wstring> ().c_str (), L"x").length () == i);
			}
		}
}





namespace	{
	void	Test17_RegExp_ ()
		{
		#if		!qCompilerAndStdLib_Bug_regexpr
			VerifyTestResult (not String (L"Hello world").Match (L"ello"));
			VerifyTestResult (String (L"Hello world").Find (L"ello").size () == 1);
			vector<String>	r	=	String (L"<h2>Egg prices</h2>").Find (L"<h(.)>([^<]+)");
			VerifyTestResult (r.size () == 3 and r[1] == L"2" and r[2] == L"Egg prices");
			VerifyTestResult (String (L"Hello world").Replace (L"world", L"Planet") == L"Hello Planet");
		#endif
		}
}





namespace	{
	void	Test18_Compare_ ()
		{
			const	String	kHELLOWorld	=	String (L"Hello world");
			VerifyTestResult (kHELLOWorld.Compare (kHELLOWorld, String::eWithCase_CO) == 0);
			VerifyTestResult (kHELLOWorld.Compare (String (L"Hello world"), String::eWithCase_CO) == 0);

			VerifyTestResult (kHELLOWorld.Compare (kHELLOWorld.ToLowerCase (), String::eWithCase_CO) < 0);
			VerifyTestResult (kHELLOWorld.Compare (kHELLOWorld.ToLowerCase (), String::eCaseInsensitive_CO) == 0);
			VerifyTestResult (String (L"fred").Compare (L"fredy", String::eCaseInsensitive_CO) < 0);
			VerifyTestResult (String (L"fred").Compare (L"Fredy", String::eCaseInsensitive_CO) < 0);
			VerifyTestResult (String (L"Fred").Compare (L"fredy", String::eCaseInsensitive_CO) < 0);
			VerifyTestResult (String (L"fred").Compare (L"fredy", String::eWithCase_CO) < 0);
			VerifyTestResult (String (L"fred").Compare (L"Fredy", String::eWithCase_CO) > 0);
			VerifyTestResult (String (L"Fred").Compare (L"fredy", String::eWithCase_CO) < 0);
		}
}





namespace	{
	void	Test19_ConstCharStar_ ()
		{
			VerifyTestResult (wcscmp (String (L"fred").c_str (), L"fred") == 0);
			{
				String	tmp	=	L"333";
				VerifyTestResult (wcscmp (tmp.c_str (), L"333") == 0);
				tmp = L"Barny";
				VerifyTestResult (wcscmp (tmp.c_str (), L"Barny") == 0);
				tmp.SetCharAt ('c', 2);
				VerifyTestResult (wcscmp (tmp.c_str (), L"Bacny") == 0);
				String	bumpRefCnt	=	tmp;
				tmp.SetCharAt ('d', 2);
				VerifyTestResult (wcscmp (tmp.c_str (), L"Badny") == 0);
			}
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
			VerifyTestResult (s.GetLength () ==  (ipow (4,kRecurseDepth) * 2 * testString.GetLength ()));

		#if		qPrintTimings
			t = GetCurrentTime () - t;
			cout << tab << "finished Recursive build test. Time elapsed = " << t << " length = " << s.GetLength () << endl;
		#endif

			Test7_ ();
			Test8_ReadOnlyStrings_ ();
			Test9_StringVersusStdCString_ ();
			Test10_ConvertToFromSTDStrings_ ();
			Test11_Trim_ ();
			Test12_CodePageConverter_ ();
			Test13_ToLowerUpper_ ();
			Test14_String_StackLifetime_ ();
			Test15_StripAll_ ();
			Test16_Format_ ();
			Test17_RegExp_ ();
			Test18_Compare_ ();
			Test19_ConstCharStar_ ();
		}
}




#if qOnlyOneMain
extern  int TestStrings ()
#else
int main (int argc, const char* argv[])
#endif
{
	Stroika::TestHarness::Setup ();
	Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
	return EXIT_SUCCESS;
}

