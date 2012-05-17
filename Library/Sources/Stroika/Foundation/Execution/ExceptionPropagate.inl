/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroia_Foundation_Execution_Propagate_inl_
#define _Stroia_Foundation_Execution_Propagate_inl_ 1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


#if     qPlatform_Windows
#define CATCH_AND_CAPTURE_CATCH_BLOCK_PLATFORM_WINDOWS_PART_(CE)\
    catch (const Stroika::Foundation::Execution::Platform::Windows::HRESULTErrorException& e) {\
        (CE).fHRESULTErrorException = Stroika::Foundation::Memory::SharedPtr<Stroika::Foundation::Execution::Platform::Windows::HRESULTErrorException> (DEBUG_NEW Stroika::Foundation::Execution::Platform::Windows::HRESULTErrorException (e));\
    }\
    catch (const Stroika::Foundation::Execution::Platform::Windows::Exception& e) {\
        (CE).fWin32ErrorException = Stroika::Foundation::Memory::SharedPtr<Stroika::Foundation::Execution::Platform::Windows::Exception> (DEBUG_NEW Stroika::Foundation::Execution::Platform::Windows::Exception (e));\
    }
#else
#define CATCH_AND_CAPTURE_CATCH_BLOCK_PLATFORM_WINDOWS_PART_(CE)
#endif

#define CATCH_AND_CAPTURE_CATCH_BLOCK(CE)\
    catch (const Stroika::Foundation::Execution::RequiredComponentMissingException& e) {\
        (CE).fRequiredComponentMissingException = Stroika::Foundation::Memory::SharedPtr<Stroika::Foundation::Execution::RequiredComponentMissingException> (DEBUG_NEW Stroika::Foundation::Execution::RequiredComponentMissingException (e));\
    }\
    CATCH_AND_CAPTURE_CATCH_BLOCK_PLATFORM_WINDOWS_PART_(CE)\
    catch (const Stroika::Foundation::IO::FileFormatException& e) {\
        (CE).fFileFormatException = Stroika::Foundation::Memory::SharedPtr<Stroika::Foundation::IO::FileFormatException> (DEBUG_NEW Stroika::Foundation::IO::FileFormatException (e));\
    }\
    catch (const Stroika::Foundation::IO::FileBusyException& e) {\
        (CE).fFileBusyException = Stroika::Foundation::Memory::SharedPtr<Stroika::Foundation::IO::FileBusyException> (DEBUG_NEW Stroika::Foundation::IO::FileBusyException (e));\
    }\
    catch (const Stroika::Foundation::Execution::SilentException& e) {\
        (CE).fSilentException = Stroika::Foundation::Memory::SharedPtr<Stroika::Foundation::Execution::SilentException> (DEBUG_NEW Stroika::Foundation::Execution::SilentException (e));\
    }\
    catch (const Stroika::Foundation::Execution::StringException& e) {\
        (CE).fStringException = Stroika::Foundation::Memory::SharedPtr<Stroika::Foundation::Execution::StringException> (DEBUG_NEW Stroika::Foundation::Execution::StringException (e));\
    }\
    catch (...) {\
        (CE).fStringException = Stroika::Foundation::Memory::SharedPtr<Stroika::Foundation::Execution::StringException> (DEBUG_NEW Stroika::Foundation::Execution::StringException (L"Unknown Exception"));\
    }\
     

        }
    }
}

#endif  /*_Stroia_Foundation_Execution_Propagate_inl_*/
