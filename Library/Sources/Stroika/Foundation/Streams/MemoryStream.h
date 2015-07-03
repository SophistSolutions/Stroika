/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_MemoryStream_h_
#define _Stroika_Foundation_Streams_MemoryStream_h_  1

#include    "../StroikaPreComp.h"

#include    <mutex>
#include    <vector>

#include    "../Configuration/Common.h"
#include    "../Memory/BLOB.h"

#include    "InputOutputStream.h"
#include    "Seekable.h"



/*
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 *
 *      @todo   Once we redo BinaryStream/TextStream to be Stream<Byte>, Stream<Character>, we can redo this
 *              class to be fully templated and support BOTH.
 *
 *      @todo   This would be a good candidate class to rewrite using new Sequence_ChunkedArray
 *              class (when I implement it) based on Led chunked arrays).
 *
 *      @todo   explain about read/write at same time / threadsa, nd logic for shared SEEK OFFSET
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            using Memory::Byte; //tmphack til I templateize Memory steream

            /**
             *  \brief  Simplest to use BinaryInputOutputStream; MemoryStream can be written to, and then the BLOB of data retrieved.
             *
             *  BinaryInputOutputStream is threadsafe - meaning Read () or Write() can safely be called from
             *  multiple threads at a time freely, and the results are interleaved without corruption. There
             *  is no guarantee of ordering, but one will finish before the next starts writing (so if they
             *  are writing structured messages, those will remain in-tact).
             *
             *  MemoryStream is Seekable.
             *
             *  Since MemoryStream keeps its data all in memory, it has the limitation that
             *  attempts to seek or write more than will fit in RAM will fail (with an exception).
             *
             *  NB: This class COULD have been called MemoryStream, or MemoryInputOutputStream.
             */
            class   MemoryStream : public InputOutputStream<Byte> {
            private:
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
            public:
#endif
                class   Rep_;
            public:
                MemoryStream ();
                MemoryStream (const Memory::BLOB& blob);
                MemoryStream (const Byte* start, const Byte* end);

            public:
                /**
                 *  Convert the current contents of this MemoryStream into one of the "T" representations.
                 *  T can be one of:
                 *      Memory::BLOB
                 *      vector<Byte>
                 *      string
                 */
                template    <typename   T>
                nonvirtual  T   As () const;
            };


            template    <>
            Memory::BLOB    MemoryStream::As () const;
            template    <>
            vector<Byte>    MemoryStream::As () const;
            template    <>
            string          MemoryStream::As () const;


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "MemoryStream.inl"

#endif  /*_Stroika_Foundation_Streams_MemoryStream_h_*/
