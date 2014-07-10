/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_OptionsFile_h_
#define _Stroika_Foundation_DataExchange_OptionsFile_h_    1

#include    "../StroikaPreComp.h"

#include    "../Characters/String.h"
#include    "../Configuration/Version.h"
#include    "../Memory/BLOB.h"
#include    "../Memory/Optional.h"

#include    "ObjectVariantMapper.h"
#include    "Reader.h"
#include    "VariantValue.h"
#include    "Writer.h"


/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 * TODO:
 *
 *      @todo   Provide some (straight forward and semi-automatic) mechanism so that
 *              template    <>
 *              Optional<VariantValue>  OptionsFile::Read ()
 *
 *              knows about the correct version (if you use a versioning scheme it controls - like the filesystem
 *              filename scheme I used for Block Engineering).
 *
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
                /**
                 */
                struct  LoggerMessage {
                    enum    class   Msg {
                        eFailedToWriteFile,
                        eFailedToReadFile,
                        eFailedToParseReadFile,
                        eFailedToParseReadFileBadFormat,
                        eFailedToCompareReadFile,
                        eWritingConfigFile_SoDefaultsEditable,
                        eWritingConfigFile_BecauseUpgraded,
                        eWritingConfigFile_BecauseSomethingChanged,
                        eFailedToWriteInUseValues,
                    };
                    Msg                 fMsg;
                    Optional<String>    fFileName;

                    LoggerMessage (Msg msg, String fn);
                    nonvirtual  String  FormatMessage () const;
                };
            public:
                using LoggerType = function<void(const LoggerMessage& message)>;
                static  const   LoggerType  kDefaultLogger;

            public:
                /**
                 */
                using ModuleNameToFileNameMapperType = function<String(const String& moduleName, const String& fileSuffix)>;

            public:
                /**
                 */
                static  const   ModuleNameToFileNameMapperType  mkFilenameMapper (const String& appName);

            public:
                /**
                 *  This function gets a crack at revising the form (in variant/tree form) of the data to accomodate a version
                 *  change. The version may not be known, in which case the function is still called, but with the verison information
                 *  missing.
                 */
                using ModuleDataUpgraderType = function<VariantValue(const Memory::Optional<Configuration::Version>& version, const VariantValue& rawVariantValue)>;

            public:
                /**
                 *  kDefaultUpgrader does nothing (no-op)
                 */
                static  const   ModuleDataUpgraderType  kDefaultUpgrader;

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
                /**
                 */
                OptionsFile (
                    const String& modName,
                    const ObjectVariantMapper& mapper,
                    ModuleDataUpgraderType moduleUpgrader = kDefaultUpgrader,
                    ModuleNameToFileNameMapperType moduleNameToFileNameMapper = mkFilenameMapper (L"Put-Your-App-Name-Here"),
                    LoggerType logger = kDefaultLogger,
                    Reader reader = kDefaultReader,
                    Writer writer = kDefaultWriter
                );
                OptionsFile (
                    const String& modName,
                    const ObjectVariantMapper& mapper,
                    ModuleDataUpgraderType moduleUpgrader,
                    ModuleNameToFileNameMapperType moduleNameToReadFileNameMapper,
                    ModuleNameToFileNameMapperType moduleNameToWriteFileNameMapper,
                    LoggerType logger = kDefaultLogger,
                    Reader reader = kDefaultReader,
                    Writer writer = kDefaultWriter
                );
                OptionsFile (
                    const String& modName,
                    const ObjectVariantMapper& mapper,
                    ModuleDataUpgraderType moduleUpgrader,
                    ModuleNameToFileNameMapperType moduleNameToReadFileNameMapper,
                    ModuleNameToFileNameMapperType moduleNameToWriteFileNameMapper,
                    LoggerType logger,
                    Reader reader,
                    Writer writer,
                    const String& fileSuffix
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
                 *  Just a parameter to Read() function.
                 */
                enum    class   ReadFlags {
                    eNoWriteIfChanged,
                    eWriteIfChanged,
                };

            public:
                /**
                 *  Note - predefined version Read<VariantValue> doesn't use ObjectVariantMapper.
                 *
                 *  Note also that Read<VariantValue> does use any provided ModuleDataUpgrader, so that the data is upgraded
                 *  before its seen by the object variant mapper.
                 *
                 *  This also emits warnings, and maps to 'isMissing' or default value (depending on overload) if read data is corrupted.
                 *  to avoid this behavior, use ReadRaw() directly.
                 */
                template    <typename T>
                nonvirtual  Optional<T>   Read ();
                template    <typename T>
                nonvirtual  T   Read (const T& defaultObj, ReadFlags readFlags = ReadFlags::eNoWriteIfChanged);

            public:
                /**
                 *  Note - predefined version Write<VariantValue> doesn't use ObjectVariantMapper
                 */
                template    <typename T>
                nonvirtual  void    Write (const T& optionsObject);

            private:
                nonvirtual  String  GetReadFilePath_ () const;
                nonvirtual  String  GetWriteFilePath_ () const;

            private:
                String                          fModuleName_;
                ObjectVariantMapper             fMapper_;
                ModuleDataUpgraderType          fModuleDataUpgrader_;
                ModuleNameToFileNameMapperType  fModuleNameToReadFileNameMapper_;
                ModuleNameToFileNameMapperType  fModuleNameToWriteFileNameMapper_;
                LoggerType                      fLogger_;
                Reader                          fReader_;
                Writer                          fWriter_;
                String                          fFileSuffix_;
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
