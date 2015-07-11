/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_OutputStream_h_
#define _Stroika_Foundation_Streams_OutputStream_h_    1

#include    "../StroikaPreComp.h"

#include    <memory>

#include    "../Configuration/Common.h"
#include    "../Memory/Common.h"
#include    "../Memory/Optional.h"

#include    "Stream.h"



/**
 &* @TODO CELANUP ALL THESE DOCS
 *
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
*      @todo   Consider if Seek () past end of stream on writable stream should be
 *              allowed. Often - like in UNIX - this works - and you can then write there,
 *              and this creates a hole in teh file read back as zeros.
 *
 *              Its easier to DISALLOW this now, and then lift the restriction, and later allow it,
 *              so since I'm unsure, disallow for now. This can always be simulated with an extra
 *              zero write, and it assuming no seek past EOF makes implementations simpler, and
 *              definition more consistent (read).
 *
 *      @todo   Should add Close () method. Any subsequent calls to this stream - would fail?
 *
 *              (maybe close/flush ignored).
 *
 *              If we allow for that - we may need to have check method - isOpen?. So maybe best to
 *              have flush/close allowed, and anything else generate an assert error?
 *
 *      @todo   Add abiiliy to SetEOF (); You can SEEK, but if you seek backwards, and start writing - that doesnt change EOF. EOF
 *              remains fixed as max written to. DODUCMNET THIS (for text and binary) - and provide a SetEOF() method
 *              (maybe just for seekable streams)? Maybe add rule that SetEOF () can only go backwards (shorten). Then call
 *              PullBackEOF() or RestrictEOF() or RemovePast(); OR ResetEOFToCurrentPosiiton(). Later maybe best API.
 *
 *      @todo   Consider/document approaches to timeouts. We COULD have a stream class where
 *              it was a PROPERTY OF THE CLASS (or alternate API) where writes timeout after
 *              a certain point.
 *
 */



