/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BinaryInputStream_h_
#define _Stroika_Foundation_Streams_BinaryInputStream_h_    1

#include    "../StroikaPreComp.h"

#include    "../Memory/Common.h"

#include    "InputStream.h"


/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   Either #include Memory/BLOB.h or document in the forward declare why not.
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



namespace Stroika { namespace Foundation { namespace Memory { class BLOB; } } }



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            using   Memory::Byte;

            //       template    <typename   BASESTREAM = InputStream<Byte>>
            //         using   BinaryInputStream = BASESTREAM;
#if 0
            /**
             @todo FIXUP DOCS - ALL OR MOSTYLY OBSOLETE
             *
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
             *      @see MemoryStream for most common stream applications.
             *
             *      @see ExternallyOwnedMemoryBinaryInputStream for a more efficient, but slightly less safe
             *          mapping to streams.
             *
             */
            template    <typename   BASESTREAM = InputStream<Byte>>
            class   BinaryInputStream : public BASESTREAM {
            private:
                using inherited = BASESTREAM;

            protected:
                using       _SharedIRep     =   typename BASESTREAM::_SharedIRep;

            protected:
                /**
                 * _SharedIRep arg - MAY also mixin Seekable - and if so - this automatically uses it.
                 */
                explicit BinaryInputStream (const _SharedIRep& rep);

            public:
                /**
                 *  defaults to null (empty ())
                 */
                BinaryInputStream () = default;
                BinaryInputStream (nullptr_t);
                BinaryInputStream (const InputStream<Byte>& from);

#if 0
            public:
                /**
                 *  Read from the current stream position until EOF, and accumulate all of it into a BLOB.
                 */
                nonvirtual  Memory::BLOB ReadAll () const;
#endif
            };
#endif


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
