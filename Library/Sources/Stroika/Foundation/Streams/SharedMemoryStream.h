/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *      @todo https://stroika.atlassian.net/browse/STK-584 - 
 *            provide option for SharedMemoryStream so not seekable - less memory
 */

namespace Stroika::Foundation::Streams::SharedMemoryStream {

    template <typename ELEMENT_TYPE>
    class Ptr;

    /**
     *  \brief  SharedMemoryStream<> is an InputOutputStream<> like MemoryStream<> but supporting concurrency ; like an in memory structured pipe
     *
     *  SharedMemoryStream is Seekable.
     *
     *  Since SharedMemoryStream keeps its data all in memory, it has the limitation that
     *  attempts to seek or write more than will fit in RAM will fail (with an exception).
     *
     *  \note   SharedMemoryStream is suitable for synchronized reading and writing between two threads (producer / consumer pattern).
     *          Reads will block at the end of the stream until some thread calls SharedMemoryStream><>::CloseWrite ()
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
     *           SharedMemoryStream::Ptr<unsigned int> pipe = SharedMemoryStream::New<unsigned int> ();
     *           unsigned                              sum{};
     *           static constexpr unsigned int         kStartWith{1};
     *           static constexpr unsigned int         kUpToInclusive_{1000};
     *           Thread::Ptr                           consumer = Thread::New ([&]() {
     *                  while (auto o = pipe.Read ()) {
     *                      sum += *o;
     *                  }
     *              },
     *              Thread::eAutoStart);
     *           Thread::Ptr producer = Thread::New ([&]() {
     *                  for (unsigned int i = kStartWith; i <= kUpToInclusive_; ++i) {
     *                      pipe.Write (i);
     *                  };
     *                  pipe.CloseWrite ();    // critical or consumer hangs on final read
     *              },
     *              Thread::eAutoStart);
     *           Thread::WaitForDone ({consumer, producer});
     *           Assert (sum == (1 + kUpToInclusive_) * (kUpToInclusive_ - 1 + 1) / 2); // not a race
     *      \endcode
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized">C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized</a>
     */
    template <typename ELEMENT_TYPE>
    Ptr<ELEMENT_TYPE> New ();
    template <typename ELEMENT_TYPE>
    Ptr<ELEMENT_TYPE> New (span<const ELEMENT_TYPE> copyFrom);
    template <typename ELEMENT_TYPE>
    Ptr<ELEMENT_TYPE> New (const Memory::BLOB& copyFrom)
        requires (same_as<ELEMENT_TYPE, byte>);

    namespace Private_ {
        template <typename ELEMENT_TYPE>
        class Rep_;
    }

    /**
     *  Ptr is a copyable smart pointer to a SharedMemoryStream.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter">C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter/a>
     */
    template <typename ELEMENT_TYPE>
    class Ptr : public InputOutputStream::Ptr<ELEMENT_TYPE> {
    private:
        using inherited = typename InputOutputStream::Ptr<ELEMENT_TYPE>;

    public:
        /**
        */
        Ptr ()                = default;
        Ptr (const Ptr& from) = default;
        Ptr (const shared_ptr<Private_::Rep_<ELEMENT_TYPE>>& from);

    public:
        nonvirtual Ptr& operator= (const Ptr& rhs) = default;

    public:
        /**
         *  Convert the current contents of this SharedMemoryStream into one of the "T" representations.
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

    private:
        friend class SharedMemoryStream;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SharedMemoryStream.inl"

#endif /*_Stroika_Foundation_Streams_SharedMemoryStream_h_*/
