/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BinaryOutputStream_h_
#define _Stroika_Foundation_Streams_BinaryOutputStream_h_   1

#include    "../StroikaPreComp.h"

#include    <memory>

#include    "../Configuration/Common.h"
#include    "../Memory/Common.h"

#include    "BinaryStream.h"
#include    "OutputStream.h"


/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 *
 *      @todo   Should add Close () method. Any subsequent calls to this stream - would fail?
 *
 *              (maybe close/flush ignored).
 *
 *              If we allow for that - we may need to have check method - isOpen?. So maybe best to
 *              have flush/close allowed, and anything else generate an assert error?
 *
 *      @todo   Add abiiliy to SetEOF (); You can SEEK, but if you seek backwards, and start writing - that doesnt change EOF. EOF
 *              remains fixed as max written to. DODUCMNET THIS (for text and binary) - and provide a SetEOF() method
 *              (maybe just for seekable streams)? Maybe add rule that SetEOF () can only go backwards (shorten). Then call
 *              PullBackEOF() or RestrictEOF() or RemovePast(); OR ResetEOFToCurrentPosiiton(). Later maybe best API.
 *
 *      @todo   Consider/document approaches to timeouts. We COULD have a stream class where
 *              it was a PROPERTY OF THE CLASS (or alternate API) where writes timeout after
 *              a certain point.
 */


namespace Stroika { namespace Foundation { namespace Memory { class BLOB; } } }



namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {


            using   Memory::Byte;


            /**
             *  @todo CLEANUP DOCS
             *
             * Design Overview:
             *
             *      o   BinaryInputStream and BinaryOutputStream CAN be naturally mixed togehter to make
             *          an input/output stream. Simlarly, they can both be mixed together with Seekable.
             *          But NONE of the Binary*Stream classes may be mixed together with Text*Stream classes.
             *
             *  Note - when you Seek() away from the end of a binary output stream, and then write, you automatically
             *  extend the stream to the point seeked to, and if you seek back (less) than the end and write, this overwrites
             *  instead of inserting.
             *
             *  Note - Write is sufficient to guarnatee the data is written, but it may be buffered until you call
             *  the destructor on the BinaryOutputStream (last reference goes away) or until you call Flush ().
             */
            class   BinaryOutputStream : public OutputStream<Byte> {
            private:
                using inherited = OutputStream<Byte>;

            protected:
                /**
                 * _SharedIRep arg - MAY also mixin Seekable - and if so - this automatically uses it.
                 */
                explicit BinaryOutputStream (const _SharedIRep& rep)
                    : inherited (rep)
                {
                }
            public:
                BinaryOutputStream () = default;
                BinaryOutputStream (nullptr_t)
                    : inherited (nullptr)
                {
                }
                BinaryOutputStream (const OutputStream<Byte>& from);
            public:
                nonvirtual  void    Write (const Byte* start, const Byte* end) const
                {
                    inherited::Write (start, end);
                }
                nonvirtual  void    Write (const Memory::BLOB& blob) const;
            };



        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "BinaryOutputStream.inl"

#endif  /*_Stroika_Foundation_Streams_BinaryOutputStream_h_*/
