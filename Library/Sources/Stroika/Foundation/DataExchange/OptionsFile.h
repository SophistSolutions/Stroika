/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_OptionsFile_h_
#define _Stroika_Foundation_DataExchange_OptionsFile_h_    1

#include    "../StroikaPreComp.h"

#include    "../Characters/String.h"
#include    "../Configuration/Version.h"
#include    "../Memory/BLOB.h"
#include    "../Memory/Optional.h"

#include    "ObjectVariantMapper.h"
#include    "VariantReader.h"
#include    "VariantValue.h"
#include    "VariantWriter.h"


/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 * TODO:
 *
 *      @todo   eFailedToReadFile should be broken into failed to read, and failed to upgrade.
 *
 *      @todo   Look at http://s11n.net/
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
            using   Memory::BLOB;
            using   Memory::Optional;


            /**
             *  Simple wrapper on ObjectVariantMapper code, and code to serialize to/from JSON/XML etc, to wrap
             *  all this into an easy to use (but configurable/customizable) helper to serialize configuration data
             *  between disk and module-specific C++ objects.
             *
             *  This is SOMEWHAT flexible and customizable, but not so much. Its mostly intended to be a one-size-fits all
             *  utility that will often be useful, and will save some typing. If it does fit your needs, dont use it, and
             *  use ObjectVariantMapper and the reader abstractions directly.
             *
             *  \par Example Usage
             *      \code
             *      struct  MyData_ {
             *          bool                fEnabled = false;
             *          DateTime            fLastSynchronizedAt;
             *      };
             *      OptionsFile of {
             *          L"MyModule",
             *          [] () -> ObjectVariantMapper {
             *              ObjectVariantMapper mapper;
             *              mapper.AddClass<MyData_> (initializer_list<StructureFieldInfo> {
             *                  { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (MyData_, fEnabled), L"Enabled" },
             *                  { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (MyData_, fLastSynchronizedAt), L"Last-Synchronized-At" },
             *              });
             *              return mapper;
             *          } (),
             *          OptionsFile::kDefaultUpgrader,
             *          [] (const String & moduleName, const String & fileSuffix) -> String {
             *              return  IO::FileSystem::WellKnownLocations::GetTemporary () + moduleName;
             *          }
             *      };
             *      MyData_ m = of.Read<MyData_> (MyData_ ());  // will return default values if file not present
             *      of.Write (m);                               // test writing
             *      \endcode
             */
            class   OptionsFile {
            public:
                /**
                 */
                struct  LoggerMessage;

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
                 *  Optionally provide (based on filenames/locations) information about the incoming file(read file)
                 *  version.
                 */
                using ModuleNameToFileVersionMapperType = function<Optional<Configuration::Version>(const String& moduleName)>;

            public:
                /**
                 */
                static  const   ModuleNameToFileVersionMapperType   kDefaultModuleNameToFileVersionMapper;

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
                static  const   VariantReader  kDefaultReader;

            public:
                /**
                 *  Format serializer
                 */
                static  const   VariantWriter  kDefaultWriter;

            public:
                /**
                 */
                OptionsFile (
                    const String& modName,
                    const ObjectVariantMapper& mapper,
                    ModuleDataUpgraderType moduleUpgrader = kDefaultUpgrader,
                    ModuleNameToFileNameMapperType moduleNameToFileNameMapper = mkFilenameMapper (L"Put-Your-App-Name-Here"),
                    ModuleNameToFileVersionMapperType moduleNameToReadFileVersion = kDefaultModuleNameToFileVersionMapper,
                    LoggerType logger = kDefaultLogger,
                    VariantReader reader = kDefaultReader,
                    VariantWriter writer = kDefaultWriter
                );
                OptionsFile (
                    const String& modName,
                    const ObjectVariantMapper& mapper,
                    ModuleDataUpgraderType moduleUpgrader,
                    ModuleNameToFileNameMapperType moduleNameToReadFileNameMapper,
                    ModuleNameToFileNameMapperType moduleNameToWriteFileNameMapper,
                    ModuleNameToFileVersionMapperType moduleNameToReadFileVersion = kDefaultModuleNameToFileVersionMapper,
                    LoggerType logger = kDefaultLogger,
                    VariantReader reader = kDefaultReader,
                    VariantWriter writer = kDefaultWriter
                );
                OptionsFile (
                    const String& modName,
                    const ObjectVariantMapper& mapper,
                    ModuleDataUpgraderType moduleUpgrader,
                    ModuleNameToFileNameMapperType moduleNameToReadFileNameMapper,
                    ModuleNameToFileNameMapperType moduleNameToWriteFileNameMapper,
                    ModuleNameToFileVersionMapperType moduleNameToReadFileVersion,
                    LoggerType logger,
                    VariantReader reader,
                    VariantWriter writer,
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
                 *  Read () reads in the data from the associated options file, and returns it as a struct you've defined.
                 *
                 *  The mapping to/from external format is defined by OptionsFile constructor parameters.
                 *
                 *  The predefined (template specialization) of Read<VariantValue> doesn't use ObjectVariantMapper:
                 *  it does the raw read (just using the Reader object).
                 *
                 *  Note also that Read<VariantValue> does use any provided ModuleDataUpgrader, so that the data is upgraded
                 *  before its seen by the object variant mapper.
                 *
                 *  Read () will emit warnings, and maps to 'isMissing' or default value (depending on overload) if
                 *  read data is corrupted. To avoid this behavior, use ReadRaw() directly.
                 */
                template    <typename T>
                nonvirtual  Optional<T>   Read ();
                template    <typename T>
                nonvirtual  T   Read (const T& defaultObj, ReadFlags readFlags = ReadFlags::eWriteIfChanged);

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
                String                              fModuleName_;
                ObjectVariantMapper                 fMapper_;
                ModuleDataUpgraderType              fModuleDataUpgrader_;
                ModuleNameToFileNameMapperType      fModuleNameToReadFileNameMapper_;
                ModuleNameToFileNameMapperType      fModuleNameToWriteFileNameMapper_;
                ModuleNameToFileVersionMapperType   fModuleNameToFileVersionMapper_;
                LoggerType                          fLogger_;
                VariantReader                       fReader_;
                VariantWriter                       fWriter_;
                String                              fFileSuffix_;
            };


            /**
             */
            struct  OptionsFile::LoggerMessage {
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
