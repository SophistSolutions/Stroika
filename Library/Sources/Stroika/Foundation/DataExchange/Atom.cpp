/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
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
}

/*
 ********************************************************************************
 *************** DataExchange::Private_::AtomModuleData *************************
 ********************************************************************************
 */
DataExchange::Private_::AtomModuleData::AtomModuleData ()
{
    Assert (sAtomMgrDefaultRep_ == nullptr);
    sAtomMgrDefaultRep_         = new AtomManager_Default_Rep_ ();
    sAtomMgrCaseInsensitiveRep_ = new AtomManager_CaseInsensitive_Rep_ ();
}

DataExchange::Private_::AtomModuleData::~AtomModuleData ()
{
    delete sAtomMgrDefaultRep_;
    sAtomMgrDefaultRep_ = nullptr;
    delete sAtomMgrCaseInsensitiveRep_;
    sAtomMgrCaseInsensitiveRep_ = nullptr;
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
    if (atomI == kEmpty) {
        return String{};
    }
    [[maybe_unused]] auto&& critSec = lock_guard{sAtomMgrDefaultRep_->fCritSec};
    return (sAtomMgrDefaultRep_->fSeq)[atomI];
}

/*
 ********************************************************************************
 ******************** DataExchange::AtomManager_CaseInsensitive *****************
 ********************************************************************************
 */
auto AtomManager_CaseInsensitive::Intern (const String& s) -> AtomInternalType
{
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
    if (atomI == kEmpty) {
        return String{};
    }
    [[maybe_unused]] auto&& critSec = lock_guard{sAtomMgrCaseInsensitiveRep_->fCritSec};
    return (sAtomMgrCaseInsensitiveRep_->fSeq)[atomI];
}
