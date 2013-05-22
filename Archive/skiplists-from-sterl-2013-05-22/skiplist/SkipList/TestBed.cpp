
#include <sys/timeb.h>
#include <tchar.h>
#include <windows.h>

#include <ctype.h>

#include <stdlib.h>

#include <algorithm> 
#include <functional> 
#include <iostream>
#include <string>


#include "SkipList.h"

using namespace std;  


template	<class VALUE_TYPE>	VALUE_TYPE&	LTrim (VALUE_TYPE& s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(isspace))));
	return s;
}

template	<class VALUE_TYPE>	VALUE_TYPE&	RTrim (VALUE_TYPE& s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(isspace))).base(), s.end());
	return s;
}

template	<class VALUE_TYPE>	VALUE_TYPE&	Trim (VALUE_TYPE& s)
{
	return LTrim (RTrim(s));
}

static bool sUserAborted = false;
BOOL WINAPI CtrlCHandlerRoutine (DWORD dwCtrlType)
{
	if (dwCtrlType == CTRL_C_EVENT) {
        cerr << "<CTRL_C> - ending process." << endl;
        sUserAborted = true;
		return true;
	}
	return false;
}

bool	UserAborted ()
{
	return sUserAborted;
}



static	void	Reverse (unsigned char* buf, int entries)
{
	int index = 0;
	while (index < entries/2) {
		char tmp = buf[index];
		buf[index] = buf[entries-index-1];
		buf[entries-index-1] = tmp;
		index++;
	}
}
class Shell	{
	public:
		Shell ();
		virtual	~Shell ();

		nonvirtual void Run ();

	protected:
		nonvirtual bool RunLine (const char* command,  string& args);
		nonvirtual	void	TestSkipList ();
};

Shell::Shell ()
{
}

Shell::~Shell ()
{
}

void	Shell::Run ()
{
	enum {
		kBufferSize = 1000,
	};

	char command [kBufferSize];
	string	arg;
    do  {
		cout << "\n: " << flush;
		cin >> command;

		char args [kBufferSize];
	    cin.getline (args, sizeof(args), '\n'); 
		arg = args;
		Trim (arg);
	} while(not UserAborted () and Shell::RunLine (command, arg));

}

string	GetArg (string& src, bool quoted)
{
	Trim (src);

	string result;
	char	token = (quoted) ? '"' : ' ';
	int	index = src.find (token);
	if (index != -1) {
		int	startIndex = 0;
		if (quoted) {
			startIndex = index+1;
			index = src.find (token, startIndex);
		}
		if (index != -1) {
			result = src.substr (startIndex, (quoted) ? index-1 : index);
			src = src.substr ((quoted) ? index+1 : index);
		}
	}
	return result;
}

bool	GetFlagWithOption (string& src, string flag, string& option)
{
	option.clear ();

	string::size_type idx = src.find (flag);
	if (idx != string::npos)  {
		src.erase (idx, flag.length ());
		if (src[idx] == ' ') {
			src.erase (idx, 1);
            string::size_type idx2 = src.find (' ', idx);
			if (idx2 == string::npos) {
				idx2 = src.size ();
			}
			if (idx2 != string::npos)  {
				option = src.substr (idx, idx2-idx);
				src.erase (idx, idx2-idx);
			}
		}

		return true;
	}
	return false;
}

bool	Shell::RunLine (const char* command, string& args)
{
  try  {
		if (false) {} // makes moving command around easier when they all start else if
        else if (!strcmp (command, "quit")) {
            return false;
        }
        else if (!strcmp (command, "skiplist")) {
			TestSkipList ();
        }
        else {
			cout << "Unknown command: " << command << '\n' << flush;
 		}
    }
#if 0
	catch (StringException& err) {
		err.LogException ();
	}
#endif
	catch (...) {
		cerr << "Unknown error\n" << flush;
	}

	return true;
}






