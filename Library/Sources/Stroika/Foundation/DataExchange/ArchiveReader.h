/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_ArchiveReader_h_
#define _Stroika_Foundation_DataExchange_ArchiveReader_h_    1

#include    "../StroikaPreComp.h"

#include    <istream>

#include    "../Memory/SharedByValue.h"
#include    "../Streams/InputStream.h"

#include    "VariantValue.h"


/**
 *  \file
 *
 * TODO:
 *      @todo   UNDOCUMENTED - PRELIMINARY
 *
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {


            using   Containers::Set;
            using   Characters::String;
            using   Memory::BLOB;


            /**
             */
            class   ArchiveReader {
            protected:
                class   _IRep;

            public:
                ArchiveReader () = delete;
                ArchiveReader (const ArchiveReader&) = delete;
                ArchiveReader (ArchiveReader&& src);
                ArchiveReader& operator= (const ArchiveReader&) = delete;

            protected:
                explicit ArchiveReader (const shared_ptr<_IRep>& rep);

            public:
                /**
                 */
                nonvirtual  Set<String>     GetContainedFiles () const;

            public:
                /**
                 *  maybe define iterator/indexes and maybe have return stream
                 */
                nonvirtual  BLOB    GetData (const String& fileName) const;

            protected:
                nonvirtual  _IRep&          _GetRep ();
                nonvirtual  const _IRep&    _GetRep () const;

            private:
                shared_ptr<_IRep>   fRep_;
            };


            /**
             */
            class   ArchiveReader::_IRep {
            public:
                virtual Set<String> GetContainedFiles () const      =   0;
                virtual BLOB        GetData (const String& fileName) const = 0;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "ArchiveReader.inl"

#endif  /*_Stroika_Foundation_DataExchange_ArchiveReader_h_*/
