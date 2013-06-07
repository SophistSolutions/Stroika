
#include "../Shared/Headers/Config.h"

#include <cstring>

#include <algorithm>
#include <functional>
#include <iostream>
#include <string>

#include "../Shared/Headers/HashKey.h"

#include "../Headers/SkipList.h"
#include "../Headers/SplayTree.h"
#include "../Headers/Treap.h"

#include "../Shared/Headers/TestingData.h"
#include "../Shared/Headers/Utils.h"

using namespace std;


#define	qTweakWeights	0

#define	qTestTreap		1
#define	qTestSplayTree	1
#define	qTestSkipList	1

#if qDebug
    const	size_t	kElementsToTest = 10000;
#else
    const	size_t	kElementsToTest = 1000000;
#endif
    const	size_t kStringElementsToTest = kElementsToTest/10;

static	const	bool	kInOrder = true;
static	const	bool	kVerbose = true;


struct SharedStringKeyValue : public KeyValueInterface<string, string> {
    SharedStringKeyValue (const string& /*k*/,const string& v) :
        fValue (v)
        {
        }
    SharedStringKeyValue (string v) :
        fValue (v)
        {
        }
    SharedStringKeyValue (const char* v) :
        fValue (string (v))
        {
        }

    string	GetKey () const
    {
        return fValue;
    }
    string	GetValue () const
    {
        return fValue;
    }

    string	fValue;

};

struct	CaseInsensitiveCompare : public Comparer<string> {
    CaseInsensitiveCompare ()	{}

    static	int	Compare (const std::string& v1, const std::string& v2)
    {
        return strcasecmp (v1.c_str (), v2.c_str ());
    }
};




TestSet<size_t>	sOrderedIntegers (kElementsToTest, false);
TestSet<size_t>	sScrambledIntegers (kElementsToTest, true);
TestSet<string>	sScrambledStrings (kStringElementsToTest, true);

