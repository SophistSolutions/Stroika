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
#include    "../Streams/BasicBinaryOutputStream.h"


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
                    fLogger_ (Execution::Logger::Priority::eCriticalError, Characters::Format (L"Error analyzing configuration file (bad format) '%s' - using defaults.", GetReadFilePath_ ().c_str ()));
                    return Optional<T> ();
                }
                catch (...) {
                    // if this fails, its probably because somehow the data in the config file was bad.
                    // So at least log that, and continue without reading anything (as if empty file)
                    fLogger_ (Execution::Logger::Priority::eCriticalError, Characters::Format (L"Error analyzing configuration file '%s' - using defaults.", GetReadFilePath_ ().c_str ()));
                    return Optional<T> ();
                }
            }
            template    <typename T>
            T   OptionsFile::Read (const T& defaultObj, ReadFlags readFlags)
            {
                Optional<T> eltRead = Read<T> ();
                Optional<T> elt2Write;              // only if needed
                String      msgAugment;
                if (eltRead.IsMissing ()) {
                    if (readFlags == ReadFlags::eWriteIfChanged) {
                        elt2Write = defaultObj;
                        msgAugment = L"default";
                    }
                }
                else {
                    if (readFlags == ReadFlags::eWriteIfChanged) {
                        try {
                            // See if re-persisting the item would change it.
                            // This is useful if your data model adds or removes fields. It updates the file contents written to the
                            // upgraded/latest form
                            Memory::BLOB    oldData =   ReadRaw (); // @todo could have saved from previous Read<T>
                            Memory::BLOB    newData;
                            {
                                Streams::BasicBinaryOutputStream outStream;
                                fWriter_.Write (fMapper_.FromObject (*eltRead), outStream);
                                // not sure needed? outStream.Flush();
                                newData = outStream.As<Memory::BLOB> ();
                            }
                            if (oldData != newData) {
                                elt2Write = eltRead;
                                msgAugment = L"(because something changed)";
                            }
                        }
                        catch (...) {
                            fLogger_ (Execution::Logger::Priority::eError, Characters::Format (L"Failed to compare configuration file: %s", GetReadFilePath_ ().c_str ()));
                        }
                    }
                }
                if (elt2Write.IsPresent ()) {
                    fLogger_ (Execution::Logger::Priority::eInfo, Characters::Format (L"Writing %s '%s' configuration file.", msgAugment.c_str (), GetWriteFilePath_ ().c_str ()));
                    try {
                        Write (*elt2Write);
                    }
                    catch (...) {
                        fLogger_ (Execution::Logger::Priority::eError, Characters::Format (L"Failed to write default values to file: %s", GetWriteFilePath_ ().c_str ()));
                    }
                    return *elt2Write;
                }
                else if (eltRead.IsPresent ()) {
                    return *eltRead;
                }
                else {
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
