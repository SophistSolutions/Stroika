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
                 *  defaults to null (aka empty ())
                 */
                InputOutputStream () = default;
                InputOutputStream (nullptr_t);

            protected:
                /**
                 */
                explicit InputOutputStream (const _SharedIRep& rep);

            protected:
                /**
                 *
                 */
                nonvirtual  _SharedIRep _GetRep () const;

            public:
                /**
                 *
                 */
                nonvirtual  bool    empty () const;

            public:
                /**
                 *
                 */
                nonvirtual  bool    IsSeekable () const;

            public:
                nonvirtual  SeekOffsetType  GetReadOffset () const { return InputStream<ELEMENT_TYPE>::GetOffset (); }

            public:
                nonvirtual  SeekOffsetType  GetWriteOffset () const { return OutputStream<ELEMENT_TYPE>::GetOffset (); }
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