namespace Stroika { namespace Foundation { namespace Characters { class Character; } } }
namespace Stroika { namespace Foundation { namespace Characters { class String; } } }
namespace Stroika { namespace Foundation { namespace Memory { class BLOB; } } }



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            /**
             *  \brief  OutputStream<> is Smart pointer (with abstract Rep) class defining the interface to writing to
             *          a Stream sink of data.
             *
             * Design Overview:
             *
             *      o   @See Stream
             *
             *      o   InputStream and OutputStream may logically be mixed togehter to make an
             *          input/output stream: @see InputOutputStream<>
             *
             *      o   One (potential) slight design flaw with this API, is that its not possible to have legal partial writes.
             *          But not supporting partial writes makes use much simpler (since callers don't need
             *          to worry about that case), and its practically never useful. In principle - this API could be
             *          extended so that an exception (or extra method to ask about last write) could include information
             *          about partial writes, but for now - I don't see any reason.
             *
             *  Note - when you Seek() away from the end of an output stream, and then write, you automatically
             *  extend the stream to the point seeked to, and if you seek back (less) than the end and write, this overwrites
             *  instead of inserting.
             *
             *  Note - Write is sufficient to guarnatee the data is written, but it may be buffered until you call
             *  the destructor on the OutputStream (last reference goes away) or until you call Flush ().
             */
            template    <typename   ELEMENT_TYPE>
            class   OutputStream : public Stream<ELEMENT_TYPE> {
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
                OutputStream () = default;
                OutputStream (nullptr_t);

            protected:
                /**
                 * _SharedIRep arg - MAY also mixin Seekable - and if so - this automatically uses it.
                 */
                explicit OutputStream (const _SharedIRep& rep);

            public:
                /**
                 *      @todo - IF THIS MUST BE PUBLIC, then rename to not have _GetRep....
                 *
                 */
                nonvirtual  _SharedIRep _GetRep () const;

            public:
                /**
                 *     Create a synchonized (thread safe) copy of this stream. Note - this still refers to the same
                 *  underlying stream.
                 */
                nonvirtual  OutputStream<ELEMENT_TYPE>  Synchronized () const;

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
                 *  Write the bytes bounded by start and end. Start and End maybe equal, and only
                 *  then can they be nullptr.
                 *
                 *  Writes always succeed fully or throw (no partial writes).
                 */
                nonvirtual  void    Write (const ElementType* start, const ElementType* end) const;
                template    <typename TEST_TYPE = ELEMENT_TYPE, typename ENABLE_IF_TEST = typename enable_if <is_same<TEST_TYPE, Memory::Byte>::value>::type>
                nonvirtual  void    Write (const Memory::BLOB& blob) const;
                template    <typename TEST_TYPE = ELEMENT_TYPE, typename ENABLE_IF_TEST = typename enable_if <is_same<TEST_TYPE, Characters::Character>::value>::type>
                nonvirtual  void    Write (const wchar_t* start, const wchar_t* end) const;
                template    <typename TEST_TYPE = ELEMENT_TYPE, typename ENABLE_IF_TEST = typename enable_if <is_same<TEST_TYPE, Characters::Character>::value>::type>
                nonvirtual  void    Write (const wchar_t* cStr) const;
                template    <typename TEST_TYPE = ELEMENT_TYPE, typename ENABLE_IF_TEST = typename enable_if <is_same<TEST_TYPE, Characters::Character>::value>::type>
                nonvirtual  void    Write (const Characters::String& s) const;

            public:
                /**
                 *  \brief forces any data contained in this stream to be written.
                 *
                 *  Forces any data contained in this stream to be written.
                 *
                 *  For some streams (such as buffered streams, sockets, and OpenSSLCryptStream) - they may not
                 *  finish their writes until they are destroyed. The trouble then - is that they cannot
                 *  propagate exceptions! Calling Flush() before destroying the output stream allows exceptions
                 *  to be propagated properly.
                 */
                nonvirtual  void    Flush () const;

            public:
                /**
                 * EXPERIEMNTAL API
                 * done as template so third parties can externally extend, and have overloading work right..
                 * @todo need overloads for basic types, std::string, int, float, etc...
                 * But dont do except for string for now. Dont make same mistake as iostream - with formatting. Not clear how todo
                 * right so dont dig a hole and do it wrong (yet).
                 */
                template    <typename T, typename TEST_TYPE = ELEMENT_TYPE, typename ENABLE_IF_TEST = typename enable_if <is_same<TEST_TYPE, Characters::Character>::value>::type>
                const OutputStream<ELEMENT_TYPE>&     operator<< (T write2TextStream) const;
            };


            template    <>
            template    <>
            void    OutputStream<Characters::Character>::Write (const Characters::String& s) const;
            template    <>
            template    <>
            void    OutputStream<Characters::Character>::Write (const wchar_t* start, const wchar_t* end) const;


            template    <>
            template    <>
            const OutputStream<Characters::Character>& OutputStream<Characters::Character>::operator<< (const Characters::String& write2TextStream) const;
            template    <>
            template    <>
            const OutputStream<Characters::Character>& OutputStream<Characters::Character>::operator<< (const wchar_t* write2TextStream) const;


            /**
             *
             */
            template    <typename   ELEMENT_TYPE>
            class   OutputStream<ELEMENT_TYPE>::_IRep : public Stream<ELEMENT_TYPE>::_IRep  {
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
                virtual SeekOffsetType      GetWriteOffset () const                                     =   0;

            public:
                virtual SeekOffsetType      SeekWrite (Whence whence, SignedSeekOffsetType offset)      =   0;

            public:
                /**
                 *  Pointer must refer to valid memory at least bufSize long, and cannot be nullptr.
                 *  BufSize must always be >= 1.
                 *  Writes always succeed fully or throw (no partial writes so no return value of amount written)
                 */
                virtual void    Write (const ElementType* start, const ElementType* end)         =   0;

            public:
                /**
                 *
                 */
                virtual void    Flush ()         =   0;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "OutputStream.inl"

#endif  /*_Stroika_Foundation_Streams_OutputStream_h_*/
