/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BinaryInputOutputStream_h_
#define _Stroika_Foundation_Streams_BinaryInputOutputStream_h_    1

#include    "../StroikaPreComp.h"

#include    <memory>

#include    "../Configuration/Common.h"

#include    "BinaryInputStream.h"
#include    "BinaryOutputStream.h"
#include    "InputOutputStream.h"


/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 *
 *  TODO:
 *      @todo
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
             */
            class   BinaryInputOutputStream /*: public Streams::BinaryStream*/

                // could do without inheritenace - not sure whats best - expierment
                : public BinaryInputStream, public BinaryOutputStream {
            protected:
                class   _IRep;
                using   _SharedIRep     =   shared_ptr<_IRep>;

            protected:
                /**
                @todo update docs about Seekable
                 *  _SharedIRep must NOT inherit from Seekable. However, the resulting BinaryInputOutputStream is always seekable.
                 *
                 *  \pre dynamic_cast(rep.get (), Seekable*) == nullptr
                 */
                explicit BinaryInputOutputStream (const _SharedIRep& rep);


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
#if 0
            public:
                /**
                 * Note - this returns a proxy object - you cannot use .get() to see that its the same object as the original object
                 */
                nonvirtual  operator BinaryInputStream () const;

            public:
                /**
                 * Note - this returns a proxy object - you cannot use .get() to see that its the same object as the original object
                 */
                nonvirtual  operator BinaryOutputStream () const;
#endif


            public:
                nonvirtual  SeekOffsetType  GetReadOffset () const { return BinaryInputStream::GetOffset (); }

            public:
                nonvirtual  SeekOffsetType  GetWriteOffset () const { return BinaryOutputStream::GetOffset (); }

            public:
                _DeprecatedFunction_ (nonvirtual  SeekOffsetType  ReadGetOffset () const { return BinaryInputStream::GetOffset (); }, "Instead use IsMissing() - to be removed after v2.0a97");

            public:
                _DeprecatedFunction_ (nonvirtual  SeekOffsetType  WriteGetOffset () const { return BinaryOutputStream::GetOffset (); }, "Instead use IsMissing() - to be removed after v2.0a97");

#if 0
            public:
                /**
                 *  Pointer must refer to valid memory at least bufSize long, and cannot be nullptr.
                 *  bufSize must always be >= 1. Returns 0 iff EOF, and otherwise number of bytes read.
                 *  BLOCKING until data is available, but can return with fewer bytes than bufSize
                 *  without prejudice about how much more is available.
                 *
                 *  \pre not empty()
                 */
                nonvirtual  size_t  Read (Byte* intoStart, Byte* intoEnd);

            public:
                /**
                 *  Pointer must refer to valid memory at least bufSize long, and cannot be nullptr.
                 *  BufSize may be zero.
                 *
                 *  Writes always succeed fully or throw.
                 *
                 *  \pre not empty()
                 */
                nonvirtual  void    Write (const Byte* start, const Byte* end);


            public:
                /**
                 *  @see BinaryOutputStream::Flush
                 */
                nonvirtual void    Flush ();

            public:
                /**
                 *  @see Seekable::GetOffset
                 */
                nonvirtual SeekOffsetType      ReadGetOffset () const;

            public:
                /**
                 *  @see Seekable:::Seek
                 */
                nonvirtual SeekOffsetType      ReadSeek (Whence whence, SignedSeekOffsetType offset);

            public:
                /**
                 *  @see Seekable::GetOffset
                 */
                nonvirtual SeekOffsetType      WriteGetOffset () const;

            public:
                /**
                 *  @see Seekable::Seek
                 */
                nonvirtual SeekOffsetType      WriteSeek (Whence whence, SignedSeekOffsetType offset);
#endif
            };


            /**
             *  This MUST NOT inherit from Seekable. It mimics parts of the interface (EXPLAIN WHY)
             */
            class   BinaryInputOutputStream::_IRep : public InputStream<Byte>::_IRep, public OutputStream<Byte>::_IRep { /*, public std::enable_shared_from_this<BinaryInputOutputStream::_IRep>*/
            public:
                _IRep ();
                _IRep (const _IRep&) = delete;

            public:
                nonvirtual  _IRep& operator= (const _IRep&) = delete;
#if 0
            public:
                /**
                 *  @see BinaryInputStream::Read
                 */
                virtual size_t  Read (Byte* intoStart, Byte* intoEnd)                                   =   0;


            public:
                /**
                 *  @see BinaryOutputStream::Write
                 */
                virtual void    Write (const Byte* start, const Byte* end)                              =   0;


            public:
                /**
                 *  @see BinaryOutputStream::Flush
                 */
                virtual void    Flush ()                                                                =   0;


            public:
                /**
                 *  @see Seekable::_IRep::GetOffset
                 */
                virtual SeekOffsetType      ReadGetOffset () const                                      =   0;


            public:
                /**
                 *  @see Seekable::_IRep::Seek
                 */
                virtual SeekOffsetType      ReadSeek (Whence whence, SignedSeekOffsetType offset)       =   0;


            public:
                /**
                 *  @see Seekable::_IRep::GetOffset
                 */
                virtual SeekOffsetType      WriteGetOffset () const                                     =   0;


            public:
                /**
                 *  @see Seekable::_IRep::Seek
                 */
                virtual SeekOffsetType      WriteSeek (Whence whence, SignedSeekOffsetType offset)      =   0;
#endif
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "BinaryInputOutputStream.inl"

#endif  /*_Stroika_Foundation_Streams_BinaryInputOutputStream_h_*/