#if qKeepADTStatistics
    template <typename CONTAINER, typename DATA>
    void	RebuildTree (CONTAINER& sl, const vector<DATA>& data)
    {
        sl.RemoveAll ();
        for (auto it = data.begin (); it != data.end (); ++it) {
            Assert (not sl.Find (*it));
            sl.Add (*it, ComputeUniqueValue (it -data.begin (), false));
        }
    }

    template <typename CONTAINER, typename DATA>
    double	AdditionTest (CONTAINER* sl, const vector<size_t>& data, const vector<size_t>& indices, bool verbose)
    {
        RequireNotNull (sl);

#if qDebug
        size_t origLength = sl->GetLength ();
#endif
        size_t compares = 0;
        size_t rotations = 0;

        for (auto it = indices.begin (); it != indices.end (); ++it) {
            Assert (*it < data.size ());

            size_t v = data[*it];
            size_t origCompares  = sl->fCompares;
            size_t	origRotations = sl->fRotations;
            size_t key = v+1;
            sl->Add (key, v);
            compares += (sl->fCompares-origCompares);
            rotations += (sl->fRotations-origRotations);
            sl->Remove (key);
        }
        Assert (sl->GetLength () == origLength);

        double	avgComps = (compares)/double (indices.size ());
        double	avgRotations = (rotations)/double (indices.size ());

        if (verbose) {
            cout << "  add  avg comparisons = " << avgComps << "; avg rotations = " << avgRotations << endl << flush;
        }

        return avgComps + avgRotations;
    }

    template <typename CONTAINER, typename DATA>
    double	RemovalTest (CONTAINER* sl, const vector<size_t>& data, const vector<size_t>& indices, bool verbose)
    {
        RequireNotNull (sl);

#if qDebug
        size_t origLength = sl->GetLength ();
#endif

        size_t compares = 0;
        size_t rotations = 0;

        for (auto it = indices.begin (); it != indices.end (); ++it) {
            Assert (*it < data.size ());

            size_t origCompares  = sl->fCompares;
            size_t	origRotations = sl->fRotations;
            size_t key = data[*it];
            sl->Remove (key);
            compares += (sl->fCompares-origCompares);
            rotations += (sl->fRotations-origRotations);
            sl->Add (key, key);
        }
        Assert (sl->GetLength () == origLength);

        double	avgComps = (compares)/double (indices.size ());
        double	avgRotations = (rotations)/double (indices.size ());

        if (verbose) {
            cout << "  remove  avg comparisons = " << avgComps <<  "; avg rotations = " << avgRotations << endl << flush;
        }

        return avgComps + avgRotations;
    }

    template <typename CONTAINER, typename DATA>
    double	TraversalTests (CONTAINER* sl, const vector<DATA>& data, const vector<size_t>& indices, bool fail, bool verbose)
    {
        RequireNotNull (sl);

        size_t origCompares = sl->fCompares;
        size_t rotations  = sl->fRotations;

        for (auto it = indices.begin (); it != indices.end (); ++it) {
            Assert (*it < data.size ());
            Verify (sl->Find (data[*it]) == not fail);
        }

        double	findCompares = double (sl->fCompares-origCompares);
        double	findAvgCompares = findCompares/indices.size ();
        double	findAvgRotations = double (sl->fRotations-rotations)/indices.size ();

        if (verbose) {
            if (fail) {
                cout << "  failed find avg comparisons = ";
            }
            else {
                cout << "  find avg comparisons = ";
            }
            cout << findAvgCompares << "; avg rotations = " << findAvgRotations << endl << flush;
        }

        return findAvgCompares + findAvgRotations;
    }

    template <typename CONTAINER, typename DATA>
    double	TestList (CONTAINER& sl, const vector<DATA> data, const vector<DATA> missing, const vector<size_t>& indices, bool verbose)
    {
        size_t totalHeight = 0;
        size_t maxHeight = sl.CalcHeight (&totalHeight) ;

        const	bool	kFail = true;
        if (verbose) {
            cout << "  max height = " << maxHeight << ", average = " << double(totalHeight)/double (sl.GetLength ()) << endl;
        }

        double	findFailWeight	= TraversalTests<CONTAINER, DATA> (&sl, missing, indices, kFail, verbose);	// do before find for real, because that will restructure the tree
        double  findWeight		= TraversalTests<CONTAINER, DATA> (&sl, data, indices, not kFail, verbose);

        double	addWeight		= AdditionTest<CONTAINER, DATA> (&sl, data, indices, verbose);
        double	rmWeight		= RemovalTest<CONTAINER, DATA> (&sl, data, indices, verbose);

        if (verbose) {
            cout << "  ***CRUDE WEIGHT*** = " << (findWeight*8 + addWeight + rmWeight)/10.0 << endl;
        }
        return (findWeight*6 + findFailWeight*2 + addWeight + rmWeight)/10.0;
    }




    template <typename CONTAINER, typename DATA>
    double	IntTimingTests (CONTAINER& sl, const vector<DATA>& data, const vector<size_t>& indices, bool find)
    {
        #define	qUsePrecision 0
        #if qUsePrecision
            LARGE_INTEGER	startTime;
            QueryPerformanceCounter (&startTime);
        #else
            double	timeNeeded = Timer::GetCurrentTimeInMilliseconds ();
        #endif
        for (auto it = indices.begin (); it != indices.end (); ++it) {
            Assert (*it < data.size ());

            size_t v = data[*it];
            Verify (sl.Find (v) == find);
        }
        #if qUsePrecision
            LARGE_INTEGER	endTime;
            QueryPerformanceCounter (&endTime);
            __int64 c = endTime.QuadPart - startTime.QuadPart;
            double	ticks = static_cast<size_t> (c) / 1000000.0;
            return ticks;
        #else
            return Timer::GetCurrentTimeInMilliseconds () - timeNeeded;
        #endif
    }


    struct	TestResult {
        TestResult (bool inOrder, size_t dataLength, DataDistribution d, string description, double weight, double timingSuccess, double timingFail) :
            fInOrder (inOrder),
            fLength (dataLength),
             fDistribution (d),
            fWeight (weight),
            fTimingSuccess (timingSuccess),
            fTimingFail (timingFail),
          fDescription (description)
        {
        }

        static	void	DisplayHeader ()
        {
            cout << "Length\tOrder\tFind Time\tFail Time\tDistribution\tDescription\tWeight\tTotal Time" <<  endl;
        }

        void	DisplayLine ()
        {
            cout << fLength
                << ((fInOrder) ? "\tOrdered\t" : "\tRandom\t")
                << fTimingSuccess << "\t\t" << fTimingFail << "\t\t" << DisplayDistribution (fDistribution) << "\t\t" << fDescription.c_str () << "\t\t" << fWeight << "\t"  << (fTimingSuccess+fTimingFail)<< endl;
        }

        bool			fInOrder;
        size_t			fLength;
        DataDistribution	fDistribution;
        double			fWeight;
        double			fTimingSuccess;
        double			fTimingFail;
        string			fDescription;
    };


    typedef	enum TreeTestType {
        eAll,
        eTimingsOnly,
        eNoTimings,
        eTweaking,
        eNoRebuild
    } TreeTestType;

    template <typename CONTAINER, typename DATA>
    TestResult	RunTreeTests (CONTAINER& sl, TestSet<DATA>& ts, bool inOrder, DataDistribution d, string description, bool verbose, TreeTestType testToRun = eAll)
    {
        const vector<size_t> data	 = ts.GetData ();
        const vector<size_t> missing = ts.GetMissing ();
        const vector<size_t> indices = ts.GetIndices (d);

        if (testToRun != eTweaking and testToRun != eNoRebuild) {
            RebuildTree (sl, data);
        }
        if (verbose) {
            cout << "Testing Container of " << sl.GetLength () << " entries" << " with " << DisplayDistribution (d) << " DataDistribution. type = " << description.c_str ()  << endl;
        }
        double weight = 0;
        if (testToRun != eTimingsOnly/* and testToRun != eTweaking*/) {
            weight = TestList<CONTAINER, DATA> (sl, data, missing, indices, verbose);
        }
        double timingS = 0;
        double timingF = 0;
        if (testToRun != eNoTimings and testToRun != eTweaking) {
            if (testToRun != eNoRebuild) {
                RebuildTree (sl, data);
            }
            const	bool	kFind = true;
            timingF = IntTimingTests (sl, missing, indices, not kFind);
            timingS = IntTimingTests (sl, data, indices, kFind);
        }

        return TestResult (inOrder, sl.GetLength (), d, description, weight, timingS, timingF);
    }
