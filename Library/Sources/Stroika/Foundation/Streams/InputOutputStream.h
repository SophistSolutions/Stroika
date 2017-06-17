/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_InputOutputStream_h_
#define _Stroika_Foundation_Streams_InputOutputStream_h_ 1

#include "../StroikaPreComp.h"

#include "InputStream.h"
#include "OutputStream.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   Need InputOutputStream<ELEMENT_TYPE>::Synchronized() like InpoutStream<>::Synconbized.
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Streams {

            /**
             *  \brief  InputOutputStream is single stream object that acts much as a InputStream::Ptr and an OutputStream.
             *
             *  \note   Design Note:
             *      There are two principle cases of combining input and output streams into an InputOutputStream:
             *          1>  where there the sequences of data are unrelated
             *          2>  where writes to the input side of the stream 'appears' in the 'output' side of the stream.
             *
             *      Stroika used to have two kinds of input output streams - BinaryTiedStream, and BinaryInputOutputStream.
             *      However, that distinction was abandoned in v2.0a97, since there appeared to be no reason to capture this distinction
             *      (we may want to revisit, but thats the plan for now - LGP 2017-01-28)
             *
             *      An InputOutputStream<> could be either one of these two cases - where writes to one side appear on reads
             *      of the othe rside (such as @see MemoryStream), or where they are unrelated, such as with
             *      @see SocketStream.
             *
             *  \note   Design Note:
             *      InputOutputStream inherits from InputStream::Ptr and OutputStream, so it has two copies of the shared_ptr, even though
             *      there is only one underlying 'rep' object.
             *
             *  \note   Design Note:
             *      InputOutputStream need not have the same value for IsSeekable () - but if you call InputOutputStream::IsSeekable () - that
             *      method requires they both be the same.
             *
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-Plus-May-Need-To-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-Plus-May-Need-To-Externally-Synchronize-Letter</a>
             */
            template <typename ELEMENT_TYPE>
            class InputOutputStream : public InputStream<ELEMENT_TYPE>::Ptr, public OutputStream<ELEMENT_TYPE>::Ptr {
            protected:
                class _IRep;

            protected:
                using _SharedIRep = shared_ptr<_IRep>;

            public:
                using ElementType = ELEMENT_TYPE;

            public:
                /**
                 *  defaults to null (aka empty ())
                 */
                InputOutputStream () = default;
                InputOutputStream (nullptr_t);

            protected:
                /**
                 * _SharedIRep rep is the underlying shared output Stream object.
                 *
                 *  \req rep != nullptr (use nullptr_t constructor)
                 */
                explicit InputOutputStream (const _SharedIRep& rep);

            protected:
                /**
                 *  \brief protected access to underlying stream smart pointer
                 */
                nonvirtual _SharedIRep _GetSharedRep () const;

            protected:
                /**
                 * \req *this != nullptr
                 */
                nonvirtual const _IRep& _GetRepConstRef () const;

            protected:
                /**
                 * \req *this != nullptr
                 */
                nonvirtual _IRep& _GetRepRWRef () const;

            public:
                /**
                 *
                 */
                nonvirtual bool empty () const;

            public:
                /**
                 *      \req InputStream<ELEMENT_TYPE>::Ptr::IsSeekable () == OutputStream<ELEMENT_TYPE>::IsSeekable ()
                 */
                nonvirtual bool IsSeekable () const;

            public:
                /**
                 *
                 */
                nonvirtual SeekOffsetType GetReadOffset () const;

            public:
                /**
                 *
                 */
                nonvirtual SeekOffsetType GetWriteOffset () const;

            public:
                /**
                 *
                 */
                nonvirtual SeekOffsetType SeekWrite (SignedSeekOffsetType offset) const;
                nonvirtual SeekOffsetType SeekWrite (Whence whence, SignedSeekOffsetType offset) const;

            public:
                /**
                 *
                 */
                nonvirtual SeekOffsetType SeekRead (SignedSeekOffsetType offset) const;
                nonvirtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) const;
            };

            /**
             *
             */
            template <typename ELEMENT_TYPE>
            class InputOutputStream<ELEMENT_TYPE>::_IRep : public InputStream<ELEMENT_TYPE>::_IRep, public OutputStream<ELEMENT_TYPE>::_IRep {
            public:
                using ElementType = ELEMENT_TYPE;

            public:
                _IRep ()             = default;
                _IRep (const _IRep&) = delete;

            public:
                nonvirtual _IRep& operator= (const _IRep&) = delete;
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InputOutputStream.inl"

#endif /*_Stroika_Foundation_Streams_InputOutputStream_h_*/
