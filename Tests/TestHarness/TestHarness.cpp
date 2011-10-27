/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#include	"Stroika/Foundation/StroikaPreComp.h"

#include	<iostream>

#include	"Stroika/Foundation/Characters/CodePage.h"
#include	"Stroika/Foundation/Containers/Common.h"
#include	"Stroika/Foundation/Debug/Assertions.h"
#include	"Stroika/Foundation/Debug/Debugger.h"
#include	"Stroika/Foundation/Debug/Fatal.h"
#include	"Stroika/Foundation/Execution/Signals.h"
#include	"Stroika/Foundation/Execution/StringException.h"

#include	"TestHarness.h"


using	namespace	Stroika;
using	namespace	Stroika::Foundation;
using	namespace	Stroika::TestHarness;



namespace	{
	void	_ASSERT_HANDLER_ (const char* assertCategory, const char* assertionText, const char* fileName, int lineNum, const char* functionName)
		{
			if (assertCategory == nullptr) {
				assertCategory = "Unknown assertion";
			}
			if (assertionText == nullptr) {
				assertionText = "";
			}
			if (functionName == nullptr) {
				functionName = "";
			}
			cerr << "FAILED: " << assertCategory << "; " << assertionText << ";" << functionName << ";" << fileName << ": " << lineNum << endl;
			
			Debug::DropIntoDebuggerIfPresent ();

			_exit (EXIT_FAILURE);
		}
	void	_FatalErrorHandler_ (const Characters::TChar* msg)
		{
			#if		qTargetPlatformSDKUseswchar_t
				cerr << "FAILED: " <<  Characters::WideStringToNarrowSDKString (msg) << endl;
			#else
				cerr << "FAILED: " <<  msg << endl;
			#endif
			Debug::DropIntoDebuggerIfPresent ();
			_exit (EXIT_FAILURE);
		}
	void	_FatalSignalHandler_ (Execution::SignalIDType signal)
		{
			cerr << "FAILED: " <<  Characters::WideStringToNarrowSDKString (Execution::SignalToName (signal)) << endl;
			Debug::DropIntoDebuggerIfPresent ();
			_exit (EXIT_FAILURE);
		}
}




void	TestHarness::Setup ()
{
#if		qDebug
	Stroika::Foundation::Debug::SetAssertionHandler (_ASSERT_HANDLER_);
#endif
	Debug::RegisterDefaultFatalErrorHandlers (_FatalErrorHandler_);
	Execution::SignalHandlerRegistry::Get ().SetStandardCrashHandlerSignals (_FatalSignalHandler_);
}


void	TestHarness::PrintPassOrFail (void (*regressionTest) ())
{
	try {
		(*regressionTest) ();
		cout << "Succeeded" << endl;
	}
	catch (const std::exception& e) {
		cerr << "FAILED: REGRESSION TEST (std::exception): '" << e.what () << endl;
		cout << "Failed" << endl;
		Debug::DropIntoDebuggerIfPresent ();
		_exit (EXIT_FAILURE);
	}
	catch (Execution::StringException& e) {
		cerr << "FAILED: REGRESSION TEST (Execution::StringException): '" << Characters::WideStringToNarrowSDKString (e.As<wstring> ()) << endl;
		cout << "Failed" << endl;
		Debug::DropIntoDebuggerIfPresent ();
		_exit (EXIT_FAILURE);
	}
	catch (...) {
		cerr << "FAILED: REGRESSION TEST EXCEPTION" << endl;
		cout << "Failed" << endl;
		Debug::DropIntoDebuggerIfPresent ();
		_exit (EXIT_FAILURE);
	}
}




void	TestHarness::Test_ (bool failIfFalse, const char* regressionTestText, const char* fileName, int lineNum)
{
	if (not failIfFalse) {
		_ASSERT_HANDLER_ ("RegressionTestFailure", regressionTestText, fileName, lineNum, "");
	}
}
