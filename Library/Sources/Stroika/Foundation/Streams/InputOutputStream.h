/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InputOutputStream_h_
#define _Stroika_Foundation_Streams_InputOutputStream_h_    1

#include    "../StroikaPreComp.h"

#include    "InputStream.h"
#include    "OutputStream.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            /**
             *  \brief  Stream is an 'abstract' class defining the interface...
             *
             * Design Overview:
             *
             *      o   ...
             *
             */
            template    <typename   ELEMENT_TYPE>
            class   InputOutputStream : public InputStream<ELEMENT_TYPE>, public OutputStream<ELEMENT_TYPE> {
            protected:
                class   _IRep;

            protected:
                using       _SharedIRep     =   shared_ptr<_IRep>;

            public:
                using   ElementType = ELEMENT_TYPE;

            public:
                /**
                 *  defaults to null (empty ())
                 */
                InputOutputStream () = default;
                InputOutputStream (nullptr_t);

            protected:
                /**
                 * _SharedIRep arg - MAY also mixin Seekable - and if so - this automatically uses it.
                 */
                explicit InputOutputStream (const _SharedIRep& rep);

            protected:
                /**
                 *
                 */
                nonvirtual  _SharedIRep _GetRep () const;

          public:
                // @todo move to INL file and assert same as binaryoutputstream value
                bool    empty () const { return BinaryInputStream::empty (); }
            public:
                // @todo move to INL file and assert same as binaryoutputstream value
                bool    IsSeekable () const { return BinaryInputStream::IsSeekable (); }
            };


            /**
             *
             */
            template    <typename   ELEMENT_TYPE>
            class   InputOutputStream<ELEMENT_TYPE>::_IRep : public InputStream<ELEMENT_TYPE>::_IRep, public OutputStream<ELEMENT_TYPE>::_IRep {
            public:
                using   ElementType = ELEMENT_TYPE;

            public:
                _IRep () = default;
                _IRep (const _IRep&) = delete;

            public:
                nonvirtual  _IRep& operator= (const _IRep&) = delete;
			};


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "InputOutputStream.inl"

#endif  /*_Stroika_Foundation_Streams_InputOutputStream_h_*/
