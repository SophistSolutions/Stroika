/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
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
 *              class (when I implement it) based on Led chunked arrays). But not if allowing access
 *              as a span (???)
 * 
 *      @todo   Probably rewrite using InlineBuffer<> class
 *
 *      @todo   There should be some way to generically write vector<T> As<vector<T>>::Memory...(); For now I need
 *              multiple explicit template specializations.
 */

namespace Stroika::Foundation ::Streams {

    /**
     *  \brief  Simplest InputOutputStream; MemoryStream can be written to, and then the raw of data retrieved.
     *
     *  MemoryStream is Seekable.
     *
     *  Since MemoryStream keeps its data all in memory, it has the limitation that
     *  attempts to seek or write more than will fit in RAM will fail (with an exception).
     *
     *  Data written to the memory stream can then be read from the memory stream.
     * 
     *  Reads and writes maybe intermixed, but beware to carefully manage (the separate) input/output seek positions.
     *
     *  \note   NB: This class COULD have been called MemoryInputOutputStream.
     *
     *  \note   MemoryStream is NOT suitable for synchronized reading and writing between two threads (producer / consumer pattern).
     *          Reads return EOF instead of blocking (plus the lack of internal synchronization).
     *
     *          @see SharedMemoryStream
     *
     *  @see ExternallyOwnedMemoryInputStream
     *
     *  \par Example Usage
     *      \code
     *          BLOB                    blob    =   ReadRaw ();
     *          optional<VariantValue>  r       =   reader.Read (MemoryStream<byte>::New (blob));
     *      \endcode
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter</a>
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
         *  \brief Create a new MemoryStream, and return a Ptr to it. If args provided, they are passed to Write() before returning the resulting memory stream.
         * 
         *  \par Example Usage
         *      \code
         *          Streams::MemoryStream<byte>::Ptr out = Streams::MemoryStream<byte>::New ();
         *          DataExchange::Variant::JSON::Writer{}.Write (v, out);
         *          string xxx = out.As<string> ();
         *      \endcode
         *
         *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter</a>
         */
        static Ptr New ();
        static Ptr New (span<const ELEMENT_TYPE> copyFrom);
        static Ptr New (const Memory::BLOB& copyFrom)
            requires (is_same_v<ELEMENT_TYPE, byte>);

    private:
        class Rep_;

    public:
        [[deprecated ("Since Stroika v3.0d5 use span interface")]] static Ptr New (const ELEMENT_TYPE* start, const ELEMENT_TYPE* end)
        {
            return make_shared<Rep_> (start, end);
        }
    };

    /**
     *  Ptr is a copyable smart pointer to a MemoryStream.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter">C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter/a>
     */
    template <typename ELEMENT_TYPE>
    class MemoryStream<ELEMENT_TYPE>::Ptr : public InputOutputStream<ELEMENT_TYPE>::Ptr {
    private:
        using inherited = typename InputOutputStream<ELEMENT_TYPE>::Ptr;

    public:
        /**
         *  \par Example Usage
         *      \code
         *          Streams::MemoryStream<byte>::Ptr out = Streams::MemoryStream<byte>::New ();
         *          DataExchange::Variant::JSON::Writer{}.Write (v, out);
         *          string xxx = out.As<string> ();
         *      \endcode
         */
        Ptr () = default;
        Ptr (nullptr_t);
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
         *  And if ElementType is byte, then T can also be one of:
         *      o   Memory::BLOB
         *      o   string
         *
         *  And if ElementType is Characters::Character, then T can also be one of:
         *      o   String
         */
        template <typename T>
        nonvirtual T As () const;

    private:
        /**
         * \req *this != nullptr
         */
        nonvirtual const Rep_& GetRepConstRef_ () const;

    private:
        /**
         * \req *this != nullptr
         */
        nonvirtual Rep_& GetRepRWRef_ () const;

    private:
        friend class MemoryStream;
    };

    template <>
    template <>
    Memory::BLOB MemoryStream<byte>::Ptr::As () const;
    template <>
    template <>
    string MemoryStream<byte>::Ptr::As () const;
    template <>
    template <>
    vector<byte> MemoryStream<byte>::Ptr::As () const;

    template <>
    template <>
    Characters::String MemoryStream<Characters::Character>::Ptr::As () const;
    template <>
    template <>
    vector<Characters::Character> MemoryStream<Characters::Character>::Ptr::As () const;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "MemoryStream.inl"

#endif /*_Stroika_Foundation_Streams_MemoryStream_h_*/
