/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_OptionsFile_h_
#define _Stroika_Foundation_DataExchange_OptionsFile_h_    1

#include    "../StroikaPreComp.h"

#include    "../Characters/String.h"

#include    "ObjectVariantMapper.h"
#include    "Reader.h"
#include    "Writer.h"



/**
 *  \file
 *
 * TODO:
 *      @todo   UNDOCUMENTED - VERY PRELIMINARY DRAFT
 *
 *  GIST:
 *      if you have mapper object for type T) – one line create object that writes/reads
 *      it to/from filessytem (defaults to good places) – and handles logging warning if created,
 *      or bad, and rewrites for if changes detected (added/renamed etc).
 *      Maybe hook / configure options for these features.
 *
 *  \em Design Note:
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {


            using   Characters::String;


            /**
             *
             *  @todo Add optional function objects to map modulename to IStream/OStream objects.
             *          do in a way that knows about filenamea nd can do write through temporearte
             */
            class   OptionsFile {
            public:
                using LoggerType = function<void(const String& errorMessage)>;
                static  const   LoggerType  kDefaultLogger;

            public:
                /**
                 *  Format serializer
                 */
                static  const   Reader  kDefaultReader;

            public:
                /**
                 *  Format serializer
                 */
                static  const   Writer  kDefaultWriter;

            public:
                //OptionsFile (modName, objmapper, info-to-find-backupdir, otpions-about-logigng);
                OptionsFile (
                    const String& modName,
                    const ObjectVariantMapper& mapper,
                    LoggerType logger = kDefaultLogger
                );

            public:
                template    <typename T>
                nonvirtual  T   Read (T defaultObj = T());

            public:
                template    <typename T>
                nonvirtual  void    Write (T optionsObject);


            private:
                String                  fModuleName_;
                ObjectVariantMapper     fMapper_;
                LoggerType              fLogError_;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "OptionsFile.inl"

#endif  /*_Stroika_Foundation_DataExchange_OptionsFile_h_*/
