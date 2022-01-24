/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_DefaultSerializer_h_
#define _Stroika_Foundation_DataExchange_DefaultSerializer_h_ 1

#include "../StroikaPreComp.h"

#include <cstdint>

#include "../Memory/BLOB.h"

/*
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */
namespace Stroika::Foundation::DataExchange {

    /**
     *  \brief function object which serializes type T to a BLOB (or BLOB like) object
     * 
     *  This is (at least originally) intended to be used by Cryptography::Hash<T>, but will probably be useful for many
     *  operations that need to treat a type T as a sequence of bytes
     *
     *  NOTE - not always defined - since we dont know how to serialze everthing - callers may need to specialize this
     *
     *  For now, this works with T:
     *      o   is_trivially_copy_constructible_v (e.g. int, float, uint32_t, etc)
     *      o   const char*
     *      o   std::string
     *      o   Memory::BLOB (just passed throgh, not adpated)
     * 
     *  ALSO, MANY other Stroika classes support this by providing their own specialization of DefaultSerializer<>
     *  For example:
     *      o   Characters::String
     *      o   IO::Network::InternetAddress
     *      ...
     * 
     *  \par Example Usage (specializing and USING DefaultSerializer)
     *      \code
     *          template <>
     *          struct DefaultSerializer<Stroika::Foundation::IO::Network::InternetAddress> {
     *              Memory::BLOB operator() (const Stroika::Foundation::IO::Network::InternetAddress& arg) const
     *              {
     *                  return DefaultSerializer<Characters::String>{}(arg.As<Characters::String> ());
     *              }
     *          };
     *      \endcode
     * 
     *  \note \em Design Note
     *      We chose to have this return Memory::BLOB instead of an abstract template type which was often
     *      Memory::BLOB but could be something more efficient, because I'm still supporting C++17 (no concepts)
     *      and I think Memory::BLOB can be made VERY NEARLY as efficient (by having a constructor which uses
     *      block allocation and pre-storages (SmallStackBuffer) the space for small objects (small strings and ints etc)
     *      (maybe do this performance tweak for Stroika 2.1b6?)
     * 
     *  @todo Consider using other tricks like serialize to JSON if the converters for that are defined, or with operator<< if those are defined.
     */
    template <typename T>
    struct DefaultSerializer {
        /**
         *  Convert the given t (of type T) to a Memory::BLOB; This should be as quick and efficient as practical.
         */
        Memory::BLOB operator() (const T& t) const;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "DefaultSerializer.inl"

#endif /*_Stroika_Foundation_DataExchange_DefaultSerializer_h_*/
