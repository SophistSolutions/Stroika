/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_SharedMemoryStream_h_
#define _Stroika_Foundation_Streams_SharedMemoryStream_h_ 1

#include "../StroikaPreComp.h"

#include <vector>

#include "../Characters/String.h"
#include "../Configuration/Common.h"
#include "../Memory/BLOB.h"

#include "InputOutputStream.h"

/*
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 *  TODO:
 *      @todo https://stroika.atlassian.net/browse/STK-584 - 
 *            provide option for SharedMemoryStream so not seekable - less memory
 */

namespace Stroika {
    namespace Foundation {
        namespace Streams {

            /**
             *  \brief  SharedMemoryStream<> is an InputOutputStream<> like MemoryStream<> but supporting concurrency
             *
             *  SharedMemoryStream is Seekable.
             *
             *  Since SharedMemoryStream keeps its data all in memory, it has the limitation that
             *  attempts to seek or write more than will fit in RAM will fail (with an exception).
             *
             *  \note   SharedMemoryStream is suitable for synchonized reading and writing between two threads (producer / consumer pattern).
             *          Reads will block at the end of the stream until some thread calls SharedMemoryStream><>::CloseForWrites ()
             *
             *          @see MemoryStream
             *
             *  \note currently all SharedMemoryStreams are seekable, but future versions may allow options to be specified
             *        so they are not seekable
             *
             *  @see ExternallyOwnedMemoryInputStream
             *
             *  \par Example Usage
             *      \code
             *           SharedMemoryStream<unsigned int> pipe;
             *           unsigned                         sum{};
             *           static constexpr unsigned int    kStartWith{1};
             *           static constexpr unsigned int    kUpToInclusive_{1000};
             *           Thread                           consumer{[&]() {
             *                  while (auto o = pipe.Read ()) {
             *                      sum += *o;
             *                  }
             *              },
             *              Thread::eAutoStart};
             *           Thread producer{[&]() {
             *                  for (unsigned int i = kStartWith; i <= kUpToInclusive_; i++) {
             *                      pipe.Write (i);
             *                  };
             *                  pipe.CloseForWrites ();    // critical or consumer hangs on final read
             *              },
             *              Thread::eAutoStart};
             *           Thread::WaitForDone ({consumer, producer});
             *           Assert (sum == (1 + kUpToInclusive_) * (kUpToInclusive_ - 1 + 1) / 2); // not a race
             *      \endcode
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
             *          but the internal stream is internally synchonized.
             */
            template <typename ELEMENT_TYPE>
            class SharedMemoryStream : public InputOutputStream<ELEMENT_TYPE>::Ptr {
            private:
                using inherited = typename InputOutputStream<ELEMENT_TYPE>::Ptr;

            private:
                class Rep_;

            public:
                /**
                 *  To copy a SharedMemoryStream, use SharedMemoryStream<T>::Ptr
                 */
                SharedMemoryStream ();
                SharedMemoryStream (const SharedMemoryStream&) = delete;
                SharedMemoryStream (const ELEMENT_TYPE* start, const ELEMENT_TYPE* end);
                template <typename TEST_TYPE = ELEMENT_TYPE, typename ENABLE_IF_TEST = typename enable_if<is_same<TEST_TYPE, Memory::Byte>::value>::type>
                SharedMemoryStream (const Memory::BLOB& blob);

            public:
                /**
                 *  Use SharedMemoryStream<T>::Ptr
                 */
                nonvirtual SharedMemoryStream& operator= (const SharedMemoryStream&) = delete;

            public:
                class Ptr;

            public:
                /**
                 *  Any subsequent writes or SeekWrite() calls are a bug/caller error, though its common to continue reading.
                 *
                 *  \note Since its illegal to destory a SharedMemoryStream while there are pending reads (or writes for that matter),
                 *        it is typically Required to call this before destroying a SharedMemoryStream.
                 */
                nonvirtual void CloseForWrites ();

            public:
                /**
                 *  Convert the current contents of this SharedMemoryStream into one of the "T" representations.
                 *  T can be one of:
                 *      o   vector<ElementType>
                 *
                 *  And if ElementType is Memory::Byte, then T can also be one of:
                 *      o   Memory::BLOB
                 *      o   string
                 *
                 *  And if ElementType is Characters::Character, then T can also be one of:
                 *      o   String
                 */
                template <typename T>
                nonvirtual T As () const;
            };

            template <>
            template <>
            Memory::BLOB SharedMemoryStream<Memory::Byte>::As () const;
            template <>
            template <>
            string SharedMemoryStream<Memory::Byte>::As () const;
            template <>
            template <>
            vector<Memory::Byte> SharedMemoryStream<Memory::Byte>::As () const;

            template <>
            template <>
            Characters::String SharedMemoryStream<Characters::Character>::As () const;
            template <>
            template <>
            vector<Characters::Character> SharedMemoryStream<Characters::Character>::As () const;

            /**
             *  Ptr is a copyable smart pointer to a MemoryStream.
             */
            template <typename ELEMENT_TYPE>
            class SharedMemoryStream<ELEMENT_TYPE>::Ptr : public InputOutputStream<ELEMENT_TYPE>::Ptr {
            public:
                /**
                */
                Ptr ()                = default;
                Ptr (const Ptr& from) = default;
                Ptr (const SharedMemoryStream& from);

            public:
                nonvirtual Ptr& operator= (const Ptr& rhs) = default;
                nonvirtual Ptr& operator                   = (const SharedMemoryStream& rhs);
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SharedMemoryStream.inl"

#endif /*_Stroika_Foundation_Streams_SharedMemoryStream_h_*/
