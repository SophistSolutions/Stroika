/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_StaticConstantMaker_h_
#define _Stroika_Foundation_Execution_StaticConstantMaker_h_ 1

#include "../StroikaPreComp.h"

#include <cstdint>

#include "../Configuration/Common.h"
#include "../Memory/Common.h"

/**
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 * TODO
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Execution {

            using Memory::Byte;

            /**
             * See http://bugzilla/show_bug.cgi?id=439
             *
             * Allow use of regular constant declaration use when we have
             * an underlying system where the constant is actually FETECHED from the argument function.
             *
             * Could use this in someplace like:
             *      const ModuleInit::ConstantViaGetter<Enumeration,&Private::kTypeHiddenFilter_>   kTypeHiddenFilter;
             *
             *  See http://bugzilla/show_bug.cgi?id=439 for details.
             *
             *  Note: it would be HIGHLY DESIRABLE if C++ allowed operator. overloading, as accessing one of these
             *  values without assinging to a temporary first - means that you cannot directly call its methods.
             *  Thats a bit awkward.
             *
             *  So if you have a type T, with method m(), and variable of type T t.
             *      Your starter code might be:
             *          T   t;
             *          t.m ();
             *  When you replace 'T t' with
             *      StaticConstantMaker<T,...> t;
             *          you must call t->m();
             */
            template <typename BASETYPE, const BASETYPE& (*VALUE_GETTER) ()>
            struct StaticConstantMaker {
                inline operator const BASETYPE () const
                {
                    return VALUE_GETTER ();
                }
                inline const BASETYPE operator() () const
                {
                    return VALUE_GETTER ();
                }
                inline const BASETYPE* operator-> () const
                {
                    return &(VALUE_GETTER ());
                }
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "StaticConstantMaker.inl"

#endif /*_Stroika_Foundation_Execution_StaticConstantMaker_h_*/
