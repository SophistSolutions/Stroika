/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BufferedInputStream_h_
#define _Stroika_Foundation_Streams_BufferedInputStream_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Streams/InputStream.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *      @todo   Seekable! Must think out if this should mixin Seekable or not. I THINK it must be.
 *              I THINK if one SEEKS this, but the underlying
 *              class doesn't support seeking, we MAY need to either BUFFER MORE, or throw not supported.
 */

namespace Stroika::Foundation::Streams::BufferedInputStream {

    template <typename ELEMENT_TYPE>
    class Ptr;

    /**
     *  @brief  BufferedInputStream is an InputStream::Ptr<ELEMENT_TYPE> which provides buffered access.
     *          This is useful if calls to the underling stream source can be expensive. This class
     *          loads chunks of the stream into memory, and reduces calls to the underlying stream.
     *
     *  \note if seekable true, the resulting stream is seekable. If seekable is false, the resulting
     *        stream is not seekable. If seekable is unspecified (nullopt), the resulting stream is seekable
     *        iff the source stream is seekable.
     * 
     *        Specifying true (seekable) when the source stream is not seekable is allowed, but requires enuf
     *        memory to buffer the ENTIRE contents of the stream (other cases just buffer a bit so require more
     *        modest amounts of memory).
     * 
     *  \par Example Usage
     *      \code
     *          InputStream::Ptr<byte> in = BufferedInputStream::New<byte> (fromStream);
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          CallExpectingBinaryInputStreamPtr (BufferedInputStream::New<byte> (fromStream))
     *      \endcode
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter</a>
     */
    template <typename ELEMENT_TYPE>
    Ptr<ELEMENT_TYPE> New (const typename InputStream::Ptr<ELEMENT_TYPE>& realIn, optional<bool> seekable = {});
    template <typename ELEMENT_TYPE>
    Ptr<ELEMENT_TYPE> New (Execution::InternallySynchronized internallySynchronized, const typename InputStream::Ptr<ELEMENT_TYPE>& realIn,
                           optional<bool> seekable = {});

    namespace Private_ {
        template <typename ELEMENT_TYPE>
        class IRep_ : public InputStream::IRep<ELEMENT_TYPE> {
        public:
        };
        template <typename ELEMENT_TYPE>
        class Rep_Seekable_FromSeekable_;
        template <typename ELEMENT_TYPE, size_t INLINE_BUF_SIZE>
        class Rep_Seekable_FromUnSeekable_;
        template <typename ELEMENT_TYPE, size_t INLINE_BUF_SIZE>
        class Rep_UnSeekable_;
    }

    /**
     *  Ptr is a copyable smart pointer to a BufferedInputStream.
     */
    template <typename ELEMENT_TYPE>
    class Ptr : public InputStream::Ptr<ELEMENT_TYPE> {
        using inherited = typename InputStream::Ptr<ELEMENT_TYPE>;

    public:
        /**
         *  \par Example Usage
         *      \code
         *          BufferedInputStream::Ptr<byte> in = BufferedInputStream::New<byte> (FileInputStream::New (fileName));
         *      \endcode
         */
        Ptr ()                = default;
        Ptr (const Ptr& from) = default;
        Ptr (Ptr&& from)      = default;
        Ptr (const shared_ptr<Private_::IRep_<ELEMENT_TYPE>>& from);

    public:
        nonvirtual Ptr& operator= (const Ptr& rhs) = default;
        nonvirtual Ptr& operator= (Ptr&& rhs)      = default;

    private:
        /**
         *  \brief protected access to underlying stream smart pointer
         */
        nonvirtual shared_ptr<Private_::IRep_<ELEMENT_TYPE>> GetSharedRep_ () const;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "BufferedInputStream.inl"

#endif /*_Stroika_Foundation_Streams_BufferedInputStream_h_*/