template <>	
struct	KeyValue_SkipList<int, int> {
	KeyValue_SkipList (int k,int v) : 
		fValue (v)  
		{
		}

		int	GetKey () const
		{
			return fValue;
		}

	int	fValue;
};

template <>	
struct	KeyValue_SkipList<string, string> {
	KeyValue_SkipList (string k,string v) : 
		fValue (v)  
		{
		}

		string	GetKey () const
		{
			return fValue;
		}

	string	fValue;
};






#if qSkipListDebug
	static	void	ValidationTests ()
	{
		SkipList<int, int> sl;
		sl.RemoveAll ();
		for (size_t i = 1; i <= 10; ++i) {
			sl.Add (i, i);
			Assert (sl.GetLength () == i);
		}
		sl.ValidateAll ();
		for (size_t i = 5; i <= 10; ++i) {
			Assert (sl.Find (i));
			sl.Remove (i);
			Assert (not sl.Find (i));
		}
		for (size_t i = 1; i <= 4; ++i) {
			Assert (sl.Find (i));
			sl.Remove (i);
			Assert (not sl.Find (i));
		}
		sl.RemoveAll ();

		for (size_t i = 10; i-- > 0; ) {
			sl.Add (1, 1);
		}
		sl.ValidateAll ();
		sl.RemoveAll ();

		for (size_t i = 10; i-- > 0; ) {
			sl.Add (i, i);
		}
		sl.ValidateAll ();
		sl.RemoveAll ();

		for (size_t i = 0; i++ < 1000; ) {
			Assert (not sl.Find (i));
			sl.Add (i, i);
			Assert (sl.Find (i));
		}
		sl.ValidateAll ();
		sl.RemoveAll ();

		for (int i = 100; i < 200; ++i) {
			sl.Add (i, i);
			Assert (sl.Find (i, nullptr));
		}
		sl.ValidateAll ();
		sl.OptimizeLinks ();
		sl.ValidateAll ();
		sl.RemoveAll ();

		for (int i = 0; i <= 10; i += 1) {
			sl.Add (i, i);
			Assert (sl.Find (i));
		}
		sl.ValidateAll ();
		for (int i = 10; i > 0; i -= 2) {
			Assert (sl.Find (i, nullptr));
			sl.Remove (i);
			Assert (not sl.Find (i));
		}
		sl.ValidateAll ();

		for (int i = 0; i <= 100; i += 5) {
			sl.Add (i, i);
			Assert (sl.Find (i));
		}
		sl.ValidateAll ();
		for (int i = 1; i <= 100; i += 2) {
			sl.Add (i, i);
			Assert (sl.Find (i));
		}
		sl.ValidateAll ();
		for (int i = 99; i >= 1; i -= 3) {
			sl.Add (i, i);
			Assert (sl.Find (i));
		}
		sl.ValidateAll ();
		for (int i = 1; i <= 100; i += 2) {
			Assert (sl.Find (i));
			sl.Remove (i);
		}
		sl.ValidateAll ();
	}
#endif

