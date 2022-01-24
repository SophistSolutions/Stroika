/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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
            using TextSpan = pair<const wchar_t*, const wchar_t*>;
            _Rep (const tuple<const wchar_t*, const wchar_t*, size_t>& strAndCapacity);
            _Rep (const TextSpan& t1);
            _Rep (const TextSpan& t1, const TextSpan& t2);
            _Rep (const TextSpan& t1, const TextSpan& t2, const TextSpan& t3);

        public:
            ~_Rep ();

        private:
            static pair<wchar_t*, wchar_t*>                      mkBuf_ (size_t length);
            static tuple<const wchar_t*, const wchar_t*, size_t> mkBuf_ (const TextSpan& t1);
            static tuple<const wchar_t*, const wchar_t*, size_t> mkBuf_ (const TextSpan& t1, const TextSpan& t2);
            static tuple<const wchar_t*, const wchar_t*, size_t> mkBuf_ (const TextSpan& t1, const TextSpan& t2, const TextSpan& t3);

        public:
            virtual const wchar_t* c_str_peek () const noexcept override;

        public:
            virtual size_t size () const override;

        private:
            static size_t AdjustCapacity_ (size_t initialCapacity);

        private:
            // includes INCLUDES nul/EOS char
            // @todo add constexpr config var here, and play with it: COULD have recomputed this from the length in the DTOR (only place its used) - but that recompute could be moderately expensive, so just
            // save it (perhaps reconsider as CPU faster and memory bandwidth more limiting)
            size_t fCapacity_;
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
