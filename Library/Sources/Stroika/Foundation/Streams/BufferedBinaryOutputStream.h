/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Streams_BufferedBinaryOutputStream_h_
#define _Stroika_Foundation_Streams_BufferedBinaryOutputStream_h_   1

#include    "../StroikaPreComp.h"

#include    <vector>

#include    "../Configuration/Common.h"
#include    "BinaryOutputStream.h"


/*
 * TODO:
 *
 *      o make threadsafe
 *
 *      o   Think out Seekable. Probably MUST mixin Seekable, and properly handle (vai throw OperaitonNotSupported) if one constructs a BufferedBinaryOutputStream
 *          and tries to SEEK wihout the undelrying stream being seekable.
 *
 */

namespace   Stroika {
    namespace   Foundation {
        namespace   Streams {

            /*
             * A BufferedBinaryOutputStream wraps an argument stream (which must have lifetime > this BufferedBinaryOutputStream)
             * and will buffer up writes to it.
             *
             * NOTE - its is REQUIRED to call Flush before destruction if any pending data has not yet been flushed. It is NOT done
             * in the BufferedBinaryOutputStream::DTOR - because the underlying stream could have an exceptin writing, and its illegal to propagate
             * exceptions through destructors.
             */
            class   BufferedBinaryOutputStream : public BinaryOutputStream {
            public:
                NO_DEFAULT_CONSTRUCTOR (BufferedBinaryOutputStream);
                NO_COPY_CONSTRUCTOR (BufferedBinaryOutputStream);
                NO_ASSIGNMENT_OPERATOR (BufferedBinaryOutputStream);

            public:
                BufferedBinaryOutputStream (BinaryOutputStream& realOut);
                ~BufferedBinaryOutputStream ();

            public:
                nonvirtual  size_t  GetBufferSize () const;
                nonvirtual  void    SetBufferSize (size_t bufSize);

            public:
                // Throws away all data about to be written (buffered). Once this is called, its illegal to call Flush or another write
                nonvirtual  void    Abort ();

                //
                nonvirtual  void    Flush ();

            protected:
                // pointer must refer to valid memory at least bufSize long, and cannot be nullptr. BufSize must always be >= 1.
                // Writes always succeed fully or throw.
                virtual void            _Write (const Byte* start, const Byte* end) override;

            private:
                vector<Byte>        fBuffer_;
                BinaryOutputStream& fRealOut_;
#if     qDebug
                bool                fAborted_;
#endif
            };

        }
    }
}
#endif  /*_Stroika_Foundation_Streams_BufferedBinaryOutputStream_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "BufferedBinaryOutputStream.inl"