#if qSkipListStatistics
	double	AdditionTest (SkipList<int, int>* sl, size_t elementsToAdd)
	{
		RequireNotNull (sl);

		sl->RemoveAll ();

		cout << "SkipList of " << elementsToAdd << " entries, with link creation probability of " << sl->GetLinkHeightProbability () << endl;

		size_t origLinkCheck  = sl->fLinksChecked;
		size_t origPointerChecks  = sl->fLinksPointerChecked;

		Assert (sl->GetLength () == 0);
		for (size_t i = 0; i < elementsToAdd; ++i) {
			sl->Add (i, i);
		}
		Assert (sl->GetLength () == elementsToAdd);

		// it is a coincidence that you can use same expected for find and add. Add naturally takes more comparisons
		// because you need to set all links pointing to a node. On the other hand, during adds the list length is shorter
		// the extra compares results in x2 average compares, the shorter list results in /2 compares, equalling out
		double	expectedCompares = log (double (sl->GetLength ()))/log (1.0/sl->GetLinkHeightProbability ())/sl->GetLinkHeightProbability ();

		cout << "  add  avg comparisons = " << (sl->fLinksChecked-origLinkCheck)/double (sl->GetLength ()) << "; expected = " << expectedCompares << endl << flush;

		return (sl->fLinksChecked-origLinkCheck)/float (sl->GetLength ());
	}

	double	TraversalTests (SkipList<int, int>* sl)
	{
		RequireNotNull (sl);

		cout << "SkipList of " << sl->GetLength () << " entries, with link creation probability of " << sl->GetLinkHeightProbability () << endl;
		size_t origLinkCheck  = sl->fLinksChecked;
		size_t origPointerChecks  = sl->fLinksPointerChecked;

		double	expectedCompares = log (double (sl->GetLength ()))/log (1.0/sl->GetLinkHeightProbability ())/sl->GetLinkHeightProbability ();

		double linkHeight = 0;
		size_t maxLH = 0; 
		sl->ComputeLinkHeightStats (&linkHeight, &maxLH);

		cout << "  total links = " << linkHeight << "; avg link height = " << linkHeight/sl->GetLength () << "; max link height= " << maxLH << endl;

		origLinkCheck = sl->fLinksChecked;
		origPointerChecks = sl->fLinksPointerChecked;
		for (size_t i = 0; i < sl->GetLength (); ++i) {
			sl->Find (i, nullptr);
		}
		double	findAvgCompares = (sl->fLinksChecked-origLinkCheck)/double (sl->GetLength ());
		cout << "  find avg comparisons = " << findAvgCompares << "; expected = " << expectedCompares << endl << flush;
		cout << "  find pointer comparisons = " << (sl->fLinksPointerChecked-origPointerChecks) << "; % = " << (double (sl->fLinksPointerChecked-origPointerChecks)/(sl->fLinksChecked-origLinkCheck))*100 << "%" << endl << flush;
		
		double	optFindAvgCompares = (sl->fLinksChecked-origLinkCheck)/float (sl->GetLength ());

		return optFindAvgCompares;
	}

	void	TimingTests (SkipList<int, int>* sl, size_t elementsToAdd)
	{
		struct	Timer {
			Timer ()
			{
				fStartTime = GetCurrentTimeInMilliseconds ();
			}

			~Timer ()
			{
				double netTicks = GetCurrentTimeInMilliseconds () - fStartTime;

				char buf[128];
				sprintf_s (buf, sizeof (buf)/sizeof (buf[0]), "\n%ld milliseconds (%5.3f seconds)\n", (long)netTicks, netTicks/1000.0);
				cout << buf << flush;
			}

			static	double	GetCurrentTimeInMilliseconds ()
			{
				timeb t;
				ftime (&t);
				return t.time*1000.0 + t.millitm;
			}

			double	fStartTime;
		};

		RequireNotNull (sl);
		sl->RemoveAll ();

		cout << "SkipList of " << elementsToAdd << " entries, with link creation probability of " << sl->GetLinkHeightProbability () << endl;

		size_t origLinkCheck  = sl->fLinksChecked;

		Assert (sl->GetLength () == 0);
		for (size_t i = 0; i < elementsToAdd; ++i) {
			sl->Add (i, i);
		}
		Assert (sl->GetLength () == elementsToAdd);

		{
			cout << "Find timing, (find each element once)" << endl;
			Timer t;
			for (size_t i = 0; i < sl->GetLength (); ++i) {
				sl->Find (i, nullptr);
			}
		}

		
		cout << endl << "After optimizing links "  << endl;
		sl->OptimizeLinks ();
		{
			cout << "Optimized timing, (find each element once)" << endl;
			Timer t;
			for (size_t i = 0; i < sl->GetLength (); ++i) {
				sl->Find (i, nullptr);
			}
		}
	}
