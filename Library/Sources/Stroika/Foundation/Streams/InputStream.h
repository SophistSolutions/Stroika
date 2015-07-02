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

#include    "BinaryStream.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo
 *
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
             *  \brief  Stream is an 'abstract' class defining the interface...
             *
             * Design Overview:
             *
             *      o   ...
             *
             */
            template    <typename   ELEMENT_TYPE>
            class   InputStream : public Stream<ELEMENT_TYPE> {
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

            protected:
                /**
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
