/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/String.h"
#include    "../Containers/Mapping.h"
#include    "../Containers/Sequence.h"

#include    "Atom.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::DataExchange;


namespace {
    // VERY CRUDDY (but close to what we use in HF) impl - to get started...
    mutex   sCritSec_;  // lock needed here to keep map and sequence in sync
    Mapping<String, AtomManager_Default::AtomInternalType>*   sMap_;
    Sequence<String>*                                         sSeq_;
}






/*
 ********************************************************************************
 *************** DataExchange::Private_::AtomModuleData *************************
 ********************************************************************************
 */
DataExchange::Private_::AtomModuleData::AtomModuleData ()
{
    Assert (sMap_ == nullptr);
    Assert (sSeq_ == nullptr);
    sMap_ = new Mapping<String, AtomManager_Default::AtomInternalType> ();
    sSeq_ = new Sequence<String> ();
}

DataExchange::Private_::AtomModuleData::~AtomModuleData ()
{
    delete sMap_;
    sMap_ = nullptr;
    delete sSeq_;
    sSeq_ = nullptr;
}





/*
 ********************************************************************************
 ******************** DataExchange::AtomManager_Default *************************
 ********************************************************************************
 */
AtomManager_Default::AtomInternalType   AtomManager_Default::Intern (const String& s)
{
    lock_guard<mutex>   critSection (sCritSec_);
    auto i = sMap_->Lookup (s);
    if (i.IsPresent ()) {
        return *i;
    }
    AtomManager_Default::AtomInternalType v = sSeq_->GetLength ();
    sSeq_->Append (s);
    sMap_->Add (s, v);
    Ensure (Extract (v) == s);
    return v;
}

String  AtomManager_Default::Extract (AtomInternalType atomI)
{
    Require (atomI != kEmpty);
    return (*sSeq_)[atomI];
}
