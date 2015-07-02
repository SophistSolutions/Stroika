/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_Stream_h_
#define _Stroika_Foundation_Streams_Stream_h_    1

#include    "../StroikaPreComp.h"

#include    <memory>

#include    "../Configuration/Common.h"
#include    "../Configuration/Enumeration.h"
#include    "../Memory/Common.h"
#include    "../Memory/Optional.h"




/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   Explain in docs how Stroika streams differ from iostream
 *              o   MUCH MUCH easier to implement your own complaint stream
 *              o   Separarate interface from implementation (thats why easier to implement)
 *              o   Doesn’t mix text with binary APIs. Keeps them logically separate (view as
 *                  you will – diff – if better or worse)
 *
 *
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            /**
             *  When seeking, you can see an offset from the start (typical/start) or from the end of the
             *  stream, or from the current position)
             */
            enum    class   Whence : uint8_t {
                eFromStart,
                eFromCurrent,
                eFromEnd,

                Stroika_Define_Enum_Bounds(eFromStart, eFromEnd)
            };


            /**
             *  SeekOffsetType is unsigned, normally, because for most purposes its zero based.
             *  @see SignedSeekOffsetType.
             */
            using       SeekOffsetType          =   uint64_t;


            /**
             *  SignedSeekOffsetType is signed variant of SeekOffsetType - used to specify a seek
             *  offset which can sometimes negative (backwards).
             *
             *  @see SignedSeekOffsetType.
             */
            using       SignedSeekOffsetType    =   int64_t;

            /**
             *  \brief  Stream is an 'abstract' class defining the interface...
             *
             * Design Overview:
             *
             *      o   ...
             *
             */
            template    <typename   ELEMENT_TYPE>
            class   Stream {
            protected:
            public:
                class   _IRep;

            public:
                using   ElementType = ELEMENT_TYPE;

            public:
                /**
                 *  defaults to null (empty ())
                 */
                Stream () = default;
                Stream (nullptr_t);

            protected:
                using       _SharedIRep     =   shared_ptr<_IRep>;

            protected:
                /**
                 * _SharedIRep arg - MAY also mixin Seekable - and if so - this automatically uses it.
                 */
                explicit Stream (const _SharedIRep& rep);

            public:
                /**
                 *  empty() doesn't check the data in the stream, but instead checks if the BinaryStream
                 *  smart pointer references any actual stream.
                 *
                 *  @see clear()
                 */
                nonvirtual  bool    empty () const;

            public:
                /**
                 *  clear () doesn't clear the data in the stream, but unreferences the Stream
                 *  smart pointer.
                 *
                 *  @see empty()
                 */
                nonvirtual  void    clear ();

            protected:
                /**
                 *
                 */
                nonvirtual  _SharedIRep _GetRep () const;

            public:
                /**
                 * \brief   Returns true iff this object was constructed with a seekable input stream rep.
                 *
                 *  Returns true iff this object was constructed with a seekable input stream rep. Note -
                 *  seekability cannot change over the lifetime of an object.
                 */
                nonvirtual  bool    IsSeekable () const;

            private:
                _SharedIRep fRep_;
                bool        fSeekable_;
            };


            /**
             *
             */
            template    <typename ELEMENT_TYPE>
            class   Stream<ELEMENT_TYPE>::_IRep {
            public:
                using   ElementType = ELEMENT_TYPE;

            public:
                _IRep () = default;
                _IRep (const _IRep&) = delete;

            public:
                virtual ~_IRep () = default;

            public:
                nonvirtual  _IRep& operator= (const _IRep&) = delete;

            public:
                // cannot change value - called once and value cached
                virtual bool    IsSeekable () const = 0;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Stream.inl"

#endif  /*_Stroika_Foundation_Streams_Stream_h_*/