#endif


#if qKeepADTStatistics & qTestSplayTree
    static  const char*	DisplaySplayType (SplayType st)
    {
        switch (st) {
            case eNeverSplay:			return "never";
            case eAlwaysSplay:			return "always";
            case eUniformDistribution:	return "uniform";
            case eNormalDistribution:	return "normal";
            case eZipfDistribution:		return "zipf";
            case eCustomSplayType:		return "custom";
            default:                    break;
        }
        AssertNotReached (); return "";
    }

    static	DataDistribution	SplayTypeToDataDist (SplayType st)
    {
        switch (st) {
            case eNeverSplay:
            case eAlwaysSplay:
            case eUniformDistribution:
                return eUniformDist;
            case eNormalDistribution:
                return eNormalizedDist;
            case eZipfDistribution:
                return eZipfDist;
            case eCustomSplayType:
                return eUniformDist;	// ???
            default:
                break;
        }
        AssertNotReached (); return eUniformDist;
    }

    template <typename CONTAINER, typename DATA>
    double	TimeStrings (bool verbose)
    {
        const vector<string>&	 data	 = sScrambledStrings.GetData ();
        const vector<string>&	 missing = sScrambledStrings.GetMissing ();

        CONTAINER	sl;

        {
            if (verbose) {
                cout << "Creating Splay Tree of " <<  data.size () << " entries, random add:  ";
            }
            Timer t;
            RebuildTree<CONTAINER> (sl, data);
        }
        CONTAINER	sClean = sl;

        if (verbose) {
            cout << endl << "Find timing: find each element once, plus do search for element not in list once for each entry, total finds = " << data.size () + missing.size () << endl;
        }
        double result = 0.0;
        for (int st = eUniformDistribution; st <= eZipfDistribution; ++st) {
            sl.SetSplayType (SplayType (st));
            if (verbose) {
                cout << DisplaySplayType (sl.GetSplayType ()) << " distribution" << endl;
            }
            const vector<size_t> indices = sScrambledStrings.GetIndices (SplayTypeToDataDist (sl.GetSplayType ()));
            {
                Timer t;
                result += TraversalTests<CONTAINER, DATA> (&sl, missing, indices, true, false);
                result += TraversalTests<CONTAINER, DATA> (&sl, data, indices, false, false);
            }
            cout << endl;
            sl = sClean;
        }
        return result;
    }


#endif




