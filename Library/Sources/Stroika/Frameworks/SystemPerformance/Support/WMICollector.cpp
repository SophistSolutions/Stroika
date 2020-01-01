/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#if qPlatform_Windows
#include <Pdh.h>
#include <PdhMsg.h>
#include <Windows.h>
#else
#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include "../../../Foundation/Characters/CString/Utilities.h"
#include "../../../Foundation/Characters/FloatConversion.h"
#include "../../../Foundation/Characters/Format.h"
#include "../../../Foundation/Characters/String_Constant.h"
#include "../../../Foundation/Containers/Sequence.h"
#include "../../../Foundation/DataExchange/Variant/CharacterDelimitedLines/Reader.h"
#include "../../../Foundation/Debug/Assertions.h"
#include "../../../Foundation/Execution/Sleep.h"

#include "WMICollector.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::SystemPerformance;
using namespace Stroika::Frameworks::SystemPerformance::Support;

using Characters::String_Constant;
using Debug::AssertExternallySynchronizedLock;

#if defined(_MSC_VER)
// Use #pragma comment lib instead of explicit entry in the lib entry of the project file
#pragma comment(lib, "Pdh.lib")
#endif

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 ********** SystemPerformance::Support::WMICollector::PerInstanceData_ **********
 ********************************************************************************
 */
WMICollector::PerInstanceData_::PerInstanceData_ (const String& objectName, const String& instance, const Iterable<String>& counterNames)
    : fObjectName_ (objectName)
    , fInstance_ (instance)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Support::WMICollector::PerInstanceData_::PerInstanceData_");
#endif
    PDH_STATUS x = ::PdhOpenQuery (NULL, NULL, &fQuery_);
    if (x != 0) {
        Execution::Throw (Exception (L"PdhOpenQuery"sv));
    }
    counterNames.Apply ([this] (String i) { AddCounter (i); });
}

WMICollector::PerInstanceData_::~PerInstanceData_ ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Support::WMICollector::PerInstanceData_::~PerInstanceData_");
#endif
    AssertNotNull (fQuery_);
    //@todo add cehck result (this SB assert??)Verify
    ::PdhCloseQuery (fQuery_);
}

void WMICollector::PerInstanceData_::AddCounter (const String& counterName)
{
    Require (not fCounters_.ContainsKey (counterName));
    PDH_HCOUNTER newCounter = nullptr;
    PDH_STATUS   x          = ::PdhAddCounter (fQuery_, Characters::Format (L"\\%s(%s)\\%s", fObjectName_.c_str (), fInstance_.c_str (), counterName.c_str ()).c_str (), NULL, &newCounter);
    if (x != 0) {
        [[maybe_unused]] bool isPDH_CSTATUS_NO_OBJECT  = (x == PDH_CSTATUS_NO_OBJECT);
        [[maybe_unused]] bool isPDH_CSTATUS_NO_COUNTER = (x == PDH_CSTATUS_NO_COUNTER);
        Execution::Throw (Exception (L"PdhAddCounter"sv));
    }
    fCounters_.Add (counterName, newCounter);
}

double WMICollector::PerInstanceData_::GetCurrentValue (const String& counterName)
{
    PDH_FMT_COUNTERVALUE counterVal;
    PDH_HCOUNTER         counter = *fCounters_.Lookup (counterName);
    PDH_STATUS           x       = ::PdhGetFormattedCounterValue (counter, PDH_FMT_DOUBLE, NULL, &counterVal);
    if (x != 0) {
        [[maybe_unused]] bool isPDH_PDH_INVALID_DATA = (x == PDH_INVALID_DATA);
        Execution::Throw (Exception (L"PdhGetFormattedCounterValue"sv));
    }
    return counterVal.doubleValue;
}

optional<double> WMICollector::PerInstanceData_::PeekCurrentValue (const String& counterName)
{
    PDH_FMT_COUNTERVALUE counterVal{};
    PDH_HCOUNTER         counter = *fCounters_.Lookup (counterName);
    PDH_STATUS           x       = ::PdhGetFormattedCounterValue (counter, PDH_FMT_DOUBLE, NULL, &counterVal);
    if (x != 0) {
        return nullopt;
    }
    return counterVal.doubleValue;
}

