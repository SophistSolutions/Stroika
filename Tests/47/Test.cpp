/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
//  TEST    Foundation::Math
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"

#include "Stroika/Foundation/Math/Angle.h"
#include "Stroika/Foundation/Math/Common.h"
#include "Stroika/Foundation/Math/LinearAlgebra/Matrix.h"
#include "Stroika/Foundation/Math/Optimization/DownhillSimplexMinimization.h"
#include "Stroika/Foundation/Math/ReBin.h"
#include "Stroika/Foundation/Math/Statistics.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Math;

using namespace Stroika::Frameworks;

#if qHasFeature_GoogleTest
namespace {
    GTEST_TEST (Foundation_Math, Test2_Round_)
    {
        // really could use more cases!!!
        EXPECT_EQ (RoundUpTo (2, 10), 10);
        EXPECT_EQ (RoundDownTo (2, 10), 0);
        EXPECT_TRUE (RoundUpTo (2, 2) == 2);
        EXPECT_TRUE (RoundDownTo (2, 2) == 2);
        EXPECT_TRUE (Round<int> (2.2) == 2);
        EXPECT_TRUE (Round<int> (numeric_limits<double>::max () * 1000) == numeric_limits<int>::max ());
        EXPECT_TRUE (Round<unsigned int> (numeric_limits<double>::max () * 1000) == numeric_limits<unsigned int>::max ());
    }
}

namespace {
    GTEST_TEST (Foundation_Math, Test3_Angle_)
    {
        // really could use more cases!!!
        EXPECT_TRUE (1.1_rad + 1.1_rad < 2.3_rad);
        EXPECT_TRUE (1.1_rad + 1.1_rad < 360_deg);
        EXPECT_TRUE (1.1_rad + 1.1_rad < 180_deg);
        EXPECT_TRUE (1.1_rad + 1.1_rad > 120_deg);
    }
}

