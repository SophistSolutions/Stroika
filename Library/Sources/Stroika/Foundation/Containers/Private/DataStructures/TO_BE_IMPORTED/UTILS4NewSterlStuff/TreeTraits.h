#pragma once

#include "../../Shared/Headers/Comparer.h"
#include "../../Shared/Headers/KeyValue.h"



namespace TreeTraits {

    template    <typename KEY>
    struct  DefaultComp : public Comparer<KEY> {};

    template    <typename KEY, typename VALUE>
    struct  DefaultKeyValue : public KeyValue<KEY, VALUE> {};


    enum {
        eInvalidRemoveIgnored = 0x00,
        eInvalidRemoveThrowException = 0x01,
    };

    template    <typename KEYVALUE, typename COMP, int POLICY = eInvalidRemoveThrowException >
    struct  Traits {
        typedef  KEYVALUE   KeyValue;
        typedef  COMP       Comparer;
        static  const int   kPolicy = POLICY;   // bit field for now
    };
}
