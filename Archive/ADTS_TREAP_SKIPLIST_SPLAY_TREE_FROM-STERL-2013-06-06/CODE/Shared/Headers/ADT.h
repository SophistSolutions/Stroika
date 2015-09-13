#pragma once

#include "../../Shared/Headers/Comparer.h"
#include "../../Shared/Headers/Exception.h"
#include "../../Shared/Headers/KeyValue.h"



namespace ADT {

    template    <typename KEY>
	struct	DefaultComp : public Comparer<KEY> {};

    template    <typename KEY, typename VALUE>
	struct	DefaultKeyValue : public KeyValue<KEY,VALUE> {};


	enum {
	    eDefaultPolicy              = 0x0000,
		eInvalidRemoveIgnored       = 0x0001,
		eDuplicateAddThrowException = 0x0010,
	};

    template    <typename KEYVALUE, typename COMP, int POLICY = eDefaultPolicy >
	struct	Traits {
		typedef  KEYVALUE	KeyValue;
		typedef	 COMP		Comparer;
		static	const int	kPolicy = POLICY;	// bit field for now
	};
}

class   DuplicateAddException : public Exception {
    public:
        DuplicateAddException () :
            Exception ("Addition of duplicate key not allowed")
        {
        }

        DuplicateAddException (const std::string& key) :
            Exception ("Attempt to add duplicate element '" + key + "'")
        {
        }
};

class   InvalidRemovalException : public Exception {
    public:
        InvalidRemovalException () :
            Exception ("Attempt to remove element not in container")
        {
        }

        InvalidRemovalException (const std::string& key) :
            Exception ("Attempt to remove element '" + key + "' not in container")
        {
        }
};
