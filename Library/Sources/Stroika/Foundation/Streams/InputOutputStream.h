/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
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

namespace Stroika::Foundation::Streams::InputOutputStream {

    template <typename ELEMENT_TYPE>
    class Ptr;

    template <typename ELEMENT_TYPE>
    class IRep;

    /**
     *  \note   Design Note:
     *      There are two principle cases of combining input and output streams into an InputOutputStream:
     *          1>  where there the sequences of data are unrelated
     *          2>  where writes to the input side of the stream 'appears' in the 'output' side of the stream.
     *
     *      Stroika used to have two kinds of input output streams - BinaryTiedStream, and BinaryInputOutputStream.
     *      However, that distinction was abandoned in v2.0a97, since there appeared to be no reason to capture this distinction
     *      (we may want to revisit, but that's the plan for now - LGP 2017-01-28)
     *
     *      An InputOutputStream<> could be either one of these two cases - where writes to one side appear on reads
     *      of the other side (such as @see MemoryStream), or where they are unrelated, such as with
     *      @see SocketStream.
     */

    /**
     *  \brief  InputOutputStream is single stream object that acts much as a InputStream::Ptr and an OutputStream::Ptr.
     *
     *  @see @InputOutputStream<ELEMENT_TYPE>
     *
     *  \note   Design Note:
     *      InputOutputStream::Ptr<ELEMENT_TYPE> inherits from InputStream::Ptr<ELEMENT_TYPE> and OutputStream::Ptr<ELEMENT_TYPE>,
     *      so it has two copies of the shared_ptr, even though there is only one underlying 'rep' object.
     *
     *      It is required/guaranteed by the implementation that the 'input' and 'output' sides refer to the same underlying
     *      'rep'.
     *
     *  \note   Design Note:
     *      InputOutputStream::Ptr<ELEMENT_TYPE> need not have the same value for IsSeekable () -
     *      but if you call InputOutputStream<ELEMENT_TYPE>::IsSeekable () - that  method requires they both be the same.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter">C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter/a>
     *
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *      o   only operator== (nullptr_t) is supported
     */
    template <typename ELEMENT_TYPE>
    class Ptr : public InputStream::Ptr<ELEMENT_TYPE>, public OutputStream::Ptr<ELEMENT_TYPE> {
    public:
        /**
         *  defaults to null (aka empty ())
         */
        Ptr () = default;
        Ptr (nullptr_t);
        Ptr (const Ptr&) = default;
        Ptr (Ptr&&)      = default;
        /**
         * _SharedIRep rep is the underlying shared output Stream object.
         *
         *  \req rep != nullptr (use nullptr_t constructor)
         */
        Ptr (const shared_ptr<IRep<ELEMENT_TYPE>>& rep);

    public:
        /**
        */
        nonvirtual Ptr& operator= (const Ptr&) = default;
        nonvirtual Ptr& operator= (Ptr&&)      = default;

    public:
        /**
         *      \req InputStream::Ptr<ELEMENT_TYPE>::IsSeekable () == OutputStream<ELEMENT_TYPE>::IsSeekable ()
         */
        nonvirtual bool IsSeekable () const;

    public:
        /**
         *  Close BOTH the InputStream and OutputStream side of this InputOutputStream.
         *
         *  \brief Alias for CloseRead (); CloseWrite ();
         */
        nonvirtual void Close () const;
        nonvirtual void Close (bool reset);

    public:
        /**
         *  \brief Alias for InputStream<>::Ptr::Close ();
         */
        nonvirtual void CloseRead () const;
        nonvirtual void CloseRead (bool reset);

    public:
        /**
         *  \brief Alias for OutputStream<>::Ptr::Close ();
         */
        nonvirtual void CloseWrite () const;
        nonvirtual void CloseWrite (bool reset);

    public:
        /**
         *  Return true, unless a call to CloseRead () has been done on the underlying stream (not just Ptr).
         *  @see CloseRead ()
         */
        nonvirtual bool IsOpenRead () const;

    public:
        /**
         *  Return true, unless a call to CloseWrite () has been done on the underlying stream (not just Ptr).
         *  @see CloseWrite ()
         */
        nonvirtual bool IsOpenWrite () const;

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

    public:
        /**
         *  \brief return true iff stream ptr is nullptr
         *
         *  @see reset()
         */
        nonvirtual bool operator== (nullptr_t) const;

    public:
        /**
         *  \brief access to underlying stream smart pointer
         */
        nonvirtual shared_ptr<IRep<ELEMENT_TYPE>> GetSharedRep () const;

    public:
        /**
         * \req *this != nullptr
         */
        nonvirtual const IRep<ELEMENT_TYPE>& GetRepConstRef () const;

    public:
        /**
         * \req *this != nullptr
         */
        nonvirtual IRep<ELEMENT_TYPE>& GetRepRWRef () const;
    };

    /**
     *
     */
    template <typename ELEMENT_TYPE>
    class IRep : public InputStream::IRep<ELEMENT_TYPE>, public OutputStream::IRep<ELEMENT_TYPE> {
    public:
        using ElementType = ELEMENT_TYPE;

    public:
        IRep ()            = default;
        IRep (const IRep&) = delete;

    public:
        nonvirtual IRep& operator= (const IRep&) = delete;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InputOutputStream.inl"

#endif /*_Stroika_Foundation_Streams_InputOutputStream_h_*/
