/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileAccessException_inl_
#define _Stroika_Foundation_IO_FileAccessException_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::IO {

    using Characters::String;

    /**
     *  // @todo - this should subclass from Execution::Exception<filesystem::filesystem_error> - but needs some work...
     */
    class [[deprecated ("use IO::FileSystem::Exception instead (with make_error_code (errc::permission_denied) )- Since Stroika v2.1d18")]] FileAccessException : public Execution::SystemErrorException<>
    {
    private:
        using inherited = Execution::SystemErrorException<>;

    public:
        /**
         *  This is almost equally likely to be errc::permission_denied or errc::no_such_file_or_directory. So best to call
         *  with explicit error_code, but if not we can guess.
         */
        FileAccessException (const optional<String>& fileName = nullopt, const optional<FileAccessMode>& fileAccessMode = nullopt);
        FileAccessException (error_code ec, const optional<String>& fileName = nullopt, const optional<FileAccessMode>& fileAccessMode = nullopt);

    public:
        /**
         *  If filename not specified, empty string returned (backwards compat)
         */
        nonvirtual optional<String> GetFileName () const;

    public:
        /**
         *  If fileAccessMode not specified, FileAccessMode::eReadWrite returned (backwards compat)
         */
        nonvirtual optional<FileAccessMode> GetFileAccessMode () const;

    public:
        nonvirtual FileAccessMode GetFileAccessModeValue (FileAccessMode defaultVal = FileAccessMode::eReadWrite) const { return fFileAccessMode_.value_or (defaultVal); }

    private:
        optional<String>         fFileName_;
        optional<FileAccessMode> fFileAccessMode_;
    };

        /**
     *  ** DEPRECATED - use DeclareActivity or IO::FileSystem::Exception::Throw....if with pathname args
     */
#define Stroika_Foundation_IO_FileAccessException_CATCH_REBIND_FILENAME_ACCCESS_HELPER(USEFILENAME, USEACCESSMODE) \
    catch (const Stroika::Foundation::IO::FileAccessException& e)                                                  \
    {                                                                                                              \
        Stroika::Foundation::Execution::Throw (                                                                    \
            Stroika::Foundation::IO::FileAccessException (                                                         \
                (Memory::OptionalValue (optional<Characters::String>{USEFILENAME}, e.GetFileName ())),             \
                (Memory::OptionalValue (optional<IO::FileAccessMode>{USEACCESSMODE}, e.GetFileAccessMode ()))));   \
    }

        /**
     *  ** DEPRECATED - use DeclareActivity or IO::FileSystem::Exception::Throw....if with pathname args
      */
#define Stroika_Foundation_IO_FileAccessException_CATCH_REBIND_FILENAMESONLY_HELPER(USEFILENAME) \
    Stroika_Foundation_IO_FileAccessException_CATCH_REBIND_FILENAME_ACCCESS_HELPER (USEFILENAME, e.GetFileAccessMode ())

    /*
     ********************************************************************************
     ***************************** IO::FileAccessException **************************
     ********************************************************************************
     */
    inline optional<String> FileAccessException::GetFileName () const
    {
        return fFileName_;
    }
    inline optional<FileAccessMode> FileAccessException::GetFileAccessMode () const
    {
        return fFileAccessMode_;
    }
    inline FileAccessException::FileAccessException (const optional<String>& fileName, const optional<FileAccessMode>& fileAccessMode)
        : FileAccessException (make_error_code (errc::permission_denied), fileName, fileAccessMode)
    {
    }

}

#endif /*_Stroika_Foundation_IO_FileAccessException_inl_*/