Mapping<String, double> WMICollector::PerInstanceData_::GetCurrentValues (const String& counterName)
{
    PDH_HCOUNTER                                        counter{*fCounters_.Lookup (counterName)};
    DWORD                                               dwBufferSize{}; // Size of the pItems buffer
    DWORD                                               dwItemCount{};  // Number of items in the pItems buffer
    Memory::SmallStackBuffer<PDH_FMT_COUNTERVALUE_ITEM> items (0);
    // Get the required size of the pItems buffer.
    PDH_STATUS status = ::PdhGetFormattedCounterArray (counter, PDH_FMT_DOUBLE, &dwBufferSize, &dwItemCount, nullptr);
    if (PDH_MORE_DATA == status) {
        items.GrowToSize ((dwBufferSize + sizeof (PDH_FMT_COUNTERVALUE_ITEM) - 1) / sizeof (PDH_FMT_COUNTERVALUE_ITEM));
    }
    status = ::PdhGetFormattedCounterArray (counter, PDH_FMT_DOUBLE, &dwBufferSize, &dwItemCount, items.begin ());

    if (status == PDH_CSTATUS_INVALID_DATA) {
        /*
         *  From: https://msdn.microsoft.com/en-us/library/windows/desktop/aa371894%28v=vs.85%29.aspx
         *      PDH_CSTATUS_INVALID_DATA    The counter was successfully found, but the data returned is not valid.
         *      This error can occur if the counter value is less than the previous value. (Because counter values always
         *      increment, the counter value rolls over to zero when it reaches its maximum value.)
         *      Another possible cause is a system timer that is not correct.
         */
        return Mapping<String, double>{};
    }
    if (status != 0) {
        //PDH_CSTATUS_INVALID_DATA
        [[maybe_unused]] bool isPDH_PDH_INVALID_DATA = (status == PDH_INVALID_DATA);
        Execution::Throw (Exception (L"PdhGetFormattedCounterValue"sv));
    }

    Mapping<String, double> result;
    for (DWORD i = 0; i < dwItemCount; i++) {
        result.Add (items[i].szName, items[i].FmtValue.doubleValue);
    }
    return result;
}

/*
 ********************************************************************************
 ********************* SystemPerformance::Support::WMICollector *****************
 ********************************************************************************
 */
String WMICollector::kWildcardInstance = L"*"sv;

WMICollector::WMICollector (const String& objectName, const Iterable<String>& instances, const Iterable<String>& counterName)
    : fObjectName_ (objectName)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Support::WMICollector::WMICollector");
#endif
    instances.Apply ([this] (String i) { AddInstance_ (i); });
    counterName.Apply ([this] (String i) { AddCounter_ (i); });
}

WMICollector::WMICollector (const WMICollector& from)
    : WMICollector (from.fObjectName_, from.fInstanceData_.Keys (), from.fCounterNames_)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Support::WMICollector::WMICollector");
#endif
    /// @todo auto&& critSec = lock_guard{fCritSection_}; before copy elts!!!

    // Note the above copy CTOR does a second collect, because we don't know how to clone collected data?
}

WMICollector& WMICollector::operator= (const WMICollector& rhs)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Support::WMICollector::operator=");
#endif
    if (this != &rhs) {
        lock_guard<const AssertExternallySynchronizedLock> critSec1{rhs};
        lock_guard<const AssertExternallySynchronizedLock> critSec2{*this};
        fInstanceData_.clear ();
        fObjectName_ = rhs.fObjectName_;
        rhs.fInstanceData_.Keys ().Apply ([this] (String i) { AddInstance_ (i); });
        rhs.fCounterNames_.Apply ([this] (String i) { AddCounter_ (i); });
    }
    return *this;
}

void WMICollector::Collect ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Support::WMICollector::Collect");
#endif
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    fInstanceData_.Apply ([this] (KeyValuePair<String, std::shared_ptr<PerInstanceData_>> i) {
        PDH_STATUS x = ::PdhCollectQueryData (i.fValue->fQuery_);
        if (x != 0) {
            bool isPDH_PDH_NO_DATA = (x == PDH_NO_DATA);
            if (not isPDH_PDH_NO_DATA) {
                // happens when we try to read data about compact disk??? anyhow - best to just not throw here I think?
                // --LGP 2015-05-06 - at least not til I underand better
                Execution::Throw (Exception (L"PdhCollectQueryData"sv));
            }
        }
    });
    fTimeOfLastCollection_ = Time::GetTickCount ();
}

Set<String> WMICollector::GetAvailableInstaces ()
{
    /*
     *  Note: we only want the instanceids here, but this appears to fail if you only request instance ids and not counters at the same time.
     *  Perhaps try again more carefully once I understand PDH better.
     */
    DWORD dwCounterListSize  = 0;
    DWORD dwInstanceListSize = 0;

    PDH_STATUS pdhStatus = ::PdhEnumObjectItems (NULL, NULL, fObjectName_.c_str (), nullptr, &dwCounterListSize, nullptr, &dwInstanceListSize, PERF_DETAIL_WIZARD, 0);
    Assert (pdhStatus == PDH_MORE_DATA);

    SmallStackBuffer<Characters::SDKChar> counterBuf (dwCounterListSize + 2);
    SmallStackBuffer<Characters::SDKChar> instanceBuf (dwInstanceListSize + 2);

    pdhStatus = ::PdhEnumObjectItems (NULL, NULL, fObjectName_.c_str (), counterBuf.begin (), &dwCounterListSize, instanceBuf.begin (), &dwInstanceListSize, PERF_DETAIL_WIZARD, 0);
    if (pdhStatus != 0) {
        Execution::Throw (Exception (L"PdhEnumObjectItems"sv));
    }

    Set<String> result;
    for (const TCHAR* p = instanceBuf.begin (); *p != '\0'; p += Characters::CString::Length (p) + 1) {
        result.Add (String::FromSDKString (p));
    }
    return result;
}

