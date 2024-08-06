/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Common_h_
#define _Stroika_Foundation_Containers_Common_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <cstddef>
#include <cstdint>

namespace Stroika::Foundation::Containers {

    /**
     *  \brief For a contiguous container (such as a vector or basic_string) - find the pointer to the start of the container
     *
     *  For a contiguous container (such as a vector or basic_string) - find the pointer to the start of the container
     *
     *  Note: this is like std::begin(), except that it returns a pointer, not an iterator, and returns nullptr if the
     *  container is empty.
     */
    template <typename CONTAINER>
    typename CONTAINER::value_type* Start (CONTAINER& c);
    template <typename CONTAINER>
    const typename CONTAINER::value_type* Start (const CONTAINER& c);

    /**
     *  \brief For a contiguous container (such as a vector or basic_string) - find the pointer to the end of the container
     *
     *  For a contiguous container (such as a vector or basic_string) - find the pointer to the end of the container
     *
     *  Note: this is like std::end(), except that it returns a pointer, not an iterator, and returns nullptr if the
     *  container is empty.
     */
    template <typename CONTAINER>
    typename CONTAINER::value_type* End (CONTAINER& c);
    template <typename CONTAINER>
    const typename CONTAINER::value_type* End (const CONTAINER& c);

    /**
     *  \brief Mode flag to say if Adding to a container replaces, or if the first addition wins.
     * 
     *  \see also AddOrExtendOrReplaceMode
     */
    enum class AddReplaceMode {
        /**
         *  Associate argument value iff key argument missing
         */
        eAddIfMissing,

        /**
         *  Associate argument with value unconditionally
         */
        eAddReplaces
    };

    /**
     *  \brief Mode flag to say if Adding to a container replaces, or if the first addition wins (Logically AddOrExtendOrReplaceMode subclasses AddReplaceMode)
     * 
     *  \see also AddReplaceMode
     */
    enum class AddOrExtendOrReplaceMode {
        /**
         * same value as AddReplaceMode::eAddIfMissing
         */
        eAddIfMissing = AddReplaceMode::eAddIfMissing,

        /**
         * same value as AddReplaceMode::eAddReplaces
         */
        eAddReplaces = AddReplaceMode::eAddReplaces,

        /**
          * Used in multisets (where key can be associated with multiple values) - just add extra values.
          */
        eAddExtras,

        /**
          * if key already present, throw runtime exception
          */
        eDuplicatesRejected
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Common.inl"

#endif /*_Stroika_Foundation_Containers_Common_h_*/
