/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileAccessException_h_
#define _Stroika_Foundation_IO_FileAccessException_h_ 1

#include "../StroikaPreComp.h"

#include <optional>

#include "../Characters/String.h"
#include "../Execution/Exceptions.h"
#include "../Memory/Optional.h"

#include "FileAccessMode.h"

namespace Stroika::Foundation::IO {

    using Characters::String;

    /**
     * This exception is thrown when a given file is opened, or creation attempted, etc. It is a failure due to
     * file (or directory) access permissions. It nearly always is the result of an operation (attempted and failed)
     * on a given file (which is usually given in the object). It also is the result of a perticular operation/access
     * failure (like read, write, or list).
     *
     *  // @todo - this should subclass from Execution::Exception<filesystem::filesystem_error> - but needs some work...
     */
    class FileAccessException : public Execution::SystemErrorException<> {
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

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "FileAccessException.inl"

#endif /*_Stroika_Foundation_IO_FileAccessException_h_*/
