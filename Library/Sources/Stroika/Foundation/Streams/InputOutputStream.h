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
             **** @todo NOTE - MAJOR API deparature - combined old BinaryInputOutputStream with binaryTiedStream - so now
             *   we COULD have two sides related as in MemoryBuffer/BasicInputOutputStream - write from one side read from the other, or
             * more unrelated - like socket stream - where write on one side read by other system (often other computer) but two still somehow
             * logically related. Very little though in relation between the two.


              *  \brief  BinaryInputOutputStream is a stream that acts much as a BinaryInputStream and an BinaryOutputStream.
              *
              *  BinaryInputOutputStream might have inherited from BinaryInputStream and
              *  BinaryOutputStream, but then it would have had two copies of the shared_ptr. It seemed
              *  more economical to have just one such base, and make it easy to convert (conversion operator).
              *
              *  A BinaryInputOutputStream is a *combined* input and output stream. That means they are
              *  somehow related. If the BinaryInputOutputStream is seekable, then the same seek offset
              *  applies to BOTH the input and output sides. If you wish to have separate seek offsets,
              *  maintain two separate input and output stream objects.
              *
              *  Note that this concept applies to a file, but NOT to a socket (tcp) stream. A TCP/stream
              *  really consists of two related (paired) streams going in opposite directions.
              *
              *  Note: @BufferedBinaryInputStream and @BufferedBinaryOutputStream may not work as expected with
              *  @BinaryInputOutputStream (and subclasses), because the buffering in individual streams may not get
              *  pushed through to the associated other direction stream. For most predictable buffering results
              *  use @BufferedBinaryInputOutputStream



              OLD OBSOLETE MOSLTY WRONG DOCS FROM BINARYTIEDSTREAM
             * @todo CLEANUP
              *
              *  \brief  BinaryTiedStreams are logically two closely related streams - one input and one output
              *
              *  A BinaryTiedStreams is really two separate streams - tied together into one object. The typical
              *  use-case for a BinaryTiedStream is a communications channel with another process. So for example,
              *  pipes, or a TCP-socket-stream (IO::Network::SocketStream) would be 'tied'.
              *
              *  BinaryTiedStreams are generally not seekable, but if you want to seek, you must first cast to
              *  the appropriate subobject (either the BinaryInputStream or BinaryOutputStream to get the right
              *  stream object, and then seek on that.
              *
              *  Note that this is significantly different from a BinaryInputOutputStream (which represents
              *  a single stream that can both be written two and read from).
              *
              *  The input and output streams MAY shared a common shared_ptr<> rep, but are not required to.
              *
              *  Note - this interpreation of the word "TIED" is perhaps misleading different from the meaning used in
              *  std::iostream!


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
