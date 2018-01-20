/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Compression_Reader_h_
#define _Stroika_Foundation_DataExchange_Compression_Reader_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Memory/SharedByValue.h"
#include "../../Streams/InputStream.h"

#include "../VariantValue.h"

/**
 *  \file
 *
 * TODO:
 *      @todo   UNDOCUMENTED - PRELIMINARY - ROUGH DRAFT TO THINK OUT API
 *
 *      @todo   https://stroika.atlassian.net/browse/STK-609 - Redo the Compression Reader code to use a factory style like used in the rest of Stroika - ::New - instead of Compress methods??? needs some thought...
 */

namespace Stroika {
    namespace Foundation {
        namespace DataExchange {
            namespace Compression {

                using Memory::BLOB;
                using Memory::Byte;
                using Streams::InputStream;

                /**
                 *  Abstraction for Readers that map files or streams to collections of files, like zip files, tar files, etc.
                 */
                class Reader {
                protected:
                    class _IRep;

                public:
                    Reader ()              = delete;
                    Reader (const Reader&) = delete;
                    Reader (Reader&& src);
                    Reader& operator= (const Reader&) = delete;

                protected:
                    explicit Reader (const shared_ptr<_IRep>& rep);

                public:
                    nonvirtual InputStream<Byte>::Ptr Compress (const InputStream<Byte>::Ptr& src) const;
                    nonvirtual BLOB Compress (const BLOB& src) const;

                public:
                    nonvirtual InputStream<Byte>::Ptr Decompress (const InputStream<Byte>::Ptr& src) const;
                    nonvirtual BLOB Decompress (const BLOB& src) const;

                protected:
                    nonvirtual _IRep& _GetRep ();
                    nonvirtual const _IRep& _GetRep () const;

                private:
                    shared_ptr<_IRep> fRep_;
                };

                /**
                 */
                class Reader::_IRep {
                public:
                    virtual InputStream<Byte>::Ptr Compress (const InputStream<Byte>::Ptr& src) const = 0;

                public:
                    virtual InputStream<Byte>::Ptr Decompress (const InputStream<Byte>::Ptr& src) const = 0;
                };
            }
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Reader.inl"

#endif /*_Stroika_Foundation_DataExchange_Compression_Reader_h_*/
