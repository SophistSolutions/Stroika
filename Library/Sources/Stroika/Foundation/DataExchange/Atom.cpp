/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Concrete/SortedMapping_stdmap.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Execution/Common.h"
#include "Stroika/Foundation/Execution/SpinLock.h"

#include "Atom.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;

// @todo Consider using Synchronized<> for Sequence<>

namespace {
    struct AtomManager_Default_Rep_ {
        // VERY CRUDDY (but close to what we use in HF) impl - to get started...
        /*
         * Use SpinLock since locks very short lived. COULD use shared_mutex because much more reads than writes. But since locks so short, little point.
         */
        Execution::SpinLock                                    fCritSec; // lock needed here to keep map and sequence in sync
        Mapping<String, AtomManager_Default::AtomInternalType> fMap;
        Sequence<String>                                       fSeq;
    };
    inline AtomManager_Default_Rep_& GetAtomManager_Default_Rep_ ()
    {
        static AtomManager_Default_Rep_ sRep_{};
        return sRep_;
    }
}
namespace {
    struct AtomManager_CaseInsensitive_Rep_ {
        // VERY CRUDDY (but close to what we use in HF) impl - to get started...
        /*
         * Use SpinLock since locks very short lived. COULD use shared_mutex because much more reads than writes. But since locks so short, little point.
         */
        Execution::SpinLock                                    fCritSec; // lock needed here to keep map and sequence in sync
        Mapping<String, AtomManager_Default::AtomInternalType> fMap{
            Concrete::SortedMapping_stdmap<String, AtomManager_Default::AtomInternalType>{String::LessComparer{Characters::eCaseInsensitive}}};
        Sequence<String> fSeq;
    };
    inline AtomManager_CaseInsensitive_Rep_& GetAtomManager_CaseInsensitive_Rep_ ()
    {
        static AtomManager_CaseInsensitive_Rep_ sRep_{};
        return sRep_;
    }
}

/*
 ********************************************************************************
 ******************** DataExchange::AtomManager_Default *************************
 ********************************************************************************
 */
auto AtomManager_Default::Intern (const String& s) -> AtomInternalType
{
    AtomInternalType v;
    if (s.empty ()) {
        v = kEmpty;
    }
    else {
        auto&                       mgr = GetAtomManager_Default_Rep_ ();
        [[maybe_unused]] lock_guard critSec{mgr.fCritSec};
        auto                        i = mgr.fMap.Lookup (s);
        if (i.has_value ()) {
            return *i;
        }
        v = mgr.fSeq.size ();
        mgr.fSeq.Append (s);
        mgr.fMap.Add (s, v);
    }
    Ensure (Extract (v) == s);
    return v;
}

String AtomManager_Default::Extract (AtomInternalType atomI)
{
    if (atomI == kEmpty) {
        return String{};
    }
    auto&                       mgr = GetAtomManager_Default_Rep_ ();
    [[maybe_unused]] lock_guard critSec{mgr.fCritSec};
    return (mgr.fSeq)[atomI];
}

/*
 ********************************************************************************
 ****************** DataExchange::AtomManager_CaseInsensitive *******************
 ********************************************************************************
 */
auto AtomManager_CaseInsensitive::Intern (const String& s) -> AtomInternalType
{
    AtomInternalType v;
    if (s.empty ()) {
        v = kEmpty;
    }
    else {
        auto&                       mgr = GetAtomManager_CaseInsensitive_Rep_ ();
        [[maybe_unused]] lock_guard critSec{mgr.fCritSec};
        auto                        i = mgr.fMap.Lookup (s);
        if (i.has_value ()) {
            return *i;
        }
        v = mgr.fSeq.size ();
        mgr.fSeq.Append (s);
        mgr.fMap.Add (s, v);
    }
    Ensure (Extract (v) == s);
    return v;
}

String AtomManager_CaseInsensitive::Extract (AtomInternalType atomI)
{
    if (atomI == kEmpty) {
        return String{};
    }
    auto&                       mgr = GetAtomManager_CaseInsensitive_Rep_ ();
    [[maybe_unused]] lock_guard critSec{mgr.fCritSec};
    return (mgr.fSeq)[atomI];
}
