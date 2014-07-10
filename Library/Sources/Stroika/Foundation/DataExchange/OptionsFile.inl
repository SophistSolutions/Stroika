/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_OptionsFile_inl_
#define _Stroika_Foundation_DataExchange_OptionsFile_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Characters/Format.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {


            /*
             ********************************************************************************
             ************************** DataExchange::OptionsFile ***************************
             ********************************************************************************
             */
            template    <typename T>
            Optional<T>   OptionsFile::Read ()
            {
                Optional<VariantValue>  tmp = Read<VariantValue> ();
                if (tmp.IsMissing ()) {
                    return Optional<T> ();
                }
                try {
                    return fMapper_.ToObject<T> (*tmp);
                }
                catch (const BadFormatException& bf) {
                    fLogger_ (Execution::Logger::Priority::eCriticalError, Characters::Format (L"Error analyzing configuration file (bad format) '%s' - using defaults.", GetFilePath_ ().c_str ()));
                    return Optional<T> ();
                }
                catch (...) {
                    // if this fails, its probably because somehow the data in the config file was bad.
                    // So at least log that, and continue without reading anything (as if empty file)
                    fLogger_ (Execution::Logger::Priority::eCriticalError, Characters::Format (L"Error analyzing configuration file '%s' - using defaults.", GetFilePath_ ().c_str ()));
                    return Optional<T> ();
                }
            }
            template    <typename T>
            T   OptionsFile::Read (const T& defaultObj)
            {
                Optional<T> result = Read<T> ();
                if (result.IsPresent ()) {
                    return *result;
                }
                else {
                    try {
                        Write (defaultObj);
                    }
                    catch (...) {
                        fLogger_ (Execution::Logger::Priority::Characters::Format (L"Failed to write default values to file: %s", GetFilePath_ ().c_str ()));
                    }
                    return defaultObj;
                }
            }
            template    <typename T>
            void    OptionsFile::Write (const T& optionsObject)
            {
                Write<VariantValue> (fMapper_.FromObject<T> (optionsObject));
            }


        }

    }
}
#endif  /*_Stroika_Foundation_DataExchange_OptionsFile_inl_*/
