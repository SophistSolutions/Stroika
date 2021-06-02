/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_Concrete_Private_String_BufferedStringRep_h_
#define _Stroika_Foundation_Characters_String_Concrete_Private_String_BufferedStringRep_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../String.h"

/**
 *  \file
 *
 *
 * TODO:
 *      @todo   Evaluate and document effacacy of qString_Private_BufferedStringRep_UseBlockAllocatedForSmallBufStrings in implementaiton.
 *
 *
 */

namespace Stroika::Foundation::Characters::Concrete::Private {

// Experimental block allocation scheme for strings. We COULD enahce this to have 2 block sizes - say 16 and 32 characters?
// But experiment with this a bit first, and see how it goes...
//      -- LGP 2011-12-04
#ifndef qString_Private_BufferedStringRep_UseBlockAllocatedForSmallBufStrings
#define qString_Private_BufferedStringRep_UseBlockAllocatedForSmallBufStrings qAllowBlockAllocation
#endif

    /**
     *  This is a utility class to implement most of the basic String::_IRep functionality.
     *  This implements functions that change the string, but don't GROW it,
     *  since we don't know in general we can (thats left to subtypes).
     *
     *  \note   This class always assure nul-terminated, and so 'capacity' always at least one greater than length.
     *
     *  @todo Explain queer wrapper class cuz protected
     *
     *  @todo fCapacity_ initialized to zero then corrected - confusing... - cleanup
     */
    struct BufferedStringRep : String {
        struct _Rep : public _IRep {
        private:
            using inherited = String::_IRep;

#if qString_Private_BufferedStringRep_UseBlockAllocatedForSmallBufStrings
        private:
            /*
             * magic 'automatic round-capacity-up-to' these (in wchar_ts) - and use block allocation for them...
             * 
             * getconf LEVEL1_DCACHE_LINESIZE; getconf LEVEL2_CACHE_LINESIZE
             * typically returns 64,64 so 16*2 is 32, and 16*4 (unix) is 64. So the blocks allocated will generally fit
             * in a cache line.
             * 
             * Maybe the 'roundup' numbers should differ between UNIX / Windows (2/4 byte wchar_t)?
             */
            static constexpr size_t kNElts1_ = 16;
            static constexpr size_t kNElts2_ = 32;
            static constexpr size_t kNElts3_ = 64;
#endif

        protected:
            _Rep ()            = delete;
            _Rep (const _Rep&) = delete;

        public:
            nonvirtual _Rep& operator= (const _Rep&) = delete;

        protected:
            /**
             *  The argument wchar_t* strings MAY or MAY NOT be nul-terminated
             */
            _Rep (const wchar_t* start, const wchar_t* end, size_t reserveExtraCharacters = 0);

        public:
            ~_Rep ();

        public:
            // \req srcStart < srcEnd
            nonvirtual void InsertAt (const Character* srcStart, const Character* srcEnd, size_t index);

        public:
            virtual const wchar_t* c_str_peek () const noexcept override;

        protected:
            //Presume fStart is really a WRITABLE pointer
            nonvirtual wchar_t* _PeekStart ();

            // @todo - SB private next few methods - except for use in ReserveAtLeast_()...
            // size() function defined only so we can use Containers::ReserveSpeedTweekAddN() template
        private:
            nonvirtual void SetLength_ (size_t newLength);

        public:
            nonvirtual size_t size () const;

        private:
            /**
             */
            static size_t ReserveAtLeastTargetCapacity_ (size_t targetNewSize, size_t existingCapacity);

        private:
            /**
             *  Capacity INCLUDES null char
             */
            nonvirtual void reserve_ (size_t newCapacity);

        private:
            static size_t AdjustCapacity_ (size_t initialCapacity);

        private:
            nonvirtual void ReserveAtLeast_ (size_t newCapacity);

        private:
            size_t fCapacity_; // includes INCLUDES nul/EOS char
        };
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "String_BufferedStringRep.inl"

#endif /*_Stroika_Foundation_Characters_String_Concrete_Private_String_BufferedStringRep_h_*/