#if qDebug & qTestSplayTree
    static	void	SplayTreeValidationTests ()
    {
        SplayTree<size_t, int>	tr;

        Assert (tr.GetLength () == 0);

        tr.SetSplayType (::eAlwaysSplay);
        for (size_t i = 1; i <= 10; ++i) {
            tr.Add (i, i);
            Assert (tr.GetLength () == i);
            Assert (tr.fHead->fEntry.GetKey () == i);
            tr.ValidateAll ();
            Assert (tr.GetFirst ()->fEntry.GetKey () == 1);
            Assert (tr.GetLast ()->fEntry.GetKey () == i);
        }
        for (size_t i = 5; i <= 10; ++i) {
            Assert (tr.Find (i));
            tr.Remove (i);
            Assert (not tr.Find (i));
            tr.ValidateAll ();
        }
        for (size_t i = 1; i <= 4; ++i) {
            Assert (tr.Find (i));
            tr.Remove (i);
            Assert (not tr.Find (i));
        }
        tr.ValidateAll ();
        for (size_t i = 10; i >= 1; --i) {
            tr.Add (i, i);
            Assert (tr.fHead->fEntry.GetKey () == i);
            Assert (tr.Find (i));
        }
        tr.RemoveAll ();
        tr.SetSplayType (::eNeverSplay);
        for (size_t i = 1; i <= 10; ++i) {
            tr.Add (i, i);
            Assert (tr.GetLength () == i);
            Assert (tr.fHead->fEntry.GetKey () == 1);
            tr.ValidateAll ();
        }
        Assert (tr.CalcHeight () == 10);

        tr.SetSplayType (::eNormalDistribution);
        for (size_t i = 10; i-- > 0; ) {
            tr.Add (1, 1);
        }
        tr.ValidateAll ();
        tr.RemoveAll ();

        for (size_t i = 10; i-- > 0; ) {
            tr.Add (i, i);
        }
        tr.ValidateAll ();
        tr.RemoveAll ();

        for (size_t i = 0; i++ < 100; ) {
            Assert (not tr.Find (i));
            tr.Add (i, i);
            Assert (tr.Find (i));
        }
        tr.ValidateAll ();

        {
            SplayTree<size_t, int>	tr1 = tr;
            tr1.ValidateAll ();
            for (size_t i = 0; i++ < 100; ) {
                tr1.Find (1);
            }
            tr1.ValidateAll ();
        }

        tr.RemoveAll ();

        for (int i = 100; i < 200; ++i) {
            tr.Add (i, i);
            Assert (tr.Find (i));
        }
        tr.ValidateAll ();
        tr.RemoveAll ();

        for (int i = 0; i <= 10; i += 1) {
            tr.Add (i, i);
            Assert (tr.Find (i));
        }
        tr.ValidateAll ();
        for (int i = 10; i > 0; i -= 2) {
            Assert (tr.Find (i));
            tr.Remove (i);
            Assert (not tr.Find (i));
        }
        tr.ValidateAll ();

        for (int i = 0; i <= 100; i += 5) {
            tr.Add (i, i);
            Assert (tr.Find (i));
        }
        tr.ValidateAll ();

        for (int i = 1; i <= 100; i += 2) {
            tr.Add (i, i);
            Assert (tr.Find (i));
        }
        tr.ValidateAll ();
        for (int i = 99; i >= 1; i -= 3) {
            tr.Add (i, i);
            Assert (tr.Find (i));
        }
        tr.ValidateAll ();
        for (int i = 1; i <= 100; i += 2) {
            Assert (tr.Find (i));
            tr.Remove (i);
        }

        bool	assertedBadRemove = false;
        try {
            tr.Remove (-1);
        }
        catch (...) {
            assertedBadRemove = true;
        }
        Assert (assertedBadRemove);

        tr.ValidateAll ();
        {
            SplayTree<HashKey<size_t>, int> tr1;
            for (int i = 0; i <= 1000; ++i) {
                tr1.Add (i, i);
                int	v;
                Assert (tr1.Find (i, &v) and (v == i));
            }
            tr1.ValidateAll ();
        }
        {
            SplayTree<size_t, string> sl;
            sl.Add (3, string ("fred"));
            sl.ValidateAll ();
            string val;
            Assert (sl.Find (3, &val) and (val == "fred"));
        }
        {
            SplayTree<int*,int*> sl;
            int fred = 5;
            sl.Add (&fred, &fred);
        }
        {
            SplayTree<string, string>	sl;
            sl.Add ("fred");
            sl.ValidateAll ();
            string val;
            Assert (sl.Find ("fred", &val) and (val == "fred"));
            sl.Add ("George", "George");
            Assert (sl.GetFirst ()->fEntry.GetValue () == string ("George"));	// default to case sensitive compare
            Assert (sl.GetLast ()->fEntry.GetValue () == string ("fred"));
        }
        {
            SplayTree<string, string, TreeTraits::Traits<SharedStringKeyValue, CaseInsensitiveCompare> >	sl;
            sl.Add ("fred");
            sl.Add ("George");
            sl.ValidateAll ();
            string val;
            Assert (sl.Find ("fred", &val) and (val == "fred"));
            Assert (sl.GetFirst ()->fEntry.GetValue () == string ("fred"));
            Assert (sl.GetLast ()->fEntry.GetValue () == string ("George"));
        }
        {
            class MyTree : public SplayTree<HashKey<string>, string> {
                public:
                    MyTree () {}
                    nonvirtual	void	Add (const string& s)
                    {
                        inherited::Add (HashKey<string> (s), s);
                    }
                private:
                    typedef	SplayTree<HashKey<string>, string>	inherited;
            } sl;
            sl.Add ("fred");
            sl.ValidateAll ();
            string val;
            Assert (Hash ("fred") == Hash ("fred"));
            Assert (sl.Find (string ("fred"), &val) and (val == "fred"));
        }
    }
#endif


#if qTestSplayTree
#if qTweakWeights
static	void	DumpVector (const vector<size_t>& v)
{
	cout << "[";
	for (size_t i = 0; i < v.size (); ++i) {
		if (i != 0) {
			cout << ", ";
		}
		cout << v[i];
	}
	cout << "]";
}

