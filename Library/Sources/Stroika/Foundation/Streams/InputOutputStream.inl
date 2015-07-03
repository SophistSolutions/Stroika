/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InputOutputStream_inl_
#define _Stroika_Foundation_Streams_InputOutputStream_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            /*
             ********************************************************************************
             ********************** InputOutputStream<ELEMENT_TYPE> *************************
             ********************************************************************************
             */
            template    <typename ELEMENT_TYPE>
            inline  InputOutputStream<ELEMENT_TYPE>::InputOutputStream (const _SharedIRep& rep)
                : InputStream<ELEMENT_TYPE> (rep)
                , OutputStream<ELEMENT_TYPE> (rep)
            {
            }
            template    <typename ELEMENT_TYPE>
            inline  InputOutputStream<ELEMENT_TYPE>::InputOutputStream (nullptr_t)
                : InputStream<ELEMENT_TYPE> (nullptr)
                , OutputStream<ELEMENT_TYPE> (nullptr)
            {
            }
            template    <typename ELEMENT_TYPE>
            inline  auto    InputOutputStream<ELEMENT_TYPE>::_GetRep () const -> _SharedIRep
            {
                Ensure (dynamic_pointer_cast<_IRep> (InputStream<ELEMENT_TYPE>::_GetRep ()) == dynamic_pointer_cast<_IRep> (OutputStream<ELEMENT_TYPE>::_GetRep ()))
                return dynamic_pointer_cast<_IRep> (InputStream<ELEMENT_TYPE>::_GetRep ());
            }
            template    <typename ELEMENT_TYPE>
            inline  bool    InputOutputStream<ELEMENT_TYPE>::empty () const
            {
                Ensure (InputStream<ELEMENT_TYPE>::empty () == OutputStream<ELEMENT_TYPE>::empty ());
                return InputStream<ELEMENT_TYPE>::empty ();
            }
            template    <typename ELEMENT_TYPE>
            inline  bool    InputOutputStream<ELEMENT_TYPE>::IsSeekable () const
            {
                return InputStream<ELEMENT_TYPE>::IsSeekable ();
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Streams_InputOutputStream_inl_*/
