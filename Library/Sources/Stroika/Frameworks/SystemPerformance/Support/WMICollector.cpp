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
using   Debug::AssertExternallySynchronizedLock;



#if     defined (_MSC_VER)
// Use #pragma comment lib instead of explicit entry in the lib entry of the project file
#pragma comment (lib, "Pdh.lib")
#endif





/*
 ********************************************************************************
 ********** SystemPerformance::Support::WMICollector::PerInstanceData_ **********
 ********************************************************************************
 */
WMICollector::PerInstanceData_::PerInstanceData_ (const String& objectName, const String& instance)
    : fObjectName_ (objectName)
    , fInstance_ (instance)
{
    PDH_STATUS  x = ::PdhOpenQuery (NULL, NULL, &fQuery_);
    if (x != 0) {
        Execution::DoThrow (StringException (L"PdhOpenQuery"));
    }
}

WMICollector::PerInstanceData_::~PerInstanceData_ ()
{
    AssertNotNull (fQuery_);
    ::PdhCloseQuery (fQuery_);
}

void    WMICollector::PerInstanceData_::AddCounter (const String& counterName)
{
    Require (not fCounters_.ContainsKey (counterName));
    PDH_HCOUNTER newCounter = nullptr;
    PDH_STATUS  x = PdhAddCounter (fQuery_, Characters::Format (L"\\%s(%s)\\%s", fObjectName_.c_str (), fInstance_.c_str (), counterName.c_str ()).c_str (), NULL, &newCounter);
    if (x != 0) {
        bool isPDH_CSTATUS_NO_OBJECT = (x == PDH_CSTATUS_NO_OBJECT);
        bool isPDH_CSTATUS_NO_COUNTER = (x == PDH_CSTATUS_NO_COUNTER);
        Execution::DoThrow (StringException (L"PdhAddCounter"));
    }
    fCounters_.Add (counterName, newCounter);
}

double  WMICollector::PerInstanceData_::GetCurrentValue (const String& counterName)
{
    PDH_FMT_COUNTERVALUE counterVal;
    PDH_HCOUNTER    counter = *fCounters_.Lookup (counterName);
    PDH_STATUS  x = ::PdhGetFormattedCounterValue (counter, PDH_FMT_DOUBLE, NULL, &counterVal);
    if (x != 0) {
        Execution::DoThrow (StringException (L"PdhGetFormattedCounterValue"));
    }
    return counterVal.doubleValue;
}



/*
 ********************************************************************************
 ********************* SystemPerformance::Support::WMICollector *****************
 ********************************************************************************
 */
WMICollector::WMICollector (const String& objectName, const String& instance, const Iterable<String>& counterName)
    : WMICollector (objectName, Iterable<String> { instance }, counterName)
{
}

WMICollector::WMICollector (const String& objectName, const Iterable<String>& instances, const Iterable<String>& counterName)
    : fObjectName_ (objectName)
{
    instances.Apply ([this] (String i) { AddInstance_ (i); });
    counterName.Apply ([this] (String i) { AddCounter_ (i); });
    Collect ();
    {
        const Time::DurationSecondsType kUseIntervalIfNoBaseline_ { 1.0 };
        Execution::Sleep (kUseIntervalIfNoBaseline_);
    }
}

WMICollector::WMICollector (const WMICollector& from)
    : WMICollector (from.fObjectName_, from.fInstanceData_.Keys (), from.fCounterNames_)
{
    /// @todo auto    critSec1 { Execution::make_unique_lock (from) }; before copy elts!!!

    // Note the above copy CTOR does a second collect, because we dont know how to clone collected data?
}

WMICollector& WMICollector::operator= (const WMICollector& rhs)
{
    if (this != &rhs) {
        lock_guard<const AssertExternallySynchronizedLock> critSec1 { rhs };
        lock_guard<const AssertExternallySynchronizedLock> critSec2 { *this };
        fInstanceData_.clear ();
        fObjectName_ = rhs.fObjectName_;
        rhs.fInstanceData_.Keys ().Apply ([this] (String i) { AddInstance_ (i); });
        rhs.fCounterNames_.Apply ([this] (String i) { AddCounter_ (i); });
        Collect ();
        {
            const Time::DurationSecondsType kUseIntervalIfNoBaseline_ { 1.0 };
            Execution::Sleep (kUseIntervalIfNoBaseline_);
        }
    }
    return *this;
}

void     WMICollector::Collect ()
{
    lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
    fInstanceData_.Apply ([this] (KeyValuePair<String, std::shared_ptr<PerInstanceData_>> i) {
        PDH_STATUS  x = ::PdhCollectQueryData (i.fValue->fQuery_);
        if (x != 0) {
            Execution::DoThrow (StringException (L"PdhCollectQueryData"));
        }
    });
    fTimeOfLastCollection_ = Time::GetTickCount ();
}

void    WMICollector::AddCounters (const String& counterName)
{
    lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
    AddCounter_ (counterName);
    Collect ();
}

void    WMICollector::AddCounters (const Iterable<String>& counterNames)
{
    lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
    counterNames.Apply ([this] (String i) { AddCounter_ (i); });
    Collect ();
}

void    WMICollector::AddInstances (const String& instance)
{
    lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
    AddInstance_ (instance);
    Collect ();
}

void    WMICollector::AddInstances (const Iterable<String>& instances)
{
    lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
    instances.Apply ([this] (String i) { AddInstance_ (i); });
    Collect ();
}

double  WMICollector::GetCurrentValue (const String& instance, const String& counterName)
{
    lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
    Require (fInstanceData_.ContainsKey (instance));
    return fInstanceData_.Lookup (instance)->get ()->GetCurrentValue (counterName);
}

void    WMICollector::AddCounter_ (const String& counterName)
{
    lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
    Require (not fCounterNames_.Contains (counterName));
    fInstanceData_.Apply ([this, counterName] (KeyValuePair<String, std::shared_ptr<PerInstanceData_>> i) {
        i.fValue->AddCounter (counterName);
    });
    fCounterNames_.Add (counterName);
}

void    WMICollector::AddInstance_ (const String& instance)
{
    lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
    Require (not fInstanceData_.ContainsKey (instance));
    fInstanceData_.Add (instance, make_shared<PerInstanceData_> (fObjectName_, instance) );
}