static  void	TweakSplayTreeWeights ()
{
    DataDistribution dType = eZipfDist;
    vector<size_t> splayTreeBonuses = SplayTree<size_t, int>::GetHeightWeights (eZipfDistribution);

    const size_t kMyBestSetW[] = {1, 1, 11, 30, 30, 60, 66, 62, 174, 261, 278, 278, 278, 278 };
    vector<size_t>	kOverrideSet(kMyBestSetW, kMyBestSetW + sizeof(kMyBestSetW) / sizeof(kMyBestSetW[0]));
    splayTreeBonuses = kOverrideSet;	// turn on if you want to restart without recompiling splay tree with new weights

    vector<size_t> currentBestEver = splayTreeBonuses;
    SplayTree<size_t, int> sl;
    sl.SetSplayType (eUniformDistribution);
    RebuildTree (sl, sScrambledIntegers.GetData ());
    SplayTree<size_t, int>	cleanT = sl;

    sl.SetSplayType (eCustomSplayType);
    sl.SetCustomHeightWeights (splayTreeBonuses);


    TestResult	tr = RunTreeTests (sl, sScrambledIntegers, not kInOrder, dType, "", not kVerbose, eTweaking);
    //double timeNeeded = tr.fTimingSuccess + tr.fTimingFail;
    double timeNeeded = tr.fWeight;
    sl = cleanT;

    double bestResult = timeNeeded;
    double trueBest = bestResult;
//bestResult *= 2;
    cout << "Starting tests for " << DisplayDistribution (dType) << " with ";
    DumpVector (currentBestEver);
    cout << " in " << trueBest << endl;

    for (int i = 0; i < 10; ++i) {
        sl = cleanT;
       tr = RunTreeTests (sl, sScrambledIntegers, not kInOrder, dType, "", not kVerbose, eTweaking);
//		cout << tr1.fTimingSuccess + tr1.fTimingFail << endl;
        cout << tr.fWeight << endl;
    }

    for (;;) {
        // pick a random weight to muck with
        size_t wIndex = RandomSize_t (0, splayTreeBonuses.size ()-1);
        size_t	curValue = splayTreeBonuses[wIndex];
        size_t mid = curValue/2;
        size_t rangeT = curValue+mid;
        if (rangeT < curValue+4) {
            rangeT = curValue+4;
        }
        size_t newValue = RandomSize_t (mid, rangeT);
        newValue = std::min (size_t (10000), newValue);
        if (newValue == curValue) continue;

cout << "." << flush;
        vector<size_t> restoreSnapShot = splayTreeBonuses;
        splayTreeBonuses[wIndex] = newValue;
        for (size_t ww = wIndex; ww-- > 0;) {
            if (splayTreeBonuses[ww] > newValue) {
                splayTreeBonuses[ww] = newValue;
            }
        }
        for (size_t ww = wIndex+1; ww < splayTreeBonuses.size (); ++ww) {
            if (splayTreeBonuses[ww] == curValue) {
                splayTreeBonuses[ww] = newValue;
            }
        }
        sl.SetCustomHeightWeights (splayTreeBonuses);

        sl = cleanT;
        tr = RunTreeTests (sl, sScrambledIntegers, not kInOrder, dType, "", not kVerbose, eTweaking);
//		double result = tr.fTimingSuccess + tr.fTimingFail;;
        double result = tr.fWeight;
        if (result < bestResult) {
            cout << endl << "**Success!!**"<< " with score = " << result;
            if (result < trueBest) {
                cout << "  ***NEW RECORD***";
            }
            cout << endl << "new vector is ";
            DumpVector (splayTreeBonuses);
            cout << endl;
            bestResult = result;
            if (trueBest > bestResult) {
                currentBestEver = splayTreeBonuses;
                trueBest = bestResult;
            }
        }
        else {
            //cout << "**Failure** because " << result << " > " << bestResult << endl;
            splayTreeBonuses = restoreSnapShot;
        }
    }
    cout << endl << endl << "Final reulst = " << endl;
    DumpVector (splayTreeBonuses);
}
#endif

