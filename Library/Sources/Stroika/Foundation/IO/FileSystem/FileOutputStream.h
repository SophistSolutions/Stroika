/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_FileOutputStream_h_
#define _Stroika_Foundation_IO_FileSystem_FileOutputStream_h_  1

#include    "../../StroikaPreComp.h"

#include    "../../Characters/String.h"
#include    "../../Streams/OutputStream.h"

#include    "../FileAccessMode.h"



/**
 *  \file
 *
 * TODO:
 *      @todo   Use Exeuction??xx?? - caller - handler for thread interupts..
 *
 *      @todo   Add enum BufferFlag { eBuffered, eUnbuffered }; to mk () method, and like we have
 *              with FileOutputStream
 *
 *      @todo   Add method to get access to FD  - PeekAtFD(), GetFD()???)
 *
 *      @todo   Unclear if we need the mutexes here. Probably yes (necause our API promises re-entrancy but I'm unclear
 *              on filessytem reads/writes).
 *
 *      @todo   This code is just a first (probably working) draft. But it needs cleanup. Review
 *              older FileReader/FileWriter code - and see if the windows code should use CreateFile instead of s_open...
 *
 *      @todo   We need a BinaryFileInputOutputStream
 *
 *      @todo   Document these are not buffered, (well document). Document how easy to wrap buffered stream around.
 *
 *      @todo   Add optional file sharing parameters to stream readers, and maybe file-descriptor CTOR?
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   FileSystem {


                using   Characters::String;


                /**
                 *  EXAMPLE USAGE:
                 *      String fileName = IO::FileSystem::WellKnownLocations::GetTemporary () + L"t.txt";
                 *      JSON::Writer ().Write (v, IO::FileSystem::FileOutputStream::mk (fileName));
                 */
                class   FileOutputStream : public Streams::OutputStream<Memory::Byte> {
                private:
                    using   inherited   =   OutputStream<Memory::Byte>;

                public:
                    /**
                     *  This flag is used to configure if BinaryOutputStream::Flush will invoke the OS fsync() funciton
                     *  to force data to disk (by default Flush just forces the data out of this object to the next object,
                     *  for files, the operating system).
                     */
                    enum class FlushFlag {
                        eToOperatingSystem,
                        eToDisk
                    };

                public:
                    FileOutputStream (const String& fileName, FlushFlag flushFlag = FlushFlag::eToOperatingSystem);

                public:
                    static  OutputStream<Memory::Byte>   mk (const String& fileName, FlushFlag flushFlag = FlushFlag::eToOperatingSystem);

                private:
                    class   Rep_;
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
#include    "FileOutputStream.inl"

#endif  /*_Stroika_Foundation_IO_FileSystem_FileOutputStream_h_*/
