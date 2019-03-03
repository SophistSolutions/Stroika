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