static  void	TestSplayTree ()
{
#if qDebug
    cout << "validating splaytree" << endl;
    SplayTreeValidationTests ();
    cout << "validated" << endl;
#endif
#if qKeepADTStatistics
        // try to force loading before running any tests
        sOrderedIntegers.PreLoad ();
        sScrambledIntegers.PreLoad ();
        sScrambledStrings.PreLoad ();

        #if qTweakWeights
            TweakSplayTreeWeights ();
        #endif

        SplayTree<size_t, int> sl;
        vector<TestResult>	testResults;

#if 0
        cout << endl << "*** In Order Adds *** " << endl;
        for (int testDist = eUniformDist; testDist <= eZipfDist; ++testDist) {
            for (int testST = eAlwaysSplay; testST <= eZipfDistribution; ++testST) {
                sl.SetSplayType (static_cast<SplayType> (testST));
                testResults.push_back (RunTreeTests (sl, sOrderedIntegers, kInOrder, static_cast<DataDistribution> (testDist), kVerbose));
            }
        }
#endif

        cout << endl << endl << "*** Random Adds *** " << endl;
        for (int testDist = eUniformDist; testDist <= eZipfDist; ++testDist) {
    //	for (int testDist = eZipfDist; testDist <= eZipfDist; ++testDist) {
            for (int testST = eAlwaysSplay; testST <= eZipfDistribution; ++testST) {
    //		for (int testST = eZipfDistribution; testST <= eZipfDistribution; ++testST) {
                sl.SetSplayType (static_cast<SplayType> (testST));
                testResults.push_back (RunTreeTests (sl, sScrambledIntegers, not kInOrder, static_cast<DataDistribution> (testDist), DisplaySplayType (sl.GetSplayType ()), kVerbose));
            }
        }

        TestResult::DisplayHeader ();
        for (auto it = testResults.begin (); it != testResults.end (); ++it) {
            it->DisplayLine ();
        }
        cout << endl << endl;

        cout << "Splay of " <<  sl.GetLength () << " string keyed entries" << endl;
        TimeStrings<SplayTree<string, size_t>, string> (kVerbose);

        cout << endl << "Splay of " <<  sl.GetLength () << " hashed string keyed entries" << endl;
        TimeStrings<SplayTree<HashKey<string>, size_t>, string> (kVerbose);
        cout << endl << endl;
#endif
}
#endif

#if qDebug & qTestTreap
    static	void	TreapValidationTests ()
    {
        Treap<size_t, int>	tr;

        Assert (tr.GetLength () == 0);

        for (size_t i = 1; i <= 10; ++i) {
            tr.Add (i, i);
            Assert (tr.GetLength () == i);
            tr.ValidateAll ();
            Assert (tr.GetFirst ()->fEntry.GetKey () == 1);
            Assert (tr.GetLast ()->fEntry.GetKey () == i);
        }
        for (size_t i = 5; i <= 10; ++i) {
            Assert (tr.Find (i));
            tr.Remove (i);
            Assert (not tr.Find (i));
            tr.ValidateAll ();
        }
        for (size_t i = 1; i <= 4; ++i) {
            Assert (tr.Find (i));
            tr.Remove (i);
            Assert (not tr.Find (i));
        }
        for (size_t i = 10; i >= 1; --i) {
            tr.Add (i, i);
            Assert (tr.Find (i));
        }
        tr.RemoveAll ();

        for (size_t i = 10; i-- > 0; ) {
            tr.Add (1, 1);
        }
        tr.ValidateAll ();
        tr.RemoveAll ();

        for (size_t i = 10; i-- > 0; ) {
            tr.Add (i, i);
        }
        tr.ValidateAll ();
        tr.RemoveAll ();

        for (size_t i = 0; i++ < 100; ) {
            Assert (not tr.Find (i));
            tr.Add (i, i);
            Assert (tr.Find (i));
        }
        tr.ValidateAll ();
        cout << "  height = " << tr.CalcHeight () << endl;
        tr.Optimize ();
        tr.ValidateAll ();
        cout << "  after opt height = " << tr.CalcHeight () << endl;
        tr.RemoveAll ();

        for (int i = 100; i < 200; ++i) {
            tr.Add (i, i);
            Assert (tr.Find (i));
        }
        tr.ValidateAll ();
        tr.RemoveAll ();

        for (int i = 0; i <= 10; i += 1) {
            tr.Add (i, i);
            Assert (tr.Find (i));
        }
        tr.ValidateAll ();
        for (int i = 10; i > 0; i -= 2) {
            Assert (tr.Find (i));
            tr.Remove (i);
            Assert (not tr.Find (i));
        }
        tr.ValidateAll ();

        for (int i = 0; i <= 100; i += 5) {
            tr.Add (i, i);
            Assert (tr.Find (i));
        }
        tr.ValidateAll ();

        for (int i = 1; i <= 100; i += 2) {
            tr.Add (i, i);
            Assert (tr.Find (i));
        }
        tr.ValidateAll ();
        for (int i = 99; i >= 1; i -= 3) {
            tr.Add (i, i);
            Assert (tr.Find (i));
        }
        tr.ValidateAll ();
        for (int i = 1; i <= 100; i += 2) {
            Assert (tr.Find (i));
            tr.Remove (i);
        }
        tr.ValidateAll ();

        {
            Treap<HashKey <size_t>, int> tr1;
            for (int i = 0; i <= 1000; ++i) {
                tr1.Add (i, i);
                int	v;
                Assert (tr1.Find (i, &v) and (v == i));
            }
            tr.ValidateAll ();
        }
        {
            Treap<size_t, string> sl;
            sl.Add (3, string ("fred"));
            sl.ValidateAll ();
            string val;
            Assert (sl.Find (3, &val) and (val == "fred"));
        }
        {
            Treap<string, string>	sl;
            sl.Add ("fred", "fred");
            sl.ValidateAll ();
            string val;
            Assert (sl.Find ("fred", &val) and (val == "fred"));
        }
        {
            Treap<int*,int*> sl;
            int fred = 5;
            sl.Add (&fred, &fred);
        }
}
#endif

