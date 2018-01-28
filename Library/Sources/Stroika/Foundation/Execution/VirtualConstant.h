/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_VirtaulConstant_h_
#define _Stroika_Foundation_Execution_VirtaulConstant_h_ 1

#include "../StroikaPreComp.h"

/**
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 * TODO
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Execution {

            /**
             * Allow use of regular constant declaration use when we have
             * an underlying system where the constant is actually FETECHED from the argument function.
             *
             * Could use this in someplace like:
             *      const Execution::VirtaulConstant<Enumeration,&Private::kTypeHiddenFilter_>   kTypeHiddenFilter;
             *
             *  See https://stroika.atlassian.net/browse/STK-381 for details.
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
             *      VirtaulConstant<T,...> t;
             *          you must call t().m();
             */
            template <typename BASETYPE, const BASETYPE& (*VALUE_GETTER) ()>
            struct VirtaulConstant {
                operator const BASETYPE () const;
                const BASETYPE operator() () const;
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "VirtualConstant.inl"

#endif /*_Stroika_Foundation_Execution_VirtaulConstant_h_*/
