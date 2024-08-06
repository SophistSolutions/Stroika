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
        eAddIfMissing,
        eAddReplaces
    };

    /**
     *  \brief Mode flag to say if Adding to a container replaces, or if the first addition wins (Logically AddOrExtendOrReplaceMode subclasses AddReplaceMode)
     * 
     *  \see also AddReplaceMode
     */
    struct AddOrExtendOrReplaceMode {
    private:
        constexpr AddOrExtendOrReplaceMode (uint8_t v) noexcept;

    public:
        /**
         */
        constexpr AddOrExtendOrReplaceMode (AddReplaceMode v) noexcept;
        constexpr AddOrExtendOrReplaceMode (const AddOrExtendOrReplaceMode&) noexcept = default;

    public:
        /**
         * allows values to be used in switch statements...
         */
        constexpr operator uint8_t () const noexcept;

    public:
        /**
         * same value as AddReplaceMode::eAddIfMissing
         */
        static const AddOrExtendOrReplaceMode eAddIfMissing;
        /**
         * same value as AddReplaceMode::eAddReplaces
         */
        static const AddOrExtendOrReplaceMode eAddReplaces;

        /**
         */
        static const AddOrExtendOrReplaceMode eAddExtras;

        /**
         */
        static const AddOrExtendOrReplaceMode eDuplicatesRejected;

    private:
        uint8_t fValue_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Common.inl"

#endif /*_Stroika_Foundation_Containers_Common_h_*/