#if qKeepADTStatistics & qTestTreap

    template <typename CONTAINER, typename DATA>
    double	TimeStrings_Treap (bool verbose)
    {
        const vector<string>&	 data	 = sScrambledStrings.GetData ();
        const vector<string>&	 missing = sScrambledStrings.GetMissing ();

        CONTAINER	sl;

        {
            if (verbose) {
                cout << "Creating Treap Tree of " <<  data.size () << " entries, random add:  ";
            }
            Timer t;
            RebuildTree<CONTAINER> (sl, data);
        }
        CONTAINER	sClean = sl;

        if (verbose) {
            cout << endl << "Find timing: find each element once, plus do search for element not in list once for each entry, total finds = " << data.size () + missing.size () << endl;
        }
        double result = 0.0;

        for (int st = eUniformDist; st <= eZipfDist; ++st) {
            DataDistribution dd = DataDistribution (st);
            if (verbose) {
                cout << DisplayDistribution (dd) << " distribution" << endl;
            }
            const vector<size_t> indices = sScrambledStrings.GetIndices (dd);
            {
                Timer t;
//size_t compare = sl.fCompares;
                result += TraversalTests<CONTAINER, DATA> (&sl, missing, indices, true, false);
                result += TraversalTests<CONTAINER, DATA> (&sl, data, indices, false, false);
//cout << "comps = " << sl.fCompares - compare << endl;
            }
            cout << endl;
            sl = sClean;
        }

        return result;
    }

#endif

#if qTestTreap



template <typename KEY, typename VALUE, size_t OPTFINDCHANCE>
void	RunTests (vector<TestResult>& testResults)
{
    Treap<KEY, VALUE, TreapTraits<
        KeyValue<KEY, VALUE>,
        TreeTraits::DefaultComp<KEY>,
        TreeTraits::eInvalidRemoveThrowException,
        OPTFINDCHANCE> > sl;

    char buf [100];
    sprintf(buf,"%lu Find Opt",(unsigned long)OPTFINDCHANCE);
    string	description = string (buf);

    RebuildTree (sl, sScrambledIntegers.GetData ());
    for (int testDist = eUniformDist; testDist <= eZipfDist; ++testDist) {
        testResults.push_back (RunTreeTests (sl, sScrambledIntegers, not kInOrder, static_cast<DataDistribution> (testDist), description, kVerbose));
    }
}

static  void	TestTreap ()
{
    #if qDebug
        TreapValidationTests ();
    #endif	/* qDebug */

    #if qKeepADTStatistics
        // try to force loading before running any tests
        sOrderedIntegers.PreLoad ();
        sScrambledIntegers.PreLoad ();
        sScrambledStrings.PreLoad ();


        Treap<size_t, int> sl;
        vector<TestResult>	testResults;


        cout << endl << endl << "*** Random Adds *** " << endl;
        RebuildTree (sl, sScrambledIntegers.GetData ());
        for (int testDist = eUniformDist; testDist <= eZipfDist; ++testDist) {
            testResults.push_back (RunTreeTests (sl, sScrambledIntegers, not kInOrder, static_cast<DataDistribution> (testDist), "unoptimized", kVerbose, eNoRebuild));
        }
        sl.Optimize ();
        for (int testDist = eUniformDist; testDist <= eZipfDist; ++testDist) {
            testResults.push_back (RunTreeTests (sl, sScrambledIntegers, not kInOrder, static_cast<DataDistribution> (testDist), "optimized", kVerbose, eNoRebuild));
        }

        RunTests<size_t, int, 10> (testResults);
        RunTests<size_t, int, 5> (testResults);
        RunTests<size_t, int, 2> (testResults);

        TestResult::DisplayHeader ();
        for (auto it = testResults.begin (); it != testResults.end (); ++it) {
            it->DisplayLine ();
        }
        cout << endl << endl;

        cout << "Treap with string keyed entries" << endl;
        TimeStrings_Treap<Treap<string, size_t>, string> (kVerbose);

        cout << endl << "Treap with hashed string keyed entries" << endl;
        TimeStrings_Treap<Treap<HashKey<string>, size_t>, string> (kVerbose);

        cout << endl << endl;

    #endif	/* qKeepADTStatistics */
}
#endif