Set<String> WMICollector::GetAvailableCounters ()
{
    /*
     *  Note: we only want the instanceids here, but this appears to fail if you only request instance ids and not counters at the same time.
     *  Perhaps try again more carefully once I understand PDH better.
     */
    DWORD dwCounterListSize  = 0;
    DWORD dwInstanceListSize = 0;

    PDH_STATUS pdhStatus = ::PdhEnumObjectItems (NULL, NULL, fObjectName_.c_str (), nullptr, &dwCounterListSize, nullptr, &dwInstanceListSize, PERF_DETAIL_WIZARD, 0);
    Assert (pdhStatus == PDH_MORE_DATA);

    SmallStackBuffer<Characters::SDKChar> counterBuf (dwCounterListSize + 2);
    SmallStackBuffer<Characters::SDKChar> instanceBuf (dwInstanceListSize + 2);

    pdhStatus = ::PdhEnumObjectItems (NULL, NULL, fObjectName_.c_str (), counterBuf.begin (), &dwCounterListSize, instanceBuf.begin (), &dwInstanceListSize, PERF_DETAIL_WIZARD, 0);
    if (pdhStatus != 0) {
        Execution::Throw (Exception (L"PdhEnumObjectItems"sv));
    }

    Set<String> result;
    for (const TCHAR* p = counterBuf.begin (); *p != '\0'; p += Characters::CString::Length (p) + 1) {
        result.Add (String::FromSDKString (p));
    }
    return result;
}

void WMICollector::AddCounters (const String& counterName)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Support::WMICollector::AddCounters");
#endif
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    AddCounter_ (counterName);
}

void WMICollector::AddCounters (const Iterable<String>& counterNames)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Support::WMICollector::AddCounters");
#endif
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    counterNames.Apply ([this] (String i) { AddCounter_ (i); });
}

void WMICollector::AddInstances (const String& instance)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Support::WMICollector::AddInstances");
#endif
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    AddInstance_ (instance);
}

void WMICollector::AddInstances (const Iterable<String>& instances)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Support::WMICollector::AddInstances");
#endif
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    instances.Apply ([this] (String i) { AddInstance_ (i); });
}

bool WMICollector::AddInstancesIf (const String& instance)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Support::WMICollector::AddInstancesIf");
#endif
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    if (not fInstanceData_.ContainsKey (instance)) {
        AddInstance_ (instance);
        return true;
    }
    return false;
}

bool WMICollector::AddInstancesIf (const Iterable<String>& instances)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Support::WMICollector::AddInstancesIf");
#endif
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    bool                                               anyAdded = false;
    instances.Apply ([this, &anyAdded] (String i) {
        if (not fInstanceData_.ContainsKey (i)) {
            AddInstance_ (i);
            anyAdded = true;
        }
    });
    return anyAdded;
}

double WMICollector::GetCurrentValue (const String& instance, const String& counterName)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Support::WMICollector::GetCurrentValue");
#endif
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    Require (fInstanceData_.ContainsKey (instance));
    return fInstanceData_.Lookup (instance)->get ()->GetCurrentValue (counterName);
}

optional<double> WMICollector::PeekCurrentValue (const String& instance, const String& counterName)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Support::WMICollector::PeekCurrentValue");
#endif
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    Require (fInstanceData_.ContainsKey (instance));
    return fInstanceData_.Lookup (instance)->get ()->PeekCurrentValue (counterName);
}

Mapping<String, double> WMICollector::GetCurrentValues (const String& counterName)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Support::WMICollector::GetCurrentValues");
#endif
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    Require (fInstanceData_.ContainsKey (WMICollector::kWildcardInstance));
    return fInstanceData_.Lookup (WMICollector::kWildcardInstance)->get ()->GetCurrentValues (counterName);
}

void WMICollector::AddCounter_ (const String& counterName)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Support::WMICollector::AddCounter_");
#endif
    //RENEABLKE WHEN WE HAVE RECURSIVE DEBUG LOCK - lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
    Require (not fCounterNames_.Contains (counterName));
    fInstanceData_.Apply ([this, counterName] (KeyValuePair<String, std::shared_ptr<PerInstanceData_>> i) {
        i.fValue->AddCounter (counterName);
    });
    fCounterNames_.Add (counterName);
}

void WMICollector::AddInstance_ (const String& instance)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx ("Stroika::Frameworks::SystemPerformance::Support::WMICollector::AddInstance_");
#endif
    //RENEABLKE WHEN WE HAVE RECURSIVE DEBUG LOCK - lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
    Require (not fInstanceData_.ContainsKey (instance));
    fInstanceData_.Add (instance, make_shared<PerInstanceData_> (fObjectName_, instance, fCounterNames_));
}
