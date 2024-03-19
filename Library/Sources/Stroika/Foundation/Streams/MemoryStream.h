/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_MemoryStream_h_
#define _Stroika_Foundation_Streams_MemoryStream_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <vector>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Memory/BLOB.h"

#include "InputOutputStream.h"

/*
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation ::Streams::MemoryStream {

    template <typename ELEMENT_TYPE>
    class Ptr;

    /**
     *  \brief  Create the Simplest InputOutputStream; MemoryStream can be written to, and then the raw data retrieved.
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
     *  \note   Despite the somewhat 'low-level' sounding name, this can be used with arbitrary C++ objects (not just POD), so it respects copy-constructors etc.
     *
     *  \note   MemoryStream is NOT suitable for synchronized reading and writing between two threads (producer / consumer pattern).
     *          Reads return EOF instead of blocking (plus the lack of internal synchronization).
     *
     *          @see SharedMemoryStream for that purpose.
     *
     *  @see ExternallyOwnedMemoryInputStream
     *
     *  \par Example Usage
     *      \code
     *          BLOB                    blob    =   ReadRaw ();
     *          optional<VariantValue>  r       =   reader.Read (MemoryStream::New<byte> (blob));
     *      \endcode
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter</a>
     *
     *  \par Example Usage
     *      \code
     *          Streams::MemoryStream::Ptr<byte> out = Streams::MemoryStream::New<byte> ();
     *          DataExchange::Variant::JSON::Writer{}.Write (v, out);
     *          string xxx = out.As<string> ();
     *      \endcode
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter</a>
     * 
     *  TODO:
     *      @todo   The Rep_ would be a good candidate class to rewrite using new Sequence_ChunkedArray
     *              class (when I implement it) based on Led chunked arrays). But not if allowing access
     *              as a span (???)
     * 
     *      @todo   Probably rewrite using InlineBuffer<> class
     */
    template <typename ELEMENT_TYPE>
    Ptr<ELEMENT_TYPE> New ();
    template <typename ELEMENT_TYPE, size_t EXTENT>
    Ptr<ELEMENT_TYPE> New (span<const ELEMENT_TYPE, EXTENT> copyFrom);
    template <typename ELEMENT_TYPE>
    Ptr<ELEMENT_TYPE> New (const Memory::BLOB& copyFrom)
        requires (is_same_v<ELEMENT_TYPE, byte>);

    namespace Private_ {
        template <typename ELEMENT_TYPE>
        class Rep_;
    }

    /**
     *  Ptr is a smart pointer to a MemoryStream shared 'Rep' object.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter">C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter/a>
     */
    template <typename ELEMENT_TYPE>
    class Ptr : public InputOutputStream::Ptr<ELEMENT_TYPE> {
    private:
        using inherited = typename InputOutputStream::Ptr<ELEMENT_TYPE>;

    public:
        /**
         *  \par Example Usage
         *      \code
         *          Streams::MemoryStream::Ptr<byte> out = Streams::MemoryStream::New<byte> ();
         *          DataExchange::Variant::JSON::Writer{}.Write (v, out);
         *          string xxx = out.As<string> ();
         *      \endcode
         */
        Ptr () = default;
        Ptr (nullptr_t);
        Ptr (const Ptr& from) = default;
        Ptr (const shared_ptr<Private_::Rep_<ELEMENT_TYPE>>& from);

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
        nonvirtual T As () const
            requires (same_as<T, vector<ELEMENT_TYPE>> or (same_as<ELEMENT_TYPE, byte> and (same_as<T, Memory::BLOB> or same_as<T, string>)) or
                      (same_as<ELEMENT_TYPE, Characters::Character> and (same_as<T, Characters::String>)));

    public:
        /**
         *  Dump in some debugger friendly format/summary
         */
        nonvirtual Characters::String ToString () const;

    private:
        /**
         * \req *this != nullptr
         */
        nonvirtual const Private_::Rep_<ELEMENT_TYPE>& GetRepConstRef_ () const;

    private:
        /**
         * \req *this != nullptr
         */
        nonvirtual Private_::Rep_<ELEMENT_TYPE>& GetRepRWRef_ () const;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "MemoryStream.inl"

#endif /*_Stroika_Foundation_Streams_MemoryStream_h_*/
