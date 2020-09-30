/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_DefaultSerializer_h_
#define _Stroika_Foundation_Cryptography_DefaultSerializer_h_ 1

#include "../StroikaPreComp.h"

#include <cstdint>

#include "../Characters/String.h"
#include "../Memory/BLOB.h"

/*
 *  \version    <a href="Code-Status.md#Alpha">Alpha</a>
 *
 */

namespace Stroika::Foundation::Cryptography {

    /**
     *  \brief function object which serializes type T to a BLOB (or BLOB like) object
     * 
     *  THis is (at least originally) intended to be used by Hash<T>, but will probably be useful for many
     *  operations that need to treat a type T as a sequence of bytes
     *
     *  NOTE - not always defined - since we dont know how to serialze everthing - callers may need to specialize this
     *
     *  For now, this works with TYPE_TO_COMPUTE_HASH_OF:
     *      o   is_arithmetic (e.g. int, float, uint32_t, etc)
     *      o   const char*
     *      o   std::string
     *      o   String (or anything promotable to string)
     *      o   Memory::BLOB (just passed throgh, not adpated)
     * ((PRELIMINARY - WROKING ON DEFINING THIS))
     * 
     *  @todo Consider moving this to 'DataExchange' or 'Memory'? and more generally defining somehow - using other tricks like 
     *        serialize to JSON if the converters for that are defined, or with operator<< if those are defined.
     */
    template <typename T>
    struct DefaultSerializer {
        // for now produce a BLOB, but later produce an object which can copy if is_trivially_copyable into another T copy
        // and return direct pointers to start and end
        Memory::BLOB operator() (const T& t) const;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "DefaultSerializer.inl"

#endif /*_Stroika_Foundation_Cryptography_DefaultSerializer_h_*/