#if qDebug & qTestSkipList
    static	void	ValidationTests ()
    {
        SkipList<int, int> sl;
        for (size_t i = 1; i <= 10; ++i) {
            sl.Add (i, i);
            Assert (sl.GetLength () == i);
            sl.ValidateAll ();
        }
        for (size_t i = 5; i <= 10; ++i) {
            Assert (sl.Find (i));
            sl.Remove (i);
            Assert (not sl.Find (i));
            sl.ValidateAll ();
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
        sl.Optimize ();
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
        {
            SkipList<int, string> sl1;
            sl1.Add (3, string ("fred"));
            sl1.ValidateAll ();
            string val;
            Assert (sl1.Find (3, &val) and (val == "fred"));
        }
        {
            SkipList<string, string>	sl1;
            sl1.Add ("fred", "fred");
            sl1.ValidateAll ();
            string val;
            Assert (sl1.Find ("fred", &val) and (val == "fred"));
        }
        {
            SkipList<int*,int*> sl1;
            int fred = 5;
            sl1.Add (&fred, &fred);
        }
    }
#endif

#if qKeepADTStatistics & qTestSkipList
    template <typename CONTAINER, typename DATA>
    double	TimeStrings_SkipList (CONTAINER& sl, bool rebuild, bool verbose)
    {
        const vector<string>&	 data	 = sScrambledStrings.GetData ();
        const vector<string>&	 missing = sScrambledStrings.GetMissing ();

        if (rebuild) {
            if (verbose) {
                cout << "Creating SkipList of " <<  data.size () << " entries, random add:  ";
            }
            Timer t;
            RebuildTree<CONTAINER> (sl, data);
        }
        CONTAINER	sClean = sl;

        if (verbose) {
            cout << endl << "Find timing: find each element once, plus do search for element not in list once for each entry, total finds = " << data.size () + missing.size () << endl;
        }
        double result = 0.0;

        for (int st = eUniformDist; st <= eZipfDist; ++st) {
            DataDistribution dd = DataDistribution (st);
            if (verbose) {
                cout << DisplayDistribution (dd) << " distribution" << endl;
            }
            const vector<size_t> indices = sScrambledStrings.GetIndices (dd);
            {
                Timer t;
//size_t compare = sl.fCompares;
                result += TraversalTests<CONTAINER, DATA> (&sl, missing, indices, true, false);
                result += TraversalTests<CONTAINER, DATA> (&sl, data, indices, false, false);
//cout << "comps = " << sl.fCompares - compare << endl;
            }
            cout << endl;
            sl = sClean;
        }


        return result;
    }
#endif

#if qTestSkipList
static  void	TestSkipList ()
{
//	Assert (sizeof (KeyValue<int, int>) == 4);	// make sure template specialization is working. If not size would be 8  (4 for value, 4 for key)

    #if qDebug
        ValidationTests ();
    #endif	/* qDebug */

    #if qKeepADTStatistics
        // try to force loading before running any tests
        sOrderedIntegers.PreLoad ();
        sScrambledIntegers.PreLoad ();
        sScrambledStrings.PreLoad ();


        SkipList<size_t, int> sl;
        vector<TestResult>	testResults;

        cout << endl << endl << "*** Random Adds *** " << endl;
        RebuildTree (sl, sScrambledIntegers.GetData ());
        for (int testDist = eUniformDist; testDist <= eZipfDist; ++testDist) {
            testResults.push_back (RunTreeTests (sl, sScrambledIntegers, not kInOrder, static_cast<DataDistribution> (testDist), "unoptimized", kVerbose, eNoRebuild));
        }
        sl.Optimize ();
        for (int testDist = eUniformDist; testDist <= eZipfDist; ++testDist) {
            testResults.push_back (RunTreeTests (sl, sScrambledIntegers, not kInOrder, static_cast<DataDistribution> (testDist), "optimized", kVerbose, eNoRebuild));
        }

        TestResult::DisplayHeader ();
        for (auto it = testResults.begin (); it != testResults.end (); ++it) {
            it->DisplayLine ();
        }
        cout << endl << endl;

        const	bool	kRebuild = true;
        {
            SkipList<string, size_t>	slT;

            cout << "Skiplist with string keyed entries" << endl;
            TimeStrings_SkipList<SkipList<string, size_t>, string> (slT, kRebuild, kVerbose);
            {
                cout << "Optimized SkipList  ";
                Timer t;
                slT.Optimize ();
            }
            cout << endl;
            TimeStrings_SkipList<SkipList<string, size_t>, string> (slT, not kRebuild, kVerbose);
        }

        {
            SkipList<HashKey<string>, size_t>	slT;
            cout << endl << "Skiplist with hashed string keyed entries" << endl;
            TimeStrings_SkipList<SkipList<HashKey<string>, size_t>, string> (slT, kRebuild, kVerbose);
        }

        cout << endl << endl;

    #endif	/* qKeepADTStatistics */
}
#endif /*qTestSkipList*/


int main()
{
    cout << "starting " << sScrambledStrings.GetData ().size () << endl;

 #if qTestTreap
    TestTreap ();
#endif
#if qTestSplayTree
    TestSplayTree ();
#endif
#if qTestSkipList
    TestSkipList ();
#endif

    return 0;
}