namespace {
    GTEST_TEST (Foundation_Math, Test4_OddEvenPrime_)
    {
        EXPECT_TRUE (IsPrime (2));
        EXPECT_TRUE (IsOdd (3));
        EXPECT_TRUE (IsEven (4));
        EXPECT_TRUE (IsPrime (5));
        for (int i = 1; i < 1000; ++i) {
            EXPECT_TRUE (IsOdd (i) != IsEven (i));
            if (IsPrime (i)) {
                EXPECT_TRUE (i == 2 or IsOdd (i));
            }
            if (IsEven (i)) {
                EXPECT_TRUE (i == 2 or not IsPrime (i));
            }
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Math, Test5_ReBin_)
    {
        using ReBin::ReBin;
        {
            uint32_t srcBinData[] = {3, 5, 19, 2};
            double   resultData[4];
            ReBin (begin (srcBinData), end (srcBinData), begin (resultData), end (resultData));
            for (size_t i = 0; i < Memory::NEltsOf (srcBinData); ++i) {
                EXPECT_EQ (srcBinData[i], resultData[i]);
            }
        }
        {
            uint32_t srcBinData[] = {3, 5, 19, 2};
            double   resultData[2];
            ReBin (begin (srcBinData), end (srcBinData), begin (resultData), end (resultData));
            EXPECT_EQ (8, resultData[0]);
            EXPECT_EQ (21, resultData[1]);
        }
        {
            uint32_t srcBinData[] = {3, 5, 19, 2, 0, 0, 0};
            double   resultData[4];
            ReBin (begin (srcBinData), end (srcBinData), begin (resultData), end (resultData));
            EXPECT_TRUE (NearlyEquals ((3 + (5 * ((7.0 / 4.0) - 1))), resultData[0]));
            EXPECT_EQ (0, resultData[3]);
        }
        {
            uint32_t srcBinData[] = {3, 5, 19, 2};
            double   resultData[8];
            ReBin (begin (srcBinData), end (srcBinData), begin (resultData), end (resultData));
            EXPECT_TRUE (NearlyEquals (1.5, resultData[0]));
            EXPECT_TRUE (NearlyEquals (1.5, resultData[1]));
            EXPECT_TRUE (NearlyEquals (2.5, resultData[2]));
            EXPECT_TRUE (NearlyEquals (2.5, resultData[3]));
        }
        {
            uint32_t srcBinData[] = {3, 5, 19, 2};
            double   resultData[4];
            using SRC_DATA_DESCRIPTOR = ReBin::BasicDataDescriptor<double, uint32_t>;
            using TRG_DATA_DESCRIPTOR = ReBin::UpdatableDataDescriptor<double, double>;
            SRC_DATA_DESCRIPTOR srcData{begin (srcBinData), end (srcBinData), 0, 10};
            TRG_DATA_DESCRIPTOR trgData{begin (resultData), end (resultData), 1, 11};
            trgData.clear ();
            ReBin (srcData, &trgData);
            EXPECT_TRUE (NearlyEquals (3.8, resultData[0]));
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Math, Test6_Statistics_)
    {
        EXPECT_TRUE (Math::Mean (vector<int> ({1, 3, 5})) == 3);
        EXPECT_TRUE (Math::Mean (vector<int> ({5, 3, 1})) == 3);
        EXPECT_EQ (Math::Median (vector<int>{1, 3, 5}), 3);
        EXPECT_TRUE (Math::Median (vector<int> ({5, 3, 1})) == 3);
        EXPECT_TRUE (Math::Median (vector<int> ({5, 3, 19, 1})) == 4);
        EXPECT_TRUE (Math::Mean (vector<double> ({5, 3, 19, 1})) == 7);
        EXPECT_TRUE (Math::NearlyEquals (Math::StandardDeviation (vector<double> ({5, 3, 19, 1})), 8.164966, .0001));
    }
}

namespace {
    GTEST_TEST (Foundation_Math, Test7_NearlyEquals_)
    {
        // From Function docs
        EXPECT_TRUE (NearlyEquals (Math::nan (), Math::nan ()));
        EXPECT_TRUE (not NearlyEquals (Math::nan (), 3));
        EXPECT_TRUE (NearlyEquals (Math::infinity (), Math::infinity ()));
        EXPECT_TRUE (not NearlyEquals (Math::infinity (), -Math::infinity ()));
        EXPECT_TRUE (not NearlyEquals (Math::infinity (), 3));
        EXPECT_TRUE (not NearlyEquals (5, 3));
        EXPECT_TRUE (NearlyEquals (5, 3, 2));
        EXPECT_TRUE (NearlyEquals (0.0, -0.0));

        EXPECT_TRUE (Math::NearlyEquals (1.0, 1.0 + numeric_limits<double>::epsilon ()));
        EXPECT_TRUE (Math::NearlyEquals (Math::nan<double> (), Math::nan<double> ()));
        EXPECT_TRUE (not Math::NearlyEquals (1.0, 1.1));
        if constexpr (numeric_limits<double>::digits10 > 14) {
            EXPECT_TRUE (Math::NearlyEquals (1.0e22, 1.000000000000001e22));
        }
        EXPECT_TRUE (not Math::NearlyEquals (1.0e22, 1.1e22));
    }
}

namespace {
    GTEST_TEST (Foundation_Math, Test8_LinearAlgebra_Matrix_)
    {
        using namespace LinearAlgebra;
        {
            Matrix<int> m{10, 10};
            EXPECT_EQ (m[3][3] , 0);
            m.SetAt (3, 3, 5);
            EXPECT_EQ (m[3][3] , 5);
            // @todo support that sort of assign!!!
            //m[3][3] = 5;
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Math, Test9_Optimization_DownhillSimplexMinimization_)
    {
        using namespace Math::Optimization;
        using Characters::String;
        using Containers::Sequence;

        {
            //  COMPARE TEST WITH bash -c "python nelder_mead.py"
            //              [array([ -1.58089710e+00,  -2.39020317e-03,   1.39669799e-06]), -0.99994473460027922]
            DownhillSimplexMinimization::TargetFunction<double> f = [] (const Traversal::Iterable<double>& x) {
                return sin (x.Nth (0)) * cos (x.Nth (1)) * 1 / (abs (x.Nth (2)) + 1);
            };
            DownhillSimplexMinimization::Results<double> result = DownhillSimplexMinimization::Run (f, {0, 0, 0});
            EXPECT_TRUE (Math::NearlyEquals (result.fOptimizedParameters.Nth (0), -1.58089710e+00, 1e-5));
            EXPECT_TRUE (Math::NearlyEquals (result.fOptimizedParameters.Nth (1), -2.39020317e-03, 1e-5));
            EXPECT_TRUE (Math::NearlyEquals (result.fOptimizedParameters.Nth (2), 1.39669799e-06, 1e-5));
            EXPECT_TRUE (Math::NearlyEquals (result.fScore, -0.99994473460027922, 1e-5));
        }
        {
            DownhillSimplexMinimization::TargetFunction<double> f = [] (const Sequence<double>& x) {
                double d = x[0];
                if (d < 0 or d >= Math::kPi) { // avoid falling off ends of ranges - periodic function
                    return 100.0;
                }
                return -cos (d);
            };
            DownhillSimplexMinimization::Results<double> result = DownhillSimplexMinimization::Run (f, {.1});
            EXPECT_TRUE (Math::NearlyEquals (result.fOptimizedParameters[0], 0.0, 1e-10));
        }
        {
            // Sample from Block tuner calibration code
            constexpr double                                            NominalPhiNeutralAngle = 0.541052;
            constexpr double                                            NominalGrooveSpacing   = 1.00E-05;
            static constexpr Common::KeyValuePair<double, unsigned int> kCalData_[]            = {
                {797.4, 24568}, {800.2, 24714}, {803.1, 24860},  {805.3, 25006},  {808.2, 25152},  {810.5, 25298}, {813, 25444},
                {815.5, 25590}, {817.9, 25736}, {820.4, 25882},  {823.1, 26028},  {825.5, 26174},  {828.5, 26320}, {831.2, 26466},
                {833.7, 26612}, {836.2, 26758}, {839.1, 26904},  {842.1, 27050},  {844.6, 27196},  {847.2, 27342}, {850.2, 27488},
                {853.1, 27634}, {855.6, 27780}, {858.5, 27926},  {861.6, 28072},  {864.2, 28218},  {867.2, 28364}, {870.1, 28510},
                {872.9, 28656}, {875.7, 28802}, {878.9, 28948},  {881.6, 29094},  {885, 29240},    {887.7, 29386}, {891, 29532},
                {894, 29678},   {897.3, 29824}, {900.3, 29970},  {903.7, 30116},  {906.7, 30262},  {910.1, 30408}, {913.4, 30554},
                {916.6, 30700}, {920.2, 30846}, {923.5, 30992},  {926.6, 31138},  {929.9, 31284},  {933.2, 31430}, {936.6, 31576},
                {940.3, 31722}, {943.9, 31868}, {947.6, 32014},  {951.1, 32160},  {955, 32306},    {958.6, 32452}, {962.4, 32598},
                {966, 32744},   {969.9, 32890}, {973.3, 33036},  {977.2, 33182},  {981.1, 33328},  {984.8, 33474}, {988.5, 33620},
                {992.3, 33766}, {996.3, 33912}, {1000.5, 34058}, {1004.5, 34204}, {1008.9, 34350}, {1013, 34496},  {1020.8, 34768},
            };
            struct K_Constants_ {
                double k1;
                double k2;
                double tunerInfoD;
                double tunerInfoM{1};
                String ToString () const
                {
                    using namespace Characters::Literals;
                    Characters::StringBuilder sb;
                    sb += L"{";
                    sb += L"k1: " + Characters::Format ("{:.10e}"_f, k1) + ",";
                    sb += L"k2: " + Characters::Format ("{:.10e}"_f, k2) + ",";
                    sb += L"tunerInfoD: " + Characters::Format ("{:.10e}"_f, tunerInfoD) + ",";
                    sb += L"tunerInfoM: " + Characters::Format ("{:.10e}"_f, tunerInfoM);
                    sb += L"}";
                    return sb;
                }
            };
            static constexpr double kDACcountMax_          = 65536;
            static constexpr double k32K                   = kDACcountMax_ / 2;
            auto                    WaveNumber2Wavelength_ = [] (double wn) -> double { return 0.01 / wn; };
            auto                    MDrive2WaveLength      = [] (const K_Constants_& constants, double mirrorDriveValue) -> double {
                double signedMDrive = mirrorDriveValue - k32K;
                return 2 * constants.tunerInfoD / constants.tunerInfoM * sin (constants.k2 + constants.k1 * signedMDrive / k32K);
            };
            auto wavelengthModel = [=] (const K_Constants_& parameters, unsigned int mdrive) {
                constexpr double kMinWaveLengthAllowed_{1.0e-20};
                return Math::AtLeast (MDrive2WaveLength (parameters, mdrive), kMinWaveLengthAllowed_);
            };
            Sequence<double> initialGuess{-4.5 / 210 * 1000 * Math::kPi / 180, NominalPhiNeutralAngle};
            K_Constants_     mdKConstants = {};
            mdKConstants.tunerInfoD       = NominalGrooveSpacing;
            auto fitFun                   = [=] (const K_Constants_& parameters) {
                double result{};
                size_t nEntries{Memory::NEltsOf (kCalData_)};
                for (auto i : kCalData_) {
                    double computedWavelength = wavelengthModel (parameters, i.fValue);
                    Assert (computedWavelength > 0);
                    double calibratedWaveLength = WaveNumber2Wavelength_ (i.fKey);
                    Assert (pow (calibratedWaveLength - computedWavelength, 2) / computedWavelength >= 0);
                    result += pow (calibratedWaveLength - computedWavelength, 2) / computedWavelength;
                }
                return sqrt (result) / nEntries;
            };
            DownhillSimplexMinimization::TargetFunction<double> f = [=] (const Traversal::Iterable<double>& x) -> double {
                K_Constants_ tmp = mdKConstants;
                tmp.k1           = x.Nth (0);
                tmp.k2           = x.Nth (1);
                return fitFun (tmp);
            };
            DownhillSimplexMinimization::Options<double> options;
            options.fNoImprovementThreshold                     = 1e-12;
            DownhillSimplexMinimization::Results<double> result = DownhillSimplexMinimization::Run (f, initialGuess, options);
            EXPECT_TRUE (Math::NearlyEquals (result.fOptimizedParameters[0], -0.52946138144, 1e-5));
            EXPECT_TRUE (Math::NearlyEquals (result.fOptimizedParameters[1], 0.54376305163, 1e-5));
            // Silly to use Nth here, but I used to, and it used to trigger an address sanitizer issue (probably a bug with asan). But still - leave test in -- LGP 2018-09-28
            EXPECT_TRUE (Math::NearlyEquals (result.fOptimizedParameters.Nth (0), -0.52946138144, 1e-5));
            EXPECT_TRUE (Math::NearlyEquals (result.fOptimizedParameters.Nth (1), 0.54376305163, 1e-5));
        }
    }
}
#endif

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qHasFeature_GoogleTest
    return RUN_ALL_TESTS ();
#else
    cerr << "Stroika regression tests require building with google test feature [  PASSED  ]" << endl;
#endif
}
