#include <cstring>

#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <sstream>


#include "../Shared/Headers/Config.h"
#include "../Shared/Headers/ContainerValidation.h"
#include "../Shared/Headers/HashKey.h"

#include "../Shared/Headers/TestingData.h"
#include "../Shared/Headers/Utils.h"

#define	qTestTreap		1
#define	qTestSplayTree  0
#define	qTestSkipList	0
#define qTestRedBlackTree  0
#define qTestSortedBinaryTree   0
#define qTestAVLTree    0
#define qTestScapeGoatTree    0
#define qTestShuffleTree    0

#define	qTweakWeights	qTestSplayTree & 0


#if qTestTreap
    #include "../Headers/Treap.h"
#endif

#if qTestSplayTree
    #include "../Headers/SplayTree.h"
#endif

#if qTestSkipList
    #include "../Headers/SkipList.h"
// quick hack: define this namespace so can just include SkipList, which doesn't actually care about this namespace
namespace ADT {
    namespace BinaryTree {
    }   // namespace BinaryTree
}   // namespace ADT

#endif

#if qTestRedBlackTree
    #include "../Headers/RedBlackTree.h"
#endif

#if qTestSortedBinaryTree
    #include "../Headers/SortedBinaryTree.h"
#endif

#if qTestAVLTree
    #include "../Headers/AVLTree.h"
#endif

#if qTestScapeGoatTree
    #include "../Headers/ScapeGoatTree.h"
#endif

#if qTestShuffleTree
    #include "../Headers/ShuffleTree.h"
#endif

using namespace std;
using namespace ADT;
using namespace ADT::BinaryTree;



#if qDebug
    const	size_t	kElementsToTest = 1000;
#else
    const	size_t	kElementsToTest = 1000000;
#endif
const	size_t kStringElementsToTest = kElementsToTest/10;

static  const   int     kFindLoops = 10;
static	const	bool	kInOrder = true;
static	const	bool	kVerbose = true;
static  const	bool	kFail = true;



TestSet<size_t>	sSizeTSet (kElementsToTest);
TestSet<string>	sStringsSet (kStringElementsToTest);

template <typename CONTAINER, typename TESTSET>
void	RebuildTree (CONTAINER& t, const TESTSET& ts, bool inOrder)
{
    t.RemoveAll ();

	const std::vector<size_t>&	indices = (inOrder) ? ts.GetOrderedIndices () : ts.GetScrambledIndices ();
	//const vector<size_t>&   data = ts.GetData ();

    for (size_t i = 0; i < ts.GetLength (); ++i) {
        t.Add (ts.GetData ()[indices[i]], i);
    }
}


