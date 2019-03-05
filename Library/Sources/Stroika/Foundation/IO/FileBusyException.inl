/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileBusyException_inl_
#define _Stroika_Foundation_IO_FileBusyException_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::IO {

    /*
     ********************************************************************************
     ******************************** FileBusyException *****************************
     ********************************************************************************
     */
    /**
      */
    class [[deprecated ("deprecated - use IO::FileSystem::Exception to capture filename or Execution::SystemErrorException otherwise, and iether way make_error_code (errc::device_or_resource_busy) or make_error_code (errc::text_file_busy) Since Stroika v2.1d18")]] FileBusyException : public Execution::RuntimeErrorException<>
    {
    private:
        using inherited = Execution::RuntimeErrorException<>;

    public:
        FileBusyException (const String& fileName = String ());

    public:
        nonvirtual String GetFileName () const
        {
            return fFileName_;
        }

    private:
        String fFileName_;
    };

}

#endif /*_Stroika_Foundation_IO_FileBusyException_inl_*/
