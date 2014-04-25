/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_OptionsFile_h_
#define _Stroika_Foundation_DataExchange_OptionsFile_h_    1

#include    "../StroikaPreComp.h"

#include    "../Characters/String.h"
#include    "../Memory/BLOB.h"
#include    "../Memory/Optional.h"

#include    "ObjectVariantMapper.h"
#include    "Reader.h"
#include    "VariantValue.h"
#include    "Writer.h"


/**
 *  \file
 *
 * TODO:
 *      @todo   UNDOCUMENTED - VERY PRELIMINARY DRAFT
 *
 *      @todo   Biggest thing needed is to think out erorr handling/reporting...
 *
 *  GIST:
 *      if you have mapper object for type T) – one line create object that writes/reads
 *      it to/from filessytem (defaults to good places) – and handles logging warning if created,
 *      or bad, and rewrites for if changes detected (added/renamed etc).
 *      Maybe hook / configure options for these features.
 *
 *
 *
 *  \em Design Note:
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {


            using   Characters::String;
            using   Memory::BLOB;
            using   Memory::Optional;


            /**
             *
             *  @todo document, test, and complete (so far - just a prototype)
             */
            class   OptionsFile {
            public:
                using LoggerType = function<void(const String& errorMessage)>;
                static  const   LoggerType  kDefaultLogger;

            public:
                using ModuleNameToFileNameMapperType = function<String(const String& errorMessage)>;
                static  const   ModuleNameToFileNameMapperType  mkFilenameMapper (const String& appName);

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
                OptionsFile (
                    const String& modName,
                    const ObjectVariantMapper& mapper,
                    ModuleNameToFileNameMapperType moduleNameToFileNameMapper = mkFilenameMapper (L"Put-Your-App-Name-Here"),
                    LoggerType logWarning = kDefaultLogger,
                    LoggerType logError = kDefaultLogger,
                    Reader reader = kDefaultReader,
                    Writer writer = kDefaultWriter
                );

            public:
                /**
                 *
                 */
                nonvirtual  BLOB    ReadRaw () const;

            public:
                /**
                 *
                 */
                nonvirtual  void    WriteRaw (const BLOB& blob);

            public:
                /**
                 *  Note - predefined version Read<VariantValue> doesnt use ObjectVariantMapper
                 */
                template    <typename T>
                nonvirtual  Optional<T>   Read ();
                template    <typename T>
                nonvirtual  T   Read (const T& defaultObj);

            public:
                /**
                 *  Note - predefined version Write<VariantValue> doesn't use ObjectVariantMapper
                 */
                template    <typename T>
                nonvirtual  void    Write (const T& optionsObject);

            private:
                String                          fModuleName_;
                ObjectVariantMapper             fMapper_;
                ModuleNameToFileNameMapperType  fModuleNameToFileNameMapper_;
                LoggerType                      fLogWarning_;
                LoggerType                      fLogError_;
                Reader                          fReader_;
                Writer                          fWriter_;
            };


            template    <>
            Optional<VariantValue>  OptionsFile::Read ();
            template    <>
            void                    OptionsFile::Write (const VariantValue& optionsObject);


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
