/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_StaticConstantMaker_inl_
#define _Stroika_Foundation_Execution_StaticConstantMaker_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika {
    namespace Foundation {
        namespace Execution {

            /*
             ********************************************************************************
             ***************** StaticConstantMaker<BASETYPE, VALUE_GETTER> ******************
             ********************************************************************************
             */
            template <typename BASETYPE, const BASETYPE& (*VALUE_GETTER) ()>
            inline StaticConstantMaker<BASETYPE, VALUE_GETTER>::operator const BASETYPE () const
            {
                return VALUE_GETTER ();
            }
            template <typename BASETYPE, const BASETYPE& (*VALUE_GETTER) ()>
            inline const BASETYPE StaticConstantMaker<BASETYPE, VALUE_GETTER>::operator() () const
            {
                return VALUE_GETTER ();
            }
        }
    }
}
#endif /*_Stroika_Foundation_Execution_StaticConstantMaker_inl_*/
