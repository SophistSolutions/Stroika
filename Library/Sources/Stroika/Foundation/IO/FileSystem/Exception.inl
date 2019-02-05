/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_Exception_inl_
#define _Stroika_Foundation_IO_FileSystem_Exception_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::IO::FileSystem {

    /*
     ********************************************************************************
     ***************************** FileSystem::Exception ***************************
     ********************************************************************************
     */
    inline Exception::Exception (error_code errCode, const path& p1, const path& p2)
        : inherited (mkMsg_ (errCode, p1, p2), this->_PeekAtSDKString_ (), p1, p2, errCode)
    {
        Require (not p1.empty () or p2.empty ()); // if only one path provided, provide it first
    }
    inline Exception::Exception (error_code errCode, const Characters::String& message, const path& p1, const path& p2)
        : inherited (mkMsg_ (errCode, message, p1, p2), this->_PeekAtSDKString_ (), p1, p2, errCode)
    {
        Require (not p1.empty () or p2.empty ()); // if only one path provided, provide it first
    }

}

#endif /*_Stroika_Foundation_IO_FileSystem_Exception_inl_*/
