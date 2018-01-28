/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
//  TEST    Foundation::Execution::Other
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/Execution/Finally.h"
#include "Stroika/Foundation/Execution/Function.h"
#include "Stroika/Foundation/Execution/StaticConstantMaker.h"

#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;

namespace {
    void Test1_Function_ ()
    {
        // Make sure Function<> works as well as std::function
        {
            Function<int(bool)> f = [](bool b) -> int { return 3; };
            VerifyTestResult (f (true) == 3);
            function<int(bool)> ff = f;
            VerifyTestResult (ff (true) == 3);
        }
        // Make sure Function<> serves its one purpose - being comparable
        {
            Function<int(bool)> f1 = [](bool b) -> int { return 3; };
            Function<int(bool)> f2 = [](bool b) -> int { return 3; };

            VerifyTestResult (f1 != f2);
            VerifyTestResult (f1 < f2 or f2 < f1);
            Function<int(bool)> f3 = f1;
            VerifyTestResult (f3 == f1);
            VerifyTestResult (f3 != f2);
        }
    }
}

namespace {
    void Test2_CommandLine_ ()
    {
        {
            String           cmdLine = L"/bin/sh -c \"a b c\"";
            Sequence<String> l       = ParseCommandLine (cmdLine);
            VerifyTestResult (l.size () == 3);
            VerifyTestResult (l[0] == L"/bin/sh");
            VerifyTestResult (l[1] == L"-c");
            VerifyTestResult (l[2] == L"a b c");
        }
        {
            String           cmdLine = L"";
            Sequence<String> l       = ParseCommandLine (cmdLine);
            VerifyTestResult (l.size () == 0);
        }
        {
            String           cmdLine = L"/bin/sh -c \'a b c\'";
            Sequence<String> l       = ParseCommandLine (cmdLine);
            VerifyTestResult (l.size () == 3);
            VerifyTestResult (l[0] == L"/bin/sh");
            VerifyTestResult (l[1] == L"-c");
            VerifyTestResult (l[2] == L"a b c");
        }
        {
            String           cmdLine = L"/bin/sh\t b c     -d";
            Sequence<String> l       = ParseCommandLine (cmdLine);
            VerifyTestResult (l.size () == 4);
            VerifyTestResult (l[0] == L"/bin/sh");
            VerifyTestResult (l[1] == L"b");
            VerifyTestResult (l[2] == L"c");
            VerifyTestResult (l[3] == L"-d");
        }
    }
}

namespace {
    namespace Test3_ {
        void DoAll ()
        {
            {
                unsigned int cnt = 0;
                {
                    auto&& c = Finally (
                        [&cnt]() noexcept {
                            cnt--;
                        });
                    cnt++;
                }
                VerifyTestResult (cnt == 0);
            }
        }
    }
}


namespace {
	namespace Test4_StaticConstantMaker_ {
		namespace Private_ {
			namespace T1_ {
				static const int x{ 3 };
				inline const int& kX_ () { return x; }
				const Execution::StaticConstantMaker<int, &kX_> kX;
				void DoIt ()
				{
					const int a = kX;
				}
			}
			namespace T2_ {
				inline const int& kX_ () { 
					static const int x{ 6 };
					return x; 
				}
				const Execution::StaticConstantMaker<int, &kX_> kX;
				void DoIt ()
				{
					const int a = kX;
				}
			}
		}
		void DoAll ()
		{
			Private_::T1_::DoIt ();
			Private_::T2_::DoIt ();
		}
	}
}

namespace {
    void DoRegressionTests_ ()
    {
        Test1_Function_ ();
        Test2_CommandLine_ ();
		Test3_::DoAll ();
		Test4_StaticConstantMaker_::DoAll ();
	}
}

int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
