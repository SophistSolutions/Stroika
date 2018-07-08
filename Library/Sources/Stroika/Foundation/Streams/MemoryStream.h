/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_MemoryStream_h_
#define _Stroika_Foundation_Streams_MemoryStream_h_ 1

#include "../StroikaPreComp.h"

#include <vector>

#include "../Characters/String.h"
#include "../Configuration/Common.h"
#include "../Memory/BLOB.h"

#include "InputOutputStream.h"
#include "InternallySyncrhonizedInputOutputStream.h"

/*
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *      @todo   This would be a good candidate class to rewrite using new Sequence_ChunkedArray
 *              class (when I implement it) based on Led chunked arrays).
 *
 *      @todo   There should be some way to generically write vector<T> As<vector<T>>::Memory...(); For now I need
 *              multiple explicit template specializations.
 *
 *      @todo   https://stroika.atlassian.net/browse/STK-608 - probably be made more efficent in sync form - using direct mutex
 */

namespace Stroika::Foundation {
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
         *  \note   MemoryStream is NOT suitable for synchronized reading and writing between two threads (producer / consumer pattern).
         *          Reads return EOF instead of blocking (plus the lack of internal syncrhonization).
         *
         *          @see SharedMemoryStream
         *
         *  @see ExternallyOwnedMemoryInputStream
         *
         *  \par Example Usage
         *      \code
         *          BLOB                    blob    =   ReadRaw ();
         *          optional<VariantValue>  r       =   reader.Read (MemoryStream<Byte>::New (blob));
         *      \endcode
         */
        template <typename ELEMENT_TYPE>
        class MemoryStream : public InputOutputStream<ELEMENT_TYPE> {
        public:
            /**
             *  'MemoryStream' is a quasi-namespace: use Ptr or New () members.
             */
            MemoryStream ()                    = delete;
            MemoryStream (const MemoryStream&) = delete;

        public:
            class Ptr;

        public:
            /**
             *  \par Example Usage
             *      \code
             *          Streams::MemoryStream<Byte>::Ptr out = Streams::MemoryStream<Byte>::New ();
             *          DataExchange::Variant::JSON::Writer ().Write (v, out);
             *          string xxx = out.As<string> ();
             *      \endcode
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Rep-Inside-Ptr-Must-Be-Externally-Syncrhonized">Rep-Inside-Ptr-Must-Be-Externally-Syncrhonized</a>
             */
            static Ptr New (Execution::InternallySyncrhonized internallySyncrhonized = Execution::eNotKnownInternallySynchronized);
            static Ptr New (const ELEMENT_TYPE* start, const ELEMENT_TYPE* end);
            static Ptr New (Execution::InternallySyncrhonized internallySyncrhonized, const ELEMENT_TYPE* start, const ELEMENT_TYPE* end);
            template <typename TEST_TYPE = ELEMENT_TYPE, typename ENABLE_IF_TEST = enable_if_t<is_same_v<TEST_TYPE, Memory::Byte>>>
            static Ptr New (const Memory::BLOB& blob);
            template <typename TEST_TYPE = ELEMENT_TYPE, typename ENABLE_IF_TEST = enable_if_t<is_same_v<TEST_TYPE, Memory::Byte>>>
            static Ptr New (Execution::InternallySyncrhonized internallySyncrhonized, const Memory::BLOB& blob);

        private:
            class Rep_;

        protected:
            /**
             *  Utility to create a Ptr wrapper (to avoid having to subclass the Ptr class and access its protected constructor)
             */
            static Ptr _mkPtr (const shared_ptr<Rep_>& s);

        private:
            using InternalSyncRep_ = InternallySyncrhonizedInputOutputStream<ELEMENT_TYPE, Streams::MemoryStream<ELEMENT_TYPE>, typename MemoryStream<ELEMENT_TYPE>::Rep_>;
        };

        /**
         *  Ptr is a copyable smart pointer to a MemoryStream.
         *
         *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter">C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter/a>
         */
        template <typename ELEMENT_TYPE>
        class MemoryStream<ELEMENT_TYPE>::Ptr : public InputOutputStream<ELEMENT_TYPE>::Ptr {
        private:
            using inherited = typename InputOutputStream<ELEMENT_TYPE>::Ptr;

        public:
            /**
             *  \par Example Usage
             *      \code
             *          Streams::MemoryStream<Byte>::Ptr out = Streams::MemoryStream<Byte>::New ();
             *          DataExchange::Variant::JSON::Writer ().Write (v, out);
             *          string xxx = out.As<string> ();
             *      \endcode
             */
            Ptr () = delete; // 95% of time would be a bug - init with nullptr
            // allow no-arg CTOR when we've converted - not bad - just bad now cuz prev sematnics
            Ptr (nullptr_t) {}
            Ptr (const Ptr& from) = default;

        protected:
            Ptr (const shared_ptr<Rep_>& from);

        public:
            nonvirtual Ptr& operator= (const Ptr& rhs) = default;

        public:
            /**
             *  Convert the current contents of this MemoryStream into one of the "T" representations.
             *
             *  Only specifically specialized variants are supported. T can be one of:
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

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "MemoryStream.inl"

#endif /*_Stroika_Foundation_Streams_MemoryStream_h_*/