#endif

void	Shell::TestSkipList ()
{
	srand (GetTickCount());
	const	size_t	elementsToTest = 10000;

	Assert (sizeof (KeyValue_SkipList<int, int>) == 4);	// make sure template specialization is working. If not size would be 8  (4 for value, 4 for key)
	
	#if qSkipListDebug
		ValidationTests ();
	#endif	/* qSkipListDebug */

	#if qSkipListStatistics
		{
		SkipList<int, int> sl;
		AdditionTest (&sl, 10000);
		double findAvgCompares = TraversalTests (&sl);
		cout << endl << "After optimizing links "  << endl;
		sl.OptimizeLinks ();
		double optFindAvgCompares = TraversalTests (&sl);
		cout << "  compare reduction = " << (1.0 - optFindAvgCompares/findAvgCompares)*100 << "%" << endl << endl << endl;

		TimingTests (&sl, elementsToTest);
		cout << endl << endl << flush;
		}
	#endif	/* qSkipListStatistics */


	{
		SkipList<int, string> sl;
		sl.Add (3, string ("fred"));
		sl.ValidateAll ();
		string val;
		Assert (sl.Find (3, &val) and (val == "fred"));
	}
	{
		SkipList<string, string>	sl;
		sl.Add ("fred", "fred");
		sl.ValidateAll ();
		string val;
		Assert (sl.Find ("fred", &val) and (val == "fred"));
	}
	{
		SkipList<int*,int*> sl;
		int fred = 5;
		sl.Add (&fred, &fred);
	}

	// test counting
	{
		SkipList<int, int, Comparer<int>, true> sl;
		size_t	maxElements = 1000;
		for (size_t j = 0; j < 5; ++j) {
			for (size_t i = 1; i <= maxElements; ++i) {
				sl.Add (i, i);
				if (j == 0) {
					Assert (sl.GetLength () == i);
					Assert (sl.GetKeyCount () == i);
				}
				else {
					Assert (sl.GetLength () == (maxElements*j + i));
					Assert (sl.GetKeyCount () == maxElements);
				}
				SkipList<int, int, Comparer<int>, true>::Node* n = sl.FindNode (i);
				AssertNotNull (n);
				Assert (n->fCounter.GetCount () == (j+1));
			}
		}
		sl.ValidateAll ();
		for (size_t j = 5; j > 0; --j) {
			for (size_t i = 1; i <= maxElements; ++i) {
				sl.Remove (i);
				if (j == 1) {
					Assert (sl.GetLength () == maxElements-i);
					Assert (sl.GetKeyCount () == maxElements-i);
				}
				else {
					Assert (sl.GetLength () == (maxElements*(j-1) + maxElements-i));
					Assert (sl.GetKeyCount () == maxElements);
				}
				SkipList<int, int, Comparer<int>, true>::Node* n = sl.FindNode (i);
				Assert (j == 1 or n != nullptr);
				Assert (j == 1 or n->fCounter.GetCount () == (j-1));
			}
		}
		sl.ValidateAll ();
	}


#if 0
	SkipList<int, int> sl;
	AdditionTest (&sl, 10000);

	double	shortestCompares = elementsToTest*5;
	double	fastedProb = 0.5;
	for (double prob = 0.5; prob > .2; prob -= .001) {
		sl.OptimizeLinks (prob);
		double avgCompares = TraversalTests (&sl);
		if (avgCompares < shortestCompares) {
			shortestCompares = avgCompares;
			fastedProb = prob;
		}
	}
	cout << "least searches link probability = " << fastedProb << endl;
#endif
}



int _tmain (int argc, _TCHAR* argv[])
{
 	SetConsoleCtrlHandler (CtrlCHandlerRoutine, TRUE);

    if (argc == 1) {
		Shell	s;
		s.Run ();
    }
    else {
    }

	return 0;
}

