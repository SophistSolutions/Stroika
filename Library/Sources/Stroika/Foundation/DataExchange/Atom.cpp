/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/String.h"
#include "../Containers/Mapping.h"
#include "../Containers/Sequence.h"
#include "../Execution/Common.h"
#include "../Execution/SpinLock.h"

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
    AtomManager_Default_Rep_* sAtomMgrDefaultRep_;
    inline void               AssureInited_sAtomMgrDefaultRep_ ()
    {
        static once_flag sOnceFlag_;
        call_once (sOnceFlag_, [] () {
            atexit ([] () {
                delete sAtomMgrDefaultRep_;
            });
            AssertNotNull (sAtomMgrDefaultRep_);
            sAtomMgrDefaultRep_ = new AtomManager_Default_Rep_ ();
        });
    }
}
namespace {
    struct AtomManager_CaseInsensitive_Rep_ {
        // VERY CRUDDY (but close to what we use in HF) impl - to get started...
        /*
         * Use SpinLock since locks very short lived. COULD use shared_mutex because much more reads than writes. But since locks so short, little point.
         */
        Execution::SpinLock                                    fCritSec; // lock needed here to keep map and sequence in sync
        Mapping<String, AtomManager_Default::AtomInternalType> fMap{String::EqualsComparer{Characters::CompareOptions::eCaseInsensitive}};
        Sequence<String>                                       fSeq;
    };
    AtomManager_CaseInsensitive_Rep_* sAtomMgrCaseInsensitiveRep_;
    inline void                       AssureInited_sAtomMgrCaseInsensitiveRep_ ()
    {
        static once_flag sOnceFlag_;
        call_once (sOnceFlag_, [] () {
            atexit ([] () {
                delete sAtomMgrCaseInsensitiveRep_;
            });
            AssertNotNull (sAtomMgrCaseInsensitiveRep_);
            sAtomMgrCaseInsensitiveRep_ = new AtomManager_CaseInsensitive_Rep_ ();
        });
    }
}

/*
 ********************************************************************************
 ******************** DataExchange::AtomManager_Default *************************
 ********************************************************************************
 */
auto AtomManager_Default::Intern (const String& s) -> AtomInternalType
{
    AssureInited_sAtomMgrDefaultRep_ ();
    AtomInternalType v;
    if (s.empty ()) {
        v = kEmpty;
    }
    else {
        [[maybe_unused]] auto&& critSec = lock_guard{sAtomMgrDefaultRep_->fCritSec};
        auto                    i       = sAtomMgrDefaultRep_->fMap.Lookup (s);
        if (i.has_value ()) {
            return *i;
        }
        v = sAtomMgrDefaultRep_->fSeq.GetLength ();
        sAtomMgrDefaultRep_->fSeq.Append (s);
        sAtomMgrDefaultRep_->fMap.Add (s, v);
    }
    Ensure (Extract (v) == s);
    return v;
}

String AtomManager_Default::Extract (AtomInternalType atomI)
{
    RequireNotNull (sAtomMgrDefaultRep_); // must intern before extracting
    if (atomI == kEmpty) {
        return String{};
    }
    [[maybe_unused]] auto&& critSec = lock_guard{sAtomMgrDefaultRep_->fCritSec};
    return (sAtomMgrDefaultRep_->fSeq)[atomI];
}

/*
 ********************************************************************************
 ****************** DataExchange::AtomManager_CaseInsensitive *******************
 ********************************************************************************
 */
auto AtomManager_CaseInsensitive::Intern (const String& s) -> AtomInternalType
{
    AssureInited_sAtomMgrCaseInsensitiveRep_ ();
    AtomInternalType v;
    if (s.empty ()) {
        v = kEmpty;
    }
    else {
        [[maybe_unused]] auto&& critSec = lock_guard{sAtomMgrCaseInsensitiveRep_->fCritSec};
        auto                    i       = sAtomMgrCaseInsensitiveRep_->fMap.Lookup (s);
        if (i.has_value ()) {
            return *i;
        }
        v = sAtomMgrCaseInsensitiveRep_->fSeq.GetLength ();
        sAtomMgrCaseInsensitiveRep_->fSeq.Append (s);
        sAtomMgrCaseInsensitiveRep_->fMap.Add (s, v);
    }
    Ensure (Extract (v) == s);
    return v;
}

String AtomManager_CaseInsensitive::Extract (AtomInternalType atomI)
{
    RequireNotNull (sAtomMgrCaseInsensitiveRep_); // must intern before extracting
    if (atomI == kEmpty) {
        return String{};
    }
    [[maybe_unused]] auto&& critSec = lock_guard{sAtomMgrCaseInsensitiveRep_->fCritSec};
    return (sAtomMgrCaseInsensitiveRep_->fSeq)[atomI];
}
