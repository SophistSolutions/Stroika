/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#if     qPlatform_Windows
#include    <Pdh.h>
#include    <PdhMsg.h>
#include    <Windows.h>
#else
#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include    "../../../Foundation/Characters/CString/Utilities.h"
#include    "../../../Foundation/Characters/Format.h"
#include    "../../../Foundation/Characters/String_Constant.h"
#include    "../../../Foundation/Characters/String2Float.h"
#include    "../../../Foundation/Containers/Sequence.h"
#include    "../../../Foundation/DataExchange/CharacterDelimitedLines/Reader.h"
#include    "../../../Foundation/Debug/Assertions.h"
#include    "../../../Foundation/Execution/Sleep.h"

#include    "WMICollector.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::SystemPerformance;
using   namespace   Stroika::Frameworks::SystemPerformance::Support;

using   Characters::String_Constant;




#if     defined (_MSC_VER)
// Use #pragma comment lib instead of explicit entry in the lib entry of the project file
#pragma comment (lib, "Pdh.lib")
#endif





/*
 ********************************************************************************
 ********************* SystemPerformance::Support::WMICollector *****************
 ********************************************************************************
 */
WMICollector::WMICollector (const String& objectName, const String& instanceIndex, const Iterable<String>& counterName)
    : fObjectName_ (objectName)
    , fInstanceIndex_ (instanceIndex)
{
    PDH_STATUS  x = ::PdhOpenQuery (NULL, NULL, &fQuery);
    for (String i : counterName) {
        Add (i);
    }
    Collect ();
    {
        const Time::DurationSecondsType kUseIntervalIfNoBaseline_ { 1.0 };
        Execution::Sleep (kUseIntervalIfNoBaseline_);
    }
}

WMICollector::~WMICollector ()
{
    ::PdhCloseQuery (fQuery);
}

WMICollector::WMICollector (const WMICollector& from)
    : WMICollector (from.fObjectName_, from.fInstanceIndex_, from.fCounters.Keys ())
{
    // Note the above copy CTOR does a second collect, because we dont know how to clone collected data?
}

WMICollector& WMICollector::operator= (const WMICollector& rhs)
{
    if (this != &rhs) {
        fObjectName_ = rhs.fObjectName_;
        fInstanceIndex_ = rhs.fInstanceIndex_;
    }
    PDH_STATUS  x = ::PdhOpenQuery (NULL, NULL, &fQuery);
    for (String i : rhs.fCounters.Keys ()) {
        Add (i);
    }
    Collect ();
    {
        const Time::DurationSecondsType kUseIntervalIfNoBaseline_ { 1.0 };
        Execution::Sleep (kUseIntervalIfNoBaseline_);
    }
    return *this;
}

void     WMICollector::Collect ()
{
    PDH_STATUS  x = PdhCollectQueryData (fQuery);
    if (x != 0) {
        Execution::DoThrow (StringException (L"PdhCollectQueryData"));
    }
    fTimeOfLastCollection = Time::GetTickCount ();
}

void    WMICollector::Add (const String& counterName)
{
    PDH_HCOUNTER newCounter = nullptr;
    PDH_STATUS  x = PdhAddCounter (fQuery, Characters::Format (L"\\%s(%s)\\%s", fObjectName_.c_str (), fInstanceIndex_.c_str (), counterName.c_str ()).c_str (), NULL, &newCounter);
    if (x != 0) {
        bool isPDH_CSTATUS_NO_OBJECT = (x == PDH_CSTATUS_NO_OBJECT);
        bool isPDH_CSTATUS_NO_COUNTER = (x == PDH_CSTATUS_NO_COUNTER);
        Execution::DoThrow (StringException (L"PdhAddCounter"));
    }
    fCounters.Add (counterName, newCounter);
}

double WMICollector::GetCurrentValue (const String& name)
{
    PDH_FMT_COUNTERVALUE counterVal;
    PDH_HCOUNTER    counter = *fCounters.Lookup (name);
    PDH_STATUS  x = ::PdhGetFormattedCounterValue (counter, PDH_FMT_DOUBLE, NULL, &counterVal);
    if (x != 0) {
        Execution::DoThrow (StringException (L"PdhGetFormattedCounterValue"));
    }
    return counterVal.doubleValue;
}
