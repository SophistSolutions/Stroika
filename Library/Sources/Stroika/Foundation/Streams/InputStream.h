/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InputStream_h_
#define _Stroika_Foundation_Streams_InputStream_h_    1

#include    "../StroikaPreComp.h"

#include    <memory>

#include    "../Configuration/Common.h"
#include    "../Memory/Common.h"
#include    "../Memory/Optional.h"

#include    "Stream.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   Bake Read (SeekOffsetType* offset, Byte* intoStart, Byte* intoEnd) into REP
 *              API so its operation can be atomic.
 *
 *      @todo   Consider making GetOffsetToEndOfStream () a virtual part of rep so it can work with the locks
 *              and be safely atomic.
 */



namespace   Stroika {
    namespace   Foundation {
        namespace Memory {
            class BLOB;
        }
    }
}


namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            /**
             *  @todo CLEANUP - VERY ROUGH OLD DOCS DRAFT
             *
             *  \brief  InputStream<> is an 'abstract' class defining the interface to reading from
             *          a binary source of data.
             *
             * Design Overview:
             *
             *      o   See BinaryStream for details about memory management, but the basic idea is
             *          that BinaryInputStream acts as a smart-pointer to an abstract
             *          BinaryInputStream::_IRep.
             *
             *      o   All read's on a BinaryInputStream are BLOCKING. If there is a desire to have a
             *          non-blocking read, then create a new mixin interface and through that interface
             *          you can do non-blocking reads, but this Read() method must always block.
             *
             *      o   EOF is handled by a return value of zero. Once EOF is returned - any subsequent
             *          calls to Read () will return EOF (unless some other mechanism is used to tweak
             *          the state of the object, like a mixed in Seekable class and calling SEEK).
             *
             *      o   Exceptions indicate something went wrong - like an IO error, or possibly in some cases
             *          a formatting effort (e.g. if the source is encrypted, and the stream is decrypting,
             *          then it might detect a format error and throw).
             *
             *      o   BinaryInputStream and BinaryOutputStream CAN be logically be mixed togehter to make an
             *          input/output stream in one of two ways:
             *              @see BinaryInputOutputStream
             *              @see BinaryTiedStreams
             *
             *      @see Stroika::Foundation::Streams::iostream for adapters to work with std::iostream.
             *
             *      @see MemoryStream for most common stream applications.
             *
             *      @see ExternallyOwnedMemoryBinaryInputStream for a more efficient, but slightly less safe
             *          mapping to streams.
             */
            template    <typename   ELEMENT_TYPE>
            class   InputStream : public Stream<ELEMENT_TYPE> {
            private:
                using   inherited = Stream<ELEMENT_TYPE>;

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
                InputStream () = default;
                InputStream (nullptr_t);

            protected:
                /**
                 * _SharedIRep arg - MAY also mixin Seekable - and if so - this automatically uses it.
                 */
                explicit InputStream (const _SharedIRep& rep);

            public:
                /**
                 *      @todo - IF THIS MUST BE PUBLIC, then rename to not have _GetRep....
                 *
                 */
                nonvirtual  _SharedIRep _GetRep () const;

            public:
                /**
                 * GetOffset () returns the currently seeked offset. This is the same as Seek (eFromCurrent, 0).
                 */
                nonvirtual  SeekOffsetType  GetOffset () const;

            public:
                /**
                 * GetOffsetToEndOfStream () returns the the distance from the current offset position to the end of the stream.
                 *  This is equivalent to:
                 *      size_t  size =  Seek (Whence::eFromEnd, 0);
                 *      Seek (Whence::eFromStart, savedReadFrom);
                 *(EXCPET MAYBE GUARNATEED ATOMIC????)
                 */
                nonvirtual  SeekOffsetType  GetOffsetToEndOfStream () const;

            public:
                /**
                 * The new position, measured in bytes, is obtained by adding offset bytes to the position
                 *  specified by whence.
                 *
                 *  Seek () past the end of stream is NOT legal (may reconsider).
                 *
                 *  Seek () returns the new resulting position (measured from the start of the stream - same as GetOffset).
                 */
                nonvirtual  SeekOffsetType  Seek (SignedSeekOffsetType offset) const;
                nonvirtual  SeekOffsetType  Seek (Whence whence, SignedSeekOffsetType offset) const;

            public:
                /**
                 *  Read/0
                 *      return IsMissing() on EOF.
                 *
                 *  Read/2
                 *      Pointer must refer to valid memory at least bufSize long, and cannot be nullptr.
                 *      bufSize must always be >= 1. Returns 0 iff EOF, and otherwise number of bytes read.
                 *      BLOCKING until data is available, but can return with fewer bytes than bufSize
                 *      without prejudice about how much more is available.
                 *
                 *  Read/3
                 *      TRIAL BALLOON IDEA FROM STERL - NYI.
                 *
                 *      \req IsSeekable()
                 *
                 *      Would be LIKE Read/2 () - except NOT update any intrinsic seek offset. Input offset
                 *      would be where we read from, and would be updated to reflect where we read to. Change in
                 *      offset would be the same as the returned value.
                 */
                nonvirtual  Memory::Optional<ElementType>  Read () const;
                nonvirtual  size_t  Read (ElementType* intoStart, ElementType* intoEnd) const;
                nonvirtual  size_t  Read (SeekOffsetType* offset, ElementType* intoStart, ElementType* intoEnd) const;
            };


            /**
             *
             */
            template    <typename   ELEMENT_TYPE>
            class   InputStream<ELEMENT_TYPE>::_IRep : public Stream<ELEMENT_TYPE>::_IRep  {
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
                virtual SeekOffsetType      GetReadOffset () const                                     =   0;

            public:
                virtual SeekOffsetType      SeekRead (Whence whence, SignedSeekOffsetType offset)      =   0;

            public:
                /**
                 *  Pointer must refer to valid memory at least bufSize long, and cannot be nullptr.
                 *  bufSize must always be >= 1. Returns 0 iff EOF, and otherwise number of bytes read.
                 *  BLOCKING until data is available, but can return with fewer bytes than bufSize
                 *  without prejudice about how much more is available.
                 *
                 *  If offset is nullptr, then read uses builtin seek offset, and updates it (normal case).
                 *
                 *  If offset is non-null, treat as seek to that offset, and update that paraemter offset, but not real underlying read.
                 *  this requires seekability (assert otherwise).
                 */
                virtual size_t  Read (SeekOffsetType* offset, ElementType* intoStart, ElementType* intoEnd)          =   0;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "InputStream.inl"

#endif  /*_Stroika_Foundation_Streams_InputStream_h_*/
