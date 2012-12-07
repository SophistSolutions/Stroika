/*
 * Copyright(c) Records For Living, Inc. 2004-2012.  All rights reserved
 */
#include	"Stroika/Foundation/StroikaPreComp.h"

#include	"Stroika/Foundation/Streams/BasicBinaryInputStream.h"

#include	"../TestHarness/TestHarness.h"

using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Streams;





namespace	{
	namespace	BasicBinaryInputStream_ {

		void	TestBasicConstruction_ ()
		{
			{
				BasicBinaryInputStream	s (nullptr, nullptr);
				VerifyTestResult (not s.empty ());
				VerifyTestResult (s.IsSeekable ());
			}
			{
				const char	kData[]	=	"1";
				BasicBinaryInputStream	s (reinterpret_cast<const Byte*> (StartOfArray(kData)), reinterpret_cast<const Byte*> (EndOfArray(kData)));
				VerifyTestResult (not s.empty ());
				VerifyTestResult (s.IsSeekable ());
				Byte	result[100] = { 0 };
				VerifyTestResult (s.Read (StartOfArray (result), EndOfArray (result)) == 2);
				VerifyTestResult (result[0] == '1');
				VerifyTestResult (result[1] == '\0');
			}
		}


		void	Tests_ ()
		{
			TestBasicConstruction_ ();
		}
	}
}







namespace	{
	void	DoRegressionTests_ ()
		{
			BasicBinaryInputStream_::Tests_ ();
		}
}





int main (int argc, const char* argv[])
{
	Stroika::TestHarness::Setup ();
	Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
	return EXIT_SUCCESS;
}
