/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_MemoryStream_h_
#define _Stroika_Foundation_Streams_MemoryStream_h_ 1

#include "../StroikaPreComp.h"

#include <vector>

#include "../Characters/String.h"
#include "../Configuration/Common.h"
#include "../Memory/BLOB.h"

#include "InputOutputStream.h"

/*
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *      @todo   This would be a good candidate class to rewrite using new Sequence_ChunkedArray
 *              class (when I implement it) based on Led chunked arrays).
 *
 *      @todo   There should be some way to generically write vector<T> As<vector<T>>::Memory...(); For now I need
 *              multiple explicit template specailizations.
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Streams {

            /**
             *  \brief  Simplest to use InputOutputStream; MemoryStream can be written to, and then the raw of data retrieved.
             *
             *  MemoryStream is Seekable.
             *
             *  Since MemoryStream keeps its data all in memory, it has the limitation that
             *  attempts to seek or write more than will fit in RAM will fail (with an exception).
             *
             *  Data written to the memory stream can then be read from the memory stream.
             *
             *  \note   NB: This class COULD have been called MemoryInputOutputStream.
             *
             *  \note   MemoryStream is NOT suitable for synchonized reading and writing between two threads (producer / consumer pattern).
             *          Reads return EOF instead of blocking (plus the lack of internal syncrhonization).
             *
             *          @see SharedMemoryStream
             *
             *  @see ExternallyOwnedMemoryInputStream
             *
             *  \par Example Usage
             *      \code
             *          BLOB                    blob    =   ReadRaw ();
             *          Optional<VariantValue>  r       =   reader.Read (MemoryStream<Byte> (blob));
             *      \endcode
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Must-Externally-Synchronize-Letter-Thread-Safety">Must-Externally-Synchronize-Letter-Thread-Safety</a>
             */
            template <typename ELEMENT_TYPE>
            class MemoryStream : public InputOutputStream<ELEMENT_TYPE> {

                MemoryStream ()                    = delete;
                MemoryStream (const MemoryStream&) = delete;

            public:
                class Ptr;

            public:
                /**
                 *  To copy a MemoryStream, use MemoryStream<T>::Ptr
                 *
                 *  \par Example Usage
                 *      \code
                 *          Streams::MemoryStream<Byte>::Ptr out = Streams::MemoryStream<Byte>{};
                 *          DataExchange::Variant::JSON::Writer ().Write (v, out);
                 *          string xxx = out.As<string> ();
                 *      \endcode
                 */
                static Ptr New ();
                static Ptr New (const ELEMENT_TYPE* start, const ELEMENT_TYPE* end);
                template <typename TEST_TYPE = ELEMENT_TYPE, typename ENABLE_IF_TEST = typename enable_if<is_same<TEST_TYPE, Memory::Byte>::value>::type>
                static Ptr New (const Memory::BLOB& blob);

            private:
                class Rep_;
            };

            /**
             *  Ptr is a copyable smart pointer to a MemoryStream.
             */
            template <typename ELEMENT_TYPE>
            class MemoryStream<ELEMENT_TYPE>::Ptr : public InputOutputStream<ELEMENT_TYPE>::Ptr {
            private:
                using inherited = typename InputOutputStream<ELEMENT_TYPE>::Ptr;

            public:
                /**
                 *  \note Ptr () creates a null stream, not an empty memory stream.
                 *
                 *  \par Example Usage
                 *      \code
                 *          Streams::MemoryStream<Byte>::Ptr out = Streams::MemoryStream<Byte>{};
                 *          DataExchange::Variant::JSON::Writer ().Write (v, out);
                 *          string xxx = out.As<string> ();
                 *      \endcode
                 */
                Ptr () = delete; // 95% of time would be a bug - init with nullptr
                // allow no-arg CTOR when we've converted - not bad - just bad now cuz prev sematnics
                Ptr (nullptr_t) {}
                Ptr (const Ptr& from) = default;

            private:
                Ptr (const shared_ptr<Rep_>& from);

            public:
                nonvirtual Ptr& operator= (const Ptr& rhs) = default;
                nonvirtual Ptr& operator                   = (const MemoryStream& rhs);

            public:
                /**
                 *  Convert the current contents of this MemoryStream into one of the "T" representations.
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

            private:
                friend class MemoryStream;
            };

            template <>
            template <>
            Memory::BLOB MemoryStream<Memory::Byte>::Ptr::As () const;
            template <>
            template <>
            string MemoryStream<Memory::Byte>::Ptr::As () const;
            template <>
            template <>
            vector<Memory::Byte> MemoryStream<Memory::Byte>::Ptr::As () const;

            template <>
            template <>
            Characters::String MemoryStream<Characters::Character>::Ptr::As () const;
            template <>
            template <>
            vector<Characters::Character> MemoryStream<Characters::Character>::Ptr::As () const;
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "MemoryStream.inl"

#endif /*_Stroika_Foundation_Streams_MemoryStream_h_*/
