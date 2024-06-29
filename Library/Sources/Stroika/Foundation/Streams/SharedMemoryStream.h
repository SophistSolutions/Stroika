/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_SharedMemoryStream_h_
#define _Stroika_Foundation_Streams_SharedMemoryStream_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <vector>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Memory/BLOB.h"
#include "Stroika/Foundation/Streams/InputOutputStream.h"

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
     *  \brief Options to configure a new SharedMemoryStream; defaults should always work fine, but options can allow for better performance (at a loss of some functionality)
     */
    struct Options {
        /**
         *  \brief controls if the shared stream is automatically synchronized internally so that it can be used by two different threads safely, without any locking (applies to letter, not envelope of shared_ptr)
         */
        Execution::InternallySynchronized fInternallySynchronized{Execution::InternallySynchronized::eInternallySynchronized};

        /**
         *  \brief Controls if the SharedMemoryStream maintains history. Doing so allows the Ptr<>::As<> methods to work. Not doing so, allows potentially significant memory savings on larger streams.
         */
        bool fSeekable{true};
    };

    /**
     *  \brief  SharedMemoryStream<> is an InputOutputStream<> like MemoryStream<> but supporting concurrency (like an in memory structured pipe)
     *
     *  SharedMemoryStream is Seekable by default (see Options::fSeekable)
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
     *  \note   \em Thread-Safety   <a href='#C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized'>C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized</a> OR
     *                              <a href='#C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter'>C++-Standard-Thread-Safety-For-Envelope-Plus-Must-Externally-Synchronize-Letter</a>
     *                              depending on Options object provided to New ()
     */
    template <typename ELEMENT_TYPE>
    Ptr<ELEMENT_TYPE> New (Options options = {});
    template <typename ELEMENT_TYPE, typename COPY_FROM>
    Ptr<ELEMENT_TYPE> New (const COPY_FROM& copyFrom, Options options = {})
        requires (same_as<ELEMENT_TYPE, byte> and Configuration::IAnyOf<COPY_FROM, Memory::BLOB, span<const ELEMENT_TYPE>>);

    namespace Private_ {
        template <typename ELEMENT_TYPE>
        class IRep_;
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
        Ptr (const shared_ptr<Private_::IRep_<ELEMENT_TYPE>>& from);

    public:
        nonvirtual Ptr& operator= (const Ptr& rhs) = default;

    public:
        nonvirtual Options GetOptions () const;

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
         * 
         *  \req GetOptions ().fSeekable
         */
        template <typename T>
        nonvirtual T As () const
            requires (same_as<T, vector<ELEMENT_TYPE>> or (same_as<ELEMENT_TYPE, byte> and Configuration::IAnyOf<T, Memory::BLOB, string>) or
                      (same_as<ELEMENT_TYPE, Characters::Character> and same_as<T, Characters::String>));

    private:
        /**
         * \req *this != nullptr
         */
        nonvirtual const Private_::IRep_<ELEMENT_TYPE>& GetRepConstRef_ () const;

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