template <typename CONTAINER, typename TESTSET>
double	IntTimingTests (CONTAINER& sl, const TESTSET& ts, DataDistribution d, bool fail)
{
    #define	qUsePrecision 0
    #if qUsePrecision
        LARGE_INTEGER	startTime;
        QueryPerformanceCounter (&startTime);
    #else
        double	timeNeeded = Timer::GetCurrentTimeInMilliseconds ();
    #endif

    size_t loopEnd = (fail) ? 1 : kFindLoops;
    for (size_t i = 0; i < loopEnd; ++i) {
        for (size_t j = 0; j < ts.GetLength (); ++j) {
            if (fail) {
                Verify (not sl.Find (ts.GetMissing (j, d)));
            }
            else {
                Verify (sl.Find (ts.GetData (j, d)));
            }
        }
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



#if qKeepADTStatistics


    template <typename CONTAINER, typename TESTSET>
    double	AdditionTest (CONTAINER* sl, const TESTSET& ts, DataDistribution d, bool verbose)
    {
        RequireNotNull (sl);

#if qDebug
        size_t origLength = sl->GetLength ();
#endif
        size_t compares = 0;
        size_t rotations = 0;

        for (size_t i = 0; i < ts.GetLength (); ++i) {
             size_t  v = ts.GetData (i, d);
            size_t origCompares  = sl->fCompares;
            size_t	origRotations = sl->fRotations;
            size_t key = v+1;
            sl->Add (key, v);
            compares += (sl->fCompares-origCompares);
            rotations += (sl->fRotations-origRotations);
            sl->Remove (key);
        }
        Assert (sl->GetLength () == origLength);

        double	avgComps = (compares)/double (ts.GetLength ());
        double	avgRotations = (rotations)/double (ts.GetLength ());

        if (verbose) {
            cout << "  add  avg comparisons = " << avgComps << "; avg rotations = " << avgRotations << endl << flush;
        }

        return avgComps + avgRotations;
    }

    template <typename CONTAINER, typename TESTSET>
    double	RemovalTest (CONTAINER* sl, const TESTSET& ts, DataDistribution d, bool verbose)
    {
        RequireNotNull (sl);

#if qDebug
        size_t origLength = sl->GetLength ();
#endif

        size_t compares = 0;
        size_t rotations = 0;

       for (size_t i = 0; i < ts.GetLength (); ++i) {
             size_t  key = ts.GetData (i, d);

            size_t origCompares  = sl->fCompares;
            size_t	origRotations = sl->fRotations;
            sl->Remove (key);
            compares += (sl->fCompares-origCompares);
            rotations += (sl->fRotations-origRotations);
            sl->Add (key, key);
        }
        Assert (sl->GetLength () == origLength);

        double	avgComps = (compares)/double (ts.GetLength ());
        double	avgRotations = (rotations)/double (ts.GetLength ());

        if (verbose) {
            cout << "  remove  avg comparisons = " << avgComps <<  "; avg rotations = " << avgRotations << endl << flush;
        }

        return avgComps + avgRotations;
    }

    template <typename CONTAINER, typename TESTSET>
    double	TraversalTests (CONTAINER* sl, const TESTSET& ts, DataDistribution d, bool fail, bool verbose)
    {
        RequireNotNull (sl);

        size_t compares = sl->fCompares;
        size_t rotations  = sl->fRotations;

        size_t loopEnd = (fail) ? 1 : kFindLoops;
        for (size_t i = 0; i < loopEnd; ++i) {
            for (size_t j = 0; j < ts.GetLength (); ++j) {
                if (fail) {
                    Verify (not sl->Find (ts.GetMissing (j, d)));
                }
                else {
                    Verify (sl->Find (ts.GetData (j, d)));
                }
             }
        }

        size_t  totalFinds = loopEnd * ts.GetLength ();
        double	findAvgCompares  = double (sl->fCompares-compares)/totalFinds;
        double	findAvgRotations = double (sl->fRotations-rotations)/totalFinds;

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

    template <typename CONTAINER, typename TESTSET>
    double	TestList (CONTAINER& sl, const CONTAINER& clean, const TESTSET& testset, DataDistribution d, bool verbose)
    {
        size_t totalHeight = 0;
        size_t maxHeight = sl.CalcHeight (&totalHeight) ;

        if (verbose) {
            cout << "  max height = " << maxHeight << ", average = " << double(totalHeight)/double (sl.GetLength ()) << endl;
        }

        double	findFailWeight	= TraversalTests (&sl, testset, d, kFail, verbose);
        sl = clean;
        double  findWeight		= TraversalTests (&sl, testset, d, not kFail, verbose);
        sl = clean;
        double	addWeight		= AdditionTest (&sl, testset, d, verbose);
        sl = clean;
        double	rmWeight		= RemovalTest (&sl, testset, d, verbose);

        double  weight = (findWeight*kFindLoops + findFailWeight + addWeight + rmWeight)/(kFindLoops + 3);
        if (verbose) {
            cout << "  ***CRUDE WEIGHT*** = " << weight << endl;
        }
        return weight;
    }

    template <typename CONTAINER, typename TESTSET>
    double	TimeStrings (CONTAINER& t, const TESTSET& ts, bool verbose)
    {
        if (t.GetLength () == 0) {
            if (verbose) {
                cout << "Creating Container of " <<  ts.GetLength () << " entries, random add:  ";
            }
            Timer timer;
            RebuildTree<CONTAINER> (t, ts, not kInOrder);
        }
        CONTAINER	sClean = t;

        if (verbose) {
            cout << endl << "Find timing: " << kFindLoops << " times find each element once, plus search for element not in list once for each entry, total finds = " <<
               (ts.GetLength ()*(kFindLoops+1)) << endl;
        }
        double result = 0.0;

        for (int st = eUniformDist; st <= eZipfDist; ++st) {
            DataDistribution dd = DataDistribution (st);
            if (verbose) {
                cout << DisplayDistribution (dd) << " distribution" << endl;
            }
            {
                Timer timer;
                result += TraversalTests (&t, ts, dd, kFail, not kVerbose);
                result += TraversalTests (&t, ts, dd, not kFail, not kVerbose);
            }
            cout << endl;
            t = sClean;
        }

        return result;
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

        ~TestResult ()
        {
        }

        enum {
            kFieldWidth = 14,
        };

        static	void	DisplayHeader ()
        {
//            cout << "Length\tOrder\tFind Time\tFail Time\tDistribution\tDescription\tWeight\tTotal Time" <<  endl;
            cout << "Length"
                << setw(kFieldWidth) << "Order"
                << setw(kFieldWidth) << "Find Time"
                << setw(kFieldWidth) << "Fail Time"
                << setw(kFieldWidth) << "Distribution"
                << setw(kFieldWidth) << "Description"
                << setw(kFieldWidth) << "Weight"
                << setw(kFieldWidth) << "Total Time"
                << endl;
        }

        void	DisplayLine ()
        {
            cout << fLength
                 << setw (kFieldWidth) << ((fInOrder) ? "Ordered" : "Random")
                 << setw (kFieldWidth) << fTimingSuccess
                 << setw (kFieldWidth) << fTimingFail
                 << setw (kFieldWidth) << DisplayDistribution (fDistribution)
                 << setw (kFieldWidth) << fDescription.c_str ()
                 << setw (kFieldWidth) << fWeight
                 << setw (kFieldWidth) << (fTimingSuccess+fTimingFail)
                 << endl;
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
        eBalanceAfterRebuild,
    } TreeTestType;

    template <typename CONTAINER>
    TestResult	RunTreeTests (CONTAINER& sl, bool inOrder, DataDistribution d, string description, bool verbose, TreeTestType testToRun = eAll)
    {
        const TestSet<size_t>&	testset = sSizeTSet;

        if (testToRun != eTweaking) {
            RebuildTree (sl, testset, inOrder);
        }
        if (testToRun == eBalanceAfterRebuild) {
            sl.Balance ();
        }
        CONTAINER   clean = sl;

        if (verbose) {
            cout << "Testing Container of " << sl.GetLength () << " entries" << " with " << DisplayDistribution (d) << " DataDistribution. type = " << description.c_str ()  << endl;
        }
        double weight = 0;
        if (testToRun != eTimingsOnly/* and testToRun != eTweaking*/) {
            weight = TestList (sl, clean, testset, d, verbose);
        }
        double timingS = 0;
        double timingF = 0;
        if (testToRun != eNoTimings and testToRun != eTweaking) {
            sl = clean;
            timingF = IntTimingTests (sl, testset, d, kFail);
            sl = clean;
            timingS = IntTimingTests (sl, testset, d, not kFail);
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

    template <typename CONTAINER>
    double  TimeStringSplayTree (bool verbose)
    {
         CONTAINER	sl;
         RebuildTree (sl, sStringsSet, not kInOrder);

        double result = 0.0;
        if (verbose) {
            cout << DisplaySplayType (sl.GetSplayType ()) << " distribution:\t";
        }
        DataDistribution d = SplayTypeToDataDist (sl.GetSplayType ());
        {
            Timer t;
            result += TraversalTests (&sl, sStringsSet, d, kFail, not kVerbose);
            result += TraversalTests (&sl, sStringsSet, d, not kFail, not kVerbose);
        }

        cout << endl;
        return result;
    }

    template <typename KEY>
    void	TimeStrings (bool verbose)
    {
        typedef SplayTree<KEY, size_t, SplayTraits<KeyValue<KEY,size_t>, ADT::DefaultComp<KEY>, ADT::eDefaultPolicy, eAlwaysSplay> >           AlwaysSplayTree;
        typedef SplayTree<KEY, size_t, SplayTraits<KeyValue<KEY,size_t>, ADT::DefaultComp<KEY>, ADT::eDefaultPolicy, eUniformDistribution> >   UniformSplayTree;
        typedef SplayTree<KEY, size_t, SplayTraits<KeyValue<KEY,size_t>, ADT::DefaultComp<KEY>, ADT::eDefaultPolicy, eNormalDistribution> >    NormalSplayTree;
        typedef SplayTree<KEY, size_t, SplayTraits<KeyValue<KEY,size_t>, ADT::DefaultComp<KEY>, ADT::eDefaultPolicy, eZipfDistribution> >      ZipfSplayTree;


        if (verbose) {
            cout << endl << "Find timing: " << kFindLoops << " times find each element once, plus search for element not in list once for each entry, total finds = " <<
                (sStringsSet.GetLength ()*(kFindLoops+1)) << endl;
        }
        TimeStringSplayTree<AlwaysSplayTree> (verbose);
        TimeStringSplayTree<UniformSplayTree> (verbose);
        TimeStringSplayTree<NormalSplayTree> (verbose);
        TimeStringSplayTree<ZipfSplayTree> (verbose);
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
    typedef SplayTree<size_t, int, SplayTraits<KeyValue<size_t,int>, ADT::DefaultComp<size_t>, ADT::eDefaultPolicy, eCustomSplayType> > TweakedSplayTree;

   DataDistribution dType = eNormalizedDist;
 //    DataDistribution dType = eZipfDist;

    vector<size_t> splayTreeBonuses = SplayTree<size_t, int>::GetHeightWeights (eZipfDistribution);

   // const size_t kMyBestSetW[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192 };

#if 0
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 45, 47, 685, 685
#endif

   const size_t kMyBestSetW[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 45, 47, 685, 685}; // 27.71
    vector<size_t>	kOverrideSet(kMyBestSetW, kMyBestSetW + sizeof(kMyBestSetW) / sizeof(kMyBestSetW[0]));
    splayTreeBonuses = kOverrideSet;	// turn on if you want to restart without recompiling splay tree with new weights

    vector<size_t> currentBestEver = splayTreeBonuses;

    TweakedSplayTree sl;
    RebuildTree (sl, sSizeTSet, not kInOrder);
    TweakedSplayTree	cleanT = sl;

    sl.SetCustomHeightWeights (splayTreeBonuses);


    TestResult	tr = RunTreeTests (sl, not kInOrder, dType, "", not kVerbose, eTweaking);
    //double timeNeeded = tr.fTimingSuccess + tr.fTimingFail;
    double timeNeeded = tr.fWeight;
    sl = cleanT;

    double bestResult = timeNeeded;
    double trueBest = bestResult;

    cout << "Starting tests for " << DisplayDistribution (dType) << " with ";
    DumpVector (currentBestEver);
    cout << " in " << trueBest << endl;

    for (int i = 0; i < 3; ++i) {
        sl = cleanT;
       tr = RunTreeTests (sl,not kInOrder, dType, "", not kVerbose, eTweaking);
//		cout << tr1.fTimingSuccess + tr1.fTimingFail << endl;
        cout << tr.fWeight << endl;
    }

    const   int kFailsCutoff = 10;
    double  kBogusResult = 100000.0;
    int  failsInaRow = 0;
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

      vector<size_t> restoreSnapShot = splayTreeBonuses;
        if (failsInaRow > kFailsCutoff) {
            cout << "X" << flush;
            splayTreeBonuses = currentBestEver;
            sl.SetCustomHeightWeights (splayTreeBonuses);
            bestResult = kBogusResult;
        }
        else {
            cout << "." << flush;
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
        }



        sl = cleanT;
        tr = RunTreeTests (sl, not kInOrder, dType, "", not kVerbose, eTweaking);
//		double result = tr.fTimingSuccess + tr.fTimingFail;;
        double result = tr.fWeight;
        if (result < bestResult) {
            if (bestResult != kBogusResult) {
                cout << endl << "**Success!!**"<< " with score = " << result;
                if (result < trueBest) {
                    cout << "  ***NEW RECORD***";
                }
                cout << endl << "new vector is ";
                DumpVector (splayTreeBonuses);
                cout << endl;
            }
            bestResult = result;
            if (trueBest > bestResult) {
                currentBestEver = splayTreeBonuses;
                trueBest = bestResult;
            }
            failsInaRow = 0;
        }
        else {
            //cout << "**Failure** because " << result << " > " << bestResult << endl;
            splayTreeBonuses = restoreSnapShot;
            failsInaRow++;
        }
    }
    cout << endl << endl << "Final reulst = " << endl;
    DumpVector (splayTreeBonuses);
}
#endif

static  void    TestSplayTrees (bool inOrder, vector<TestResult>& testResults, bool verbose)
{
    if (inOrder) {
         cout << endl << "*** In Order Adds *** " << endl;
    }
    else {
        cout << endl << "*** Random Adds *** " << endl;
    }

    typedef SplayTree<size_t, int, SplayTraits<KeyValue<size_t,int>, ADT::DefaultComp<size_t>, ADT::eDefaultPolicy, eAlwaysSplay> >           AlwaysSplayTree;
    typedef SplayTree<size_t, int, SplayTraits<KeyValue<size_t,int>, ADT::DefaultComp<size_t>, ADT::eDefaultPolicy, eUniformDistribution> >   UniformSplayTree;
    typedef SplayTree<size_t, int, SplayTraits<KeyValue<size_t,int>, ADT::DefaultComp<size_t>, ADT::eDefaultPolicy, eNormalDistribution> >    NormalSplayTree;
    typedef SplayTree<size_t, int, SplayTraits<KeyValue<size_t,int>, ADT::DefaultComp<size_t>, ADT::eDefaultPolicy, eZipfDistribution> >      ZipfSplayTree;

    for (int testDist = eUniformDist; testDist <= eZipfDist; ++testDist) {
        if (testDist == eUniformDist or not inOrder) {
            AlwaysSplayTree splayTree;
            testResults.push_back (RunTreeTests (splayTree, inOrder, static_cast<DataDistribution> (testDist), DisplaySplayType (splayTree.GetSplayType ()), verbose));
        }
        if (testDist == eUniformDist) {
            UniformSplayTree splayTree;
            testResults.push_back (RunTreeTests (splayTree, inOrder, static_cast<DataDistribution> (testDist), DisplaySplayType (splayTree.GetSplayType ()), verbose));
        }
        if (testDist == eUniformDist or not inOrder) {
            NormalSplayTree splayTree;
            testResults.push_back (RunTreeTests (splayTree,inOrder, static_cast<DataDistribution> (testDist), DisplaySplayType (splayTree.GetSplayType ()), verbose));
        }
        if (testDist == eUniformDist or not inOrder) {
            ZipfSplayTree splayTree;
            testResults.push_back (RunTreeTests (splayTree, inOrder, static_cast<DataDistribution> (testDist), DisplaySplayType (splayTree.GetSplayType ()), verbose));
        }
    }
}

static  void	TestSplayTree ()
{
    #if qDebug
        cout << endl << "*** VALIDATING KEY OF size_t *** " << endl;
        SplayTreeValidationSuite<size_t> (kElementsToTest, true);
        cout << endl << "*** VALIDATING KEY OF string *** " << endl;
        SplayTreeValidationSuite<string> (kElementsToTest, true);
        return;
    #endif	/* qDebug */

#if qKeepADTStatistics
        #if qTweakWeights
            TweakSplayTreeWeights ();
        #endif

        vector<TestResult>	testResults;
#if 0
        typedef SplayTree<size_t, int, SplayTraits<KeyValue<size_t,int>, ADT::DefaultComp<size_t>, ADT::eDefaultPolicy, eUniformDistribution> >   UniformSplayTree;
        UniformSplayTree splayTree;
        testResults.push_back (RunTreeTests (splayTree, kInOrder, eUniformDist, DisplaySplayType (splayTree.GetSplayType ()), kVerbose));
        testResults.push_back (RunTreeTests (splayTree, not kInOrder, eUniformDist, DisplaySplayType (splayTree.GetSplayType ()), kVerbose));
#else
        TestSplayTrees (kInOrder, testResults, kVerbose);
        TestSplayTrees (not kInOrder, testResults, kVerbose);
#endif
        cout << endl;
        TestResult::DisplayHeader ();
        for (auto it = testResults.begin (); it != testResults.end (); ++it) {
            it->DisplayLine ();
        }
        cout << endl << endl;

        cout << "Splay of " <<  sStringsSet.GetLength () << " string keyed entries" << endl;
        TimeStrings<string> (kVerbose);

        cout << endl << "Splay of " << sStringsSet.GetLength () << " hashed string keyed entries" << endl;
        TimeStrings<HashKey<string> > (kVerbose);
        cout << endl << endl;
#endif
}
#endif


#if qTestTreap

template <typename KEY, typename VALUE, FindPrioritization OPTFINDCHANCE>
void	RunTests (vector<TestResult>& testResults, bool inOrder, bool balance)
{
    Treap<KEY, VALUE, TreapTraits<
        KeyValue<KEY, VALUE>,
        ADT::DefaultComp<KEY>,
        ADT::eDefaultPolicy,
        OPTFINDCHANCE> > sl;

    string description;
    switch (OPTFINDCHANCE) {
        case FindPrioritization::kNever:
            description = (balance) ? "balanced" : "no priority";
            break;
        case FindPrioritization::kStandard:
            description = ".5";
            break;
        case FindPrioritization::kAlways:
            description = "Always";
            break;
        default:
            AssertNotReached ();
    }

    TreeTestType testToRun = (balance) ? eBalanceAfterRebuild : eAll;
    for (int testDist = eUniformDist; testDist <= eZipfDist; ++testDist) {
        if ((not inOrder) or (testDist == eUniformDist)) {
            testResults.push_back (RunTreeTests (sl, inOrder, static_cast<DataDistribution> (testDist), description, kVerbose, testToRun));
        }
    }
}

static  void	TestTreap ()
{
    #if qDebug
        cout << endl << "*** VALIDATING KEY OF size_t *** " << endl;
        TreapValidationSuite<size_t> (kElementsToTest, true);
        cout << endl << "*** VALIDATING KEY OF string *** " << endl;
        TreapValidationSuite<string> (kElementsToTest, true);


        Treap<size_t, size_t>   tt;
        for (size_t i = 1; i <= 20; ++i) {
            tt.Add (i, i);
        }
        for (size_t i = 1; i <= 20; ++i) {
            Treap<size_t, size_t>::Iterator    it = tt.Iterate (i);

            Assert (not it.Done ());
            auto n = it.GetNode ();
            if (n->GetParent () != nullptr) {
                tt.Prioritize (it);
                Assert (n->GetParent () == nullptr);
                tt.Invariant ();
            }
        }

        // move this to the test suite
struct  NoDefaultArgClass {
    NoDefaultArgClass (int i) :
        fValue (i)
    {
    }

    bool    operator== (const NoDefaultArgClass& rhs)
    {
        return fValue == rhs.fValue;
    }
    bool    operator< (const NoDefaultArgClass& rhs)
    {
        return (fValue < rhs.fValue);
    }
    int fValue;
};
Treap<size_t, NoDefaultArgClass> tr1;   tr1.Add (1, NoDefaultArgClass (1));

Treap<NoDefaultArgClass, NoDefaultArgClass> tr2;    tr2.Add (NoDefaultArgClass (1), NoDefaultArgClass (1));
        return;
    #endif	/* qDebug */

    #if qKeepADTStatistics
        static  const   bool    kRebalance = true;

        vector<TestResult>	testResults;

        cout << endl << endl << "*** Ordered Adds *** " << endl;
        RunTests<size_t, int, FindPrioritization::kNever> (testResults, kInOrder, not kRebalance);
        RunTests<size_t, int, FindPrioritization::kStandard> (testResults, kInOrder, not kRebalance);
        RunTests<size_t, int, FindPrioritization::kAlways> (testResults, kInOrder, not kRebalance);

        cout << endl << endl << "*** Random Adds *** " << endl;
        RunTests<size_t, int, FindPrioritization::kNever> (testResults, not kInOrder, not kRebalance);
        RunTests<size_t, int, FindPrioritization::kNever> (testResults, not kInOrder, kRebalance);
        RunTests<size_t, int, FindPrioritization::kStandard> (testResults, not kInOrder, not kRebalance);
        RunTests<size_t, int, FindPrioritization::kAlways> (testResults, not kInOrder, not kRebalance);

        TestResult::DisplayHeader ();
        for (auto it = testResults.begin (); it != testResults.end (); ++it) {
            it->DisplayLine ();
        }
        cout << endl << endl;

        {
        cout << "Treap with string keyed entries" << endl;
        Treap<string, size_t> t;
        TimeStrings (t, sStringsSet, kVerbose);
        }

        {
             Treap<string, size_t, TreapTraits<
                KeyValue<string, size_t>,
                ADT::DefaultComp<string>,
                ADT::eDefaultPolicy,
                FindPrioritization::kStandard> >t;

            cout  << endl << "Treap Standard Prioritize with string keyed entries" << endl;
            TimeStrings (t, sStringsSet, kVerbose);
        }

        {
            cout << endl << "Treap with hashed string keyed entries" << endl;
            Treap<HashKey<string>, size_t> t;
            TimeStrings (t, sStringsSet, kVerbose);
        }

        cout << endl << endl;

    #endif	/* qKeepADTStatistics */
}
#endif

#if qTestSkipList
static  void	TestSkipList ()
{
//	Assert (sizeof (KeyValue<int, int>) == 4);	// make sure template specialization is working. If not size would be 8  (4 for value, 4 for key)

    #if qDebug
        cout << endl << "*** VALIDATING KEY OF size_t *** " << endl;
        SkipListValidationSuite<size_t> (kElementsToTest, true);
        cout << endl << "*** VALIDATING KEY OF string *** " << endl;
        SkipListValidationSuite<string> (kElementsToTest, true);
#if 0
        {
            SkipList_Patch<size_t, size_t>   tt;
            for (size_t i = 1; i <= 20; ++i) {
                tt.Add (i, i);
            }
            for (size_t i = 1; i <= 20; ++i) {
                tt.Prioritize (i);
                tt.Invariant ();
            }

            for (auto it : tt) {
cout << "(" << it.GetKey () << ", " << it.GetValue () << ")";
            }

            for (auto it = tt.Iterate (); it.More (); ) {
cout << "(" << it->GetKey () << ", " << it->GetValue () << ")";
            }
cout << endl;
#if 1
            auto it = tt.Iterate ();
 cout << endl << "(" << it->GetKey () << ", " << it->GetValue () << ")" << endl;
            tt.Remove (it);
cout << endl << "(" << it->GetKey () << ", " << it->GetValue () << ")" << endl;
            tt.Update (it, 99);
cout << endl << "(" << it->GetKey () << ", " << it->GetValue ()  << ")" << endl;
#endif
++it;
cout << endl << "(" << it->GetKey () << ", " << it->GetValue ()  << ")" << endl;
++it;
cout << endl << "(" << it->GetKey () << ", " << it->GetValue () << ")" << endl;

             for (; it.More (); ) {
cout << "(" << it->GetKey () << ", " << it->GetValue () << ")";
            }
cout << endl;
            for (auto it1 = tt.Iterate (5); it1.More () and (it1->GetKey () < 11); ) {
 cout << "(" << it1->GetKey () << ", " << it1->GetValue () << ")";
            }
cout << endl;
        }
#endif



        return;
    #endif	/* qDebug */

    #if qKeepADTStatistics
        SkipList<size_t, int> sl;
        vector<TestResult>	testResults;

        cout << endl << endl << "*** Ordered Adds *** " << endl;
        testResults.push_back (RunTreeTests (sl, kInOrder, eUniformDist, "unoptimized", kVerbose));

        cout << endl << endl << "*** Random Adds *** " << endl;
        for (int testDist = eUniformDist; testDist <= eZipfDist; ++testDist) {
            testResults.push_back (RunTreeTests (sl, not kInOrder, static_cast<DataDistribution> (testDist), "unoptimized", kVerbose));
        }
        for (int testDist = eUniformDist; testDist <= eZipfDist; ++testDist) {
            testResults.push_back (RunTreeTests (sl, not kInOrder, static_cast<DataDistribution> (testDist), "balanced", kVerbose, eBalanceAfterRebuild));
        }

        TestResult::DisplayHeader ();
        for (auto it = testResults.begin (); it != testResults.end (); ++it) {
            it->DisplayLine ();
        }
        cout << endl << endl;

        {
            SkipList<string, size_t>	slT;

            cout << "Skiplist with string keyed entries" << endl;
            TimeStrings (slT, sStringsSet, kVerbose);
            {
                cout << endl << endl << "Rebalance SkipList  ";
                Timer t;
                slT.Balance ();
            }
            cout << endl;
            TimeStrings (slT, sStringsSet, kVerbose);
        }

        {
            SkipList<HashKey<string>, size_t>	slT;
            cout << endl << "Skiplist with hashed string keyed entries" << endl;
            TimeStrings (slT, sStringsSet, kVerbose);
        }

        cout << endl << endl;

    #endif	/* qKeepADTStatistics */
}
#endif /*qTestSkipList*/



#if qTestRedBlackTree

static  void	TestRedBlackTree ()
{
    #if qDebug
        cout << endl << "*** VALIDATING KEY OF size_t *** " << endl;
        RedBlackTreeValidationSuite<size_t> (kElementsToTest, true);
        cout << endl << "*** VALIDATING KEY OF string *** " << endl;
        RedBlackTreeValidationSuite<string> (kElementsToTest, true);
        return;
    #endif	/* qDebug */

    #if qKeepADTStatistics
        vector<TestResult>	testResults;
        RedBlackTree<size_t, int> sl;

        cout << endl << "*** In Order Adds *** " << endl;
        testResults.push_back (RunTreeTests (sl, kInOrder, eUniformDist, "Unoptimized", kVerbose));
        testResults.push_back (RunTreeTests (sl, kInOrder, eUniformDist, "Balanced", kVerbose, eBalanceAfterRebuild));

        cout << endl << endl << "*** Random Adds *** " << endl;
        for (int testDist = eUniformDist; testDist <= eZipfDist; ++testDist) {
            testResults.push_back (RunTreeTests (sl, not kInOrder, static_cast<DataDistribution> (testDist), "Unoptimized", kVerbose));
        }

        for (int testDist = eUniformDist; testDist <= eZipfDist; ++testDist) {
            testResults.push_back (RunTreeTests (sl, not kInOrder, static_cast<DataDistribution> (testDist), "Balanced", kVerbose, eBalanceAfterRebuild));
        }
        cout << endl << endl;

        TestResult::DisplayHeader ();
        for (auto it = testResults.begin (); it != testResults.end (); ++it) {
            it->DisplayLine ();
        }
        cout << endl << endl;

        {
        cout << "Red Black Tree of " <<  sStringsSet.GetLength () << " string keyed entries" << endl;
        RedBlackTree<string, size_t> t;
        TimeStrings(t, sStringsSet, kVerbose);
        }

        {
        cout << endl << "Red Black Tree of " <<  sStringsSet.GetLength () << " hashed string keyed entries" << endl;
        RedBlackTree<HashKey<string>, size_t>   t;
        TimeStrings (t, sStringsSet, kVerbose);
        }

        cout << endl << endl;
    #endif
}

#endif


#if qTestSortedBinaryTree

static  void	TestSortedBinaryTree ()
{
     #if qDebug
        cout << endl << "*** VALIDATING KEY OF size_t *** " << endl;
        SortedBinaryTreeValidationSuite<size_t> (kElementsToTest, true);
        cout << endl << "*** VALIDATING KEY OF string *** " << endl;
        SortedBinaryTreeValidationSuite<string> (kElementsToTest, true);
        return;
    #endif	/* qDebug */

    #if qKeepADTStatistics
        vector<TestResult>	testResults;
        SortedBinaryTree<size_t, int> sl;

 //       cout << endl << "*** In Order Adds *** " << endl;
        //can't even test, would be unbearably slow
      //  testResults.push_back (RunTreeTests (sl, kInOrder, eUniformDist, "Unoptimized", kVerbose));
      //  testResults.push_back (RunTreeTests (sl, kInOrder, eUniformDist, "Balanced", kVerbose, eBalanceAfterRebuild));

        cout << endl << endl << "*** Random Adds *** " << endl;
        for (int testDist = eUniformDist; testDist <= eZipfDist; ++testDist) {
            testResults.push_back (RunTreeTests (sl, not kInOrder, static_cast<DataDistribution> (testDist), "Unoptimized", kVerbose));
        }

        for (int testDist = eUniformDist; testDist <= eZipfDist; ++testDist) {
            testResults.push_back (RunTreeTests (sl, not kInOrder, static_cast<DataDistribution> (testDist), "Balanced", kVerbose, eBalanceAfterRebuild));
        }
        cout << endl << endl;

        TestResult::DisplayHeader ();
        for (auto it = testResults.begin (); it != testResults.end (); ++it) {
            it->DisplayLine ();
        }
        cout << endl << endl;

        {
        cout << "Sorted Binary Tree of " <<  sStringsSet.GetLength () << " string keyed entries" << endl;
        SortedBinaryTree<string, size_t> t;
        TimeStrings(t, sStringsSet, kVerbose);
        }

        {
        cout << endl << "Sorted Binary Tree of " <<  sStringsSet.GetLength () << " hashed string keyed entries" << endl;
        SortedBinaryTree<HashKey<string>, size_t>   t;
        TimeStrings (t, sStringsSet, kVerbose);
        }

        cout << endl << endl;
    #endif
}

#endif



#if qTestAVLTree

#define   qTestDataDistribution   0
#if     qTestDataDistribution

static  size_t	ListAll (const AVLTree<size_t, size_t>& t)
{
    size_t  biggestValue = 0;
	std::function<void(AVLTree<size_t, size_t>::Node*)>	ListNode = [&ListNode, &biggestValue] (AVLTree<size_t, size_t>::Node* n)
	{
		if (n->GetChild (kLeft) != nullptr) {
			ListNode (n->GetChild (kLeft));
		}
	//	std::cout << n->fEntry.GetKey () << "\t" << n->fEntry.GetValue () << endl;
		if (biggestValue < n->fEntry.GetValue ()) {
		    biggestValue = n->fEntry.GetValue ();
		}
 		if (n->GetChild (kRight) != nullptr) {
			ListNode (n->GetChild (kRight));
		}
	};


	if (t.fHead != nullptr) {
		ListNode (t.fHead);
	}
	return biggestValue;
}

static  void    TestDistribution (DataDistribution dd)
{
    // but who will watch the watchmen?
   AVLTree<size_t, size_t> t;

    for (size_t j = 0; j < sSizeTSet.GetLength (); ++j) {
        size_t  key = sSizeTSet.GetData (j, dd);

        int  comp;
        AVLTree<size_t, size_t>::Node*	n = t.FindNode (key, &comp);
        if (n != nullptr and comp == 0) {
            AssertNotNull (n);
            n->fEntry.SetValue (n->fEntry.GetValue () + 1);
        }
        else {
            t.Add (key, 1);
        }
     }
     size_t biggest = ListAll (t);
     cout << "fed " << sSizeTSet.GetLength () << "; unique = " << t.GetLength () << "; most common = " << biggest << endl;
}
#endif



static  void	TestAVLTree ()
{
#if qTestDataDistribution
        TestDistribution (eUniformDist);
        TestDistribution (eNormalizedDist);
        TestDistribution (eZipfDist);
        return;
#endif

    #if qDebug
        cout << endl << "*** VALIDATING KEY OF size_t *** " << endl;
        AVLTreeValidationSuite<size_t> (kElementsToTest, true);
        cout << endl << "*** VALIDATING KEY OF string *** " << endl;
        AVLTreeValidationSuite<string> (kElementsToTest, true);
        return;
    #endif	/* qDebug */

    #if qKeepADTStatistics
        vector<TestResult>	testResults;
        AVLTree<size_t, int> sl;

        cout << endl << "*** In Order Adds *** " << endl;
        testResults.push_back (RunTreeTests (sl, kInOrder, eUniformDist, "Unoptimized", kVerbose));
        testResults.push_back (RunTreeTests (sl, kInOrder, eUniformDist, "Balanced", kVerbose, eBalanceAfterRebuild));

        cout << endl << endl << "*** Random Adds *** " << endl;
        for (int testDist = eUniformDist; testDist <= eZipfDist; ++testDist) {
            testResults.push_back (RunTreeTests (sl, not kInOrder, static_cast<DataDistribution> (testDist), "Unoptimized", kVerbose));
        }

        for (int testDist = eUniformDist; testDist <= eZipfDist; ++testDist) {
            testResults.push_back (RunTreeTests (sl, not kInOrder, static_cast<DataDistribution> (testDist), "Balanced", kVerbose, eBalanceAfterRebuild));
        }
        cout << endl << endl;

        TestResult::DisplayHeader ();
        for (auto it = testResults.begin (); it != testResults.end (); ++it) {
            it->DisplayLine ();
        }
        cout << endl << endl;

        {
        cout << "AVL Tree of " <<  sStringsSet.GetLength () << " string keyed entries" << endl;
        AVLTree<string, size_t> t;
        TimeStrings(t, sStringsSet, kVerbose);
        }

        {
        cout << endl << "AVL Tree of " <<  sStringsSet.GetLength () << " hashed string keyed entries" << endl;
        AVLTree<HashKey<string>, size_t>   t;
        TimeStrings (t, sStringsSet, kVerbose);
        }
        cout << endl << endl;
    #endif
}

#endif

#if qTestScapeGoatTree
static  string  DisplayScapeGoatType (const double& alpha)
{
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%g", alpha);
    return string (buffer);
}

static  void    TestScapeGoatTrees (bool inOrder, vector<TestResult>& testResults, bool verbose)
{
    if (inOrder) {
         cout << endl << "*** In Order Adds *** " << endl;
    }
    else {
        cout << endl << "*** Random Adds *** " << endl;
    }

    typedef ScapeGoatTree<size_t, int, ScapeGoatTraits<KeyValue<size_t,int>, ADT::DefaultComp<size_t>, ADT::eDefaultPolicy, kMinimalScapeGoatBalance> >  MinimalBalanceScapGoatTree;
    typedef ScapeGoatTree<size_t, int, ScapeGoatTraits<KeyValue<size_t,int>, ADT::DefaultComp<size_t>, ADT::eDefaultPolicy, kMaximalScapeGoatBalance> >  MaximalBalanceScapGoatTree;
    typedef ScapeGoatTree<size_t, int, ScapeGoatTraits<KeyValue<size_t,int>, ADT::DefaultComp<size_t>, ADT::eDefaultPolicy, kDefaultScapeGoatBalance> >  DefaultBalanceScapGoatTree;


    for (int testDist = eUniformDist; testDist <= eZipfDist; ++testDist) {
        if (testDist == eUniformDist or not inOrder) {
            MinimalBalanceScapGoatTree sgTree;
            testResults.push_back (RunTreeTests (sgTree, inOrder, static_cast<DataDistribution> (testDist), DisplayScapeGoatType (sgTree.GetAlpha ()), verbose));
        }
        if (testDist == eUniformDist or not inOrder) {
            MaximalBalanceScapGoatTree sgTree;
            testResults.push_back (RunTreeTests (sgTree, inOrder, static_cast<DataDistribution> (testDist), DisplayScapeGoatType (sgTree.GetAlpha ()), verbose));
        }
        if (testDist == eUniformDist or not inOrder) {
            DefaultBalanceScapGoatTree sgTree;
            testResults.push_back (RunTreeTests (sgTree,inOrder, static_cast<DataDistribution> (testDist), DisplayScapeGoatType (sgTree.GetAlpha ()), verbose));
        }
    }
}

static  void	TestScapeGoatTree ()
{
   #if qDebug
        cout << endl << "*** VALIDATING KEY OF size_t *** " << endl;
        ScapeGoatTreeValidationSuite<size_t> (kElementsToTest, true);
        cout << endl << "*** VALIDATING KEY OF string *** " << endl;
        ScapeGoatTreeValidationSuite<string> (kElementsToTest, true);
        return;
    #endif	/* qDebug */

    #if qKeepADTStatistics

        vector<TestResult>	testResults;
        ScapeGoatTree<size_t, int> sl;;

        cout << endl << "*** In Order Adds *** " << endl;
        testResults.push_back (RunTreeTests (sl, kInOrder, eUniformDist, "Unoptimized", kVerbose));
        testResults.push_back (RunTreeTests (sl, kInOrder, eUniformDist, "Balanced", kVerbose, eBalanceAfterRebuild));

        TestScapeGoatTrees (not kInOrder, testResults, kVerbose);

        cout << endl << endl;

        TestResult::DisplayHeader ();
        for (auto it = testResults.begin (); it != testResults.end (); ++it) {
            it->DisplayLine ();
        }
        cout << endl << endl;

        {
        cout << "ScapeGoat Tree of " <<  sStringsSet.GetLength () << " string keyed entries" << endl;
        ScapeGoatTree<string, size_t> t;
        TimeStrings(t, sStringsSet, kVerbose);
        }

        {
        cout << endl << "ScapeGoat Tree of " <<  sStringsSet.GetLength () << " hashed string keyed entries" << endl;
        ScapeGoatTree<HashKey<string>, size_t>   t;
        TimeStrings (t, sStringsSet, kVerbose);
        }
        cout << endl << endl;
    #endif
}
#endif


#if qTestShuffleTree

static  void	TestShuffleTree ()
{
     #if qDebug
        cout << endl << "*** VALIDATING KEY OF size_t *** " << endl;
        ShuffleTreeValidationSuite<size_t> (kElementsToTest, true);
        cout << endl << "*** VALIDATING KEY OF string *** " << endl;
        ShuffleTreeValidationSuite<string> (kElementsToTest, true);
        return;
    #endif	/* qDebug */

    #if qKeepADTStatistics
        vector<TestResult>	testResults;
        ShuffleTree<size_t, int> sl;

        cout << endl << "*** In Order Adds *** " << endl;
        for (int testDist = eUniformDist; testDist <= eZipfDist; ++testDist) {
            testResults.push_back (RunTreeTests (sl, kInOrder, static_cast<DataDistribution> (testDist), "Unoptimized", kVerbose));
        }

        cout << endl << endl << "*** Random Adds *** " << endl;
        for (int testDist = eUniformDist; testDist <= eZipfDist; ++testDist) {
            testResults.push_back (RunTreeTests (sl, not kInOrder, static_cast<DataDistribution> (testDist), "Unoptimized", kVerbose));
        }

        for (int testDist = eUniformDist; testDist <= eZipfDist; ++testDist) {
            testResults.push_back (RunTreeTests (sl, not kInOrder, static_cast<DataDistribution> (testDist), "Balanced", kVerbose, eBalanceAfterRebuild));
        }
        cout << endl << endl;

        TestResult::DisplayHeader ();
        for (auto it = testResults.begin (); it != testResults.end (); ++it) {
            it->DisplayLine ();
        }
        cout << endl << endl;

        {
        cout << "Shuffle Tree of " <<  sStringsSet.GetLength () << " string keyed entries" << endl;
        ShuffleTree<string, size_t> t;
        TimeStrings(t, sStringsSet, kVerbose);
        }

        {
        cout << endl << "Sorted Binary Tree of " <<  sStringsSet.GetLength () << " hashed string keyed entries" << endl;
        ShuffleTree<HashKey<string>, size_t>   t;
        TimeStrings (t, sStringsSet, kVerbose);
        }

        cout << endl << endl;
    #endif
}







#endif
int main()
{
#if 0
    extern  void    CurrentTesting ();
    CurrentTesting ();
    cout << endl << endl << endl << "FINISHED" << endl << flush;
    return 0;
#endif

 #if qTestTreap
    TestTreap ();
#endif
#if qTestSplayTree
    TestSplayTree ();
#endif
#if qTestSkipList
    TestSkipList ();
#endif
#if qTestRedBlackTree
    TestRedBlackTree ();
#endif
#if qTestSortedBinaryTree
    TestSortedBinaryTree ();
#endif
#if qTestAVLTree
    TestAVLTree ();
#endif
#if qTestScapeGoatTree
    TestScapeGoatTree ();
#endif
#if qTestShuffleTree
    TestShuffleTree ();
#endif

    return 0;
}
