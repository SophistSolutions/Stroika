/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BinaryInputStream_h_
#define _Stroika_Foundation_Streams_BinaryInputStream_h_    1

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
 *      @todo   Either #include Memory/BLOB.h or document in the forward declare why not.
 *
 *      @todo   Explain in docs how Stroika streams differ from iostream
 *              o   MUCH MUCH easier to implement your own complaint stream
 *              o   Separarate interface from implementation (thats why easier to implement)
 *              o   Doesn’t mix text with binary APIs. Keeps them logically separate (view as
 *                  you will – diff – if better or worse)
 *
 *      @todo   Redo BinaryInputStream::ReadAll() more efficiently checking isSeekable() and the
 *              precomputing size and doing just one read.
 *
 *      @todo   Perhaps add a GetBytesAvailable() method. This is effectively like converting
 *              blocking to safe read calls...
 *
 *      @todo   Consider adding GetLength() - as an alias for:
 *                  oldOffset o = GetOffset();
 *                  l =  Seek (eFromEnd, 0);
 *                  Seek (eFromStart, o);
 *                  return l;
 *              The reason to add this - as a virtual function - is that it can be more efficient and
 *              semantically pure (multithreading). Do here - not in Seekable - cuz only sensible for
 *              read streams, not write streams. Well defined. Issue is - probably REQUIRE Seekable()
 *              for this. But its possibly sensible even for non-seekable streams???
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


            using   Memory::Byte;


            class   BinaryInputOutputStream;


            /**
             *  \brief  BinaryInputStream is an 'abstract' class defining the interface to reading from
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
             *      @see BasicBinaryInputStream for most common stream applications.
             *
             *      @see ExternallyOwnedMemoryBinaryInputStream for a more efficient, but slightly less safe
             *          mapping to streams.
             *
             */
            class   BinaryInputStream : public BinaryStream {
            protected:
            public:
                class   _IRep;

            public:
                /**
                 *  defaults to null (empty ())
                 */
                BinaryInputStream ();
                BinaryInputStream (nullptr_t);

            protected:
                using       _SharedIRep     =   shared_ptr<_IRep>;

            protected:
                /**
                 * _SharedIRep arg - MAY also mixin Seekable - and if so - this automatically uses it.
                 */
                explicit BinaryInputStream (const _SharedIRep& rep);
                explicit BinaryInputStream (const _SharedIRep& rep, Seekable::_IRep* seekable);

            protected:
                /**
                 *
                 */
                nonvirtual  _SharedIRep _GetRep () const;

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
                 */
                nonvirtual  Memory::Optional<Byte>  Read () const;
                nonvirtual  size_t  Read (Byte* intoStart, Byte* intoEnd) const;

            public:
                /**
                 *  Read from the current stream position until EOF, and accumulate all of it into a BLOB.
                 */
                nonvirtual  Memory::BLOB ReadAll () const;

            private:
                friend  class   BinaryInputOutputStream;
            };


            /**
             *
             */
            class   BinaryInputStream::_IRep : public virtual BinaryStream::_IRep {
            public:
                _IRep ();
                _IRep (const _IRep&) = delete;

            public:
                nonvirtual  _IRep& operator= (const _IRep&) = delete;


            public:
                /**
                 *  Pointer must refer to valid memory at least bufSize long, and cannot be nullptr.
                 *  bufSize must always be >= 1. Returns 0 iff EOF, and otherwise number of bytes read.
                 *  BLOCKING until data is available, but can return with fewer bytes than bufSize
                 *  without prejudice about how much more is available.
                 */
                virtual size_t  Read (Byte* intoStart, Byte* intoEnd)          =   0;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "BinaryInputStream.inl"

#endif  /*_Stroika_Foundation_Streams_BinaryInputStream_h_*/
