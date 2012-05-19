/*
 * Copyright(c) Sophist Solutions Inc. 1990-2011.  All rights reserved
 */
#include	"Stroika/Foundation/StroikaPreComp.h"

#include	"Stroika/Foundation/Debug/Assertions.h"
#include	"Stroika/Foundation/Debug/Trace.h"
#include	"Stroika/Foundation/Memory/SharedPtr.h"
#include	"Stroika/Foundation/Memory/WeakSharedPtr.h"

#include	"../TestHarness/SimpleClass.h"
#include	"../TestHarness/TestHarness.h"





using	namespace	Stroika;
using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Memory;




namespace	{
	void	Test1 ()
		{
			struct	T1 {
				string x;
			};
			struct	T2 : T1 {
			};
			SharedPtr<T1>	t1 (new T1 ());
			VerifyTestResult (t1.IsUnique ());
			VerifyTestResult (t1.CurrentRefCount () == 1);
			SharedPtr<T2>	t2 (new T2 ());
			VerifyTestResult (t2.IsUnique ());
			VerifyTestResult (t2.CurrentRefCount () == 1);
			VerifyTestResult (t1 != t2);
			t1 = t2;
			VerifyTestResult (not t1.IsUnique ());
			VerifyTestResult (t1.CurrentRefCount () == 2);
			VerifyTestResult (not t2.IsUnique ());
			VerifyTestResult (t2.CurrentRefCount () == 2);
			VerifyTestResult (t1 == t2);
		}
	void	Test2_SharedFromThis ()
		{
			struct	TTT : Memory::enable_shared_from_this<TTT> {
				string x;
			};
			typedef	SharedPtr<TTT,SharedPtrFromThis_Traits<TTT>>	TTT_SP;

			TTT_SP	t1 (new TTT ());
			TTT_SP	t2 (new TTT ());
			t1 = t2;
			VerifyTestResult (t1.CurrentRefCount () == 2);
			TTT_SP	t3 (t1.get ());
			VerifyTestResult (t1.CurrentRefCount () == 3);
			TTT_SP	t4	=	t1.get()->shared_from_this ();
			VerifyTestResult (t1.CurrentRefCount () == 4);
		}
	void	Test3_WeakPtrs ()
		{
            {
			    WeakCapableSharedPtr<int>	t1 (new int ());
			    WeakSharedPtr<int>			wt1 (t1);
			    VerifyTestResult (wt1.Lock ().get () == t1.get ());
            }
            for (int i = 0; i < 1000; ++i) {
			    WeakSharedPtr<int>			wt1;
                {
			        WeakCapableSharedPtr<int>	t1 (new int ());
                    VerifyTestResult (t1.use_count () == 1);
                    wt1 = WeakSharedPtr<int> (t1);
                    VerifyTestResult (t1.use_count () == 1);
			        VerifyTestResult (wt1.Lock ().get () == t1.get ());
                    VerifyTestResult (t1.use_count () == 1);
                }
                VerifyTestResult (wt1.Lock ().get () == nullptr);
            }
		}
	void	Test4_WeakPtrsWithSharedWithThis ()
		{
            {
			    struct	TTT : Memory::enable_shared_from_this<TTT> {
				    string x;
			    };
			    //typedef	WeakCapableSharedPtr<TTT,SharedPtrFromThis_Traits<TTT>>	TTT_SP;
                typedef SharedPtr<TTT,WeakSharedPtrCapableSharedPtrTraits<SharedPtrFromThis_Traits<TTT>>> TTT_SP;
#if 0
			    TTT_SP	t1 (new TTT ());
			    WeakSharedPtr<TTT>			wt1 (t1);
			    VerifyTestResult (wt1.Lock ().get () == t1.get ());
#endif
            }
		}
}


namespace	{

	void	DoRegressionTests_ ()
		{
		    Test1 ();
			Test2_SharedFromThis ();
			Test3_WeakPtrs ();
            Test4_WeakPtrsWithSharedWithThis ();
		}
}



int main (int argc, const char* argv[])
{
	Stroika::TestHarness::Setup ();
	Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
	return EXIT_SUCCESS;
}



